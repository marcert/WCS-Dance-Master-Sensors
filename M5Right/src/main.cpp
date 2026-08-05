#include <M5Unified.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// --- CONFIGURATION ---
#define FOOT_ID 2              // 1 = Left, 2 = Right
#define SAMPLE_RATE_MS 10        // 100 Hz (10 ms)
#define DISPLAY_TIMEOUT_MS 30000 // 30 seconds display activity

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// --- DATA STRUCTURE TRANSMIT PACKET (IMU) ---
typedef struct struct_imu_data {
  uint8_t foot_id;
  float gyro_x;  // Roll-off value (Y-rotation of sensor coordinate system)
  float accel_z; // Vertical acceleration on foot strike
} struct_imu_data;

// --- DATA STRUCTURE RECEIVE PACKET (HANDSHAKE ACK FROM MASTER) ---
typedef struct struct_handshake_ack {
  uint8_t master_channel;
  uint8_t confirmed;
} struct_handshake_ack;

struct_imu_data sensorData;
esp_now_peer_info_t peerInfo;

// Status variables for handshake & display
volatile bool masterAckReceived = false;
volatile uint8_t masterConfirmedChannel = 0;

unsigned long lastBatteryUpdate = 0;
unsigned long displayTurnOnTime = 0;
bool isDisplayOn = true;
int bgColour = BLACK;

void updateBatteryDisplay() {
  int level = M5.Power.getBatteryLevel();
  
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE, bgColour);
  
  char battBuf[16];
  snprintf(battBuf, sizeof(battBuf), "BATT: %d%% ", level);
  M5.Display.drawString(battBuf, M5.Display.width() / 2, 95);
}

// --- ESP-NOW RECEIVE CALLBACK FOR FINGERPRINT / ACK FROM MASTER ---
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
#else
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
#endif
  if (len == sizeof(struct_handshake_ack)) {
    struct_handshake_ack ack;
    memcpy(&ack, data, sizeof(ack));
    if (ack.confirmed == 1) {
      masterConfirmedChannel = ack.master_channel;
      masterAckReceived = true;
    }
  }
}

void setup() {
  // 1. Immediately reduce CPU frequency (saves significant battery, 80 MHz is easily sufficient for 100Hz IMU)
  setCpuFrequencyMhz(80);

  auto cfg = M5.config();
  cfg.internal_imu = true;
  M5.begin(cfg);

  // --- DISPLAY INITIALIZATION ---
  M5.Display.setRotation(1);               // Landscape mode
  M5.Display.setTextDatum(middle_center);  // Centered text
  M5.Display.setBrightness(64);            // Gedimmt auf ca. 25% (Spart Akku, gut lesbar)
  
  bgColour = (FOOT_ID == 1) ? BLUE : RED;
  
  M5.Display.fillScreen(bgColour);
  M5.Display.setTextColor(WHITE, bgColour);
  
  M5.Display.setTextSize(2);
  M5.Display.drawString((FOOT_ID == 1) ? "FOOT: LEFT" : "FOOT: RIGHT", M5.Display.width() / 2, 25);
  
  updateBatteryDisplay();
  displayTurnOnTime = millis();

  // --- ESP-NOW & WIFI SETUP ---
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // 2. Reduce transmit power to 11 dBm (sufficient for 3-5m, saves power)
  WiFi.setTxPower(WIFI_POWER_11dBm);

  if (esp_now_init() != ESP_OK) {
    M5.Display.setTextSize(1);
    M5.Display.drawString("ESP-NOW Error!", M5.Display.width() / 2, 55);
    return;
  }

  // Register receive callback to evaluate master responses during scan loop
  esp_now_register_recv_cb(OnDataRecv);

  // --- CHANNEL SCAN (SCAN CHANNEL 1 TO 13) ---
  M5.Display.setTextSize(1);
  M5.Display.drawString("Searching Master...", M5.Display.width() / 2, 55);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.encrypt = false;

  sensorData.foot_id = FOOT_ID;
  sensorData.gyro_x = 0;
  sensorData.accel_z = 1.0;

  uint8_t foundChannel = 0;

  for (uint8_t ch = 1; ch <= 13; ch++) {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    
    peerInfo.channel = ch;

    if (esp_now_is_peer_exist(broadcastAddress)) {
      esp_now_del_peer(broadcastAddress);
    }
    esp_now_add_peer(&peerInfo);

    delay(10); // Settling time for RF module

    // Start 3 attempts on each channel and wait for genuine Master ACK
    for (int attempt = 0; attempt < 3; attempt++) {
      masterAckReceived = false;
      esp_now_send(broadcastAddress, (uint8_t *)&sensorData, sizeof(sensorData));

      uint32_t startWait = millis();
      while (millis() - startWait < 30) { // Wait 30 ms for response packet from Master
        if (masterAckReceived) {
          foundChannel = (masterConfirmedChannel > 0) ? masterConfirmedChannel : ch;
          break;
        }
        delay(1);
      }
      if (foundChannel > 0) break;
      delay(5);
    }

    if (foundChannel > 0) break;
  }

  // --- LOCK CHANNEL RESULT ---
  M5.Display.fillScreen(bgColour);
  M5.Display.setTextSize(2);
  M5.Display.drawString((FOOT_ID == 1) ? "FOOT: LEFT" : "FOOT: RIGHT", M5.Display.width() / 2, 25);

  M5.Display.setTextSize(1);
  if (foundChannel > 0) {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(foundChannel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    peerInfo.channel = foundChannel;
    if (esp_now_is_peer_exist(broadcastAddress)) {
      esp_now_del_peer(broadcastAddress);
    }
    esp_now_add_peer(&peerInfo);

    String chInfo = "Master on Ch " + String(foundChannel);
    M5.Display.drawString(chInfo, M5.Display.width() / 2, 55);
  } else {
    // Fallback to Channel 1 if Master was not powered on yet
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    peerInfo.channel = 1;
    if (esp_now_is_peer_exist(broadcastAddress)) {
      esp_now_del_peer(broadcastAddress);
    }
    esp_now_add_peer(&peerInfo);
    
    M5.Display.drawString("Fallback: Channel 1", M5.Display.width() / 2, 55);
  }

  displayTurnOnTime = millis();
}

void loop() {
  M5.update();

  // --- BUTTON A (FRONT BUTTON): TURN ON DISPLAY FOR 30 SECONDS ---
  if (M5.BtnA.wasPressed()) {
    isDisplayOn = true;
    displayTurnOnTime = millis();
    M5.Display.setBrightness(128);
    updateBatteryDisplay();
  }

  // --- BUTTON B (SIDE BUTTON): POWER OFF VIA LONG-PRESS ---
  if (M5.BtnB.wasHold()) {
    M5.Display.setBrightness(128);
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setTextSize(2);
    M5.Display.drawString("OFF...", M5.Display.width() / 2, M5.Display.height() / 2);
    delay(500);
    M5.Power.powerOff();
  }

  // --- DISPLAY TIMEOUT (AUTOMATICALLY TURN OFF AFTER 30 S) ---
  if (isDisplayOn && (millis() - displayTurnOnTime > DISPLAY_TIMEOUT_MS)) {
    isDisplayOn = false;
    M5.Display.setBrightness(0);
  }

  // --- UPDATE BATTERY DISPLAY (ONLY WHEN DISPLAY IS ACTIVE) ---
  if (isDisplayOn && (millis() - lastBatteryUpdate > 2000)) {
    lastBatteryUpdate = millis();
    updateBatteryDisplay();
  }

  // --- ACQUIRE IMU DATA AND SEND TO MASTER (RUNS CONTINUOUSLY) ---
  float gx, gy, gz;
  float ax, ay, az;

  M5.Imu.getGyro(&gx, &gy, &gz);
  M5.Imu.getAccel(&ax, &ay, &az);

  sensorData.gyro_x = gy; 
  sensorData.accel_z = az;

  esp_now_send(broadcastAddress, (uint8_t *)&sensorData, sizeof(sensorData));

  delay(SAMPLE_RATE_MS);
}
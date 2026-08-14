#include <M5Unified.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_sleep.h>

// --- CONFIGURATION ---
#define FOOT_ID 2              // 1 = Left, 2 = Right
// Axis inversion for the 180°-inverted right mounting is handled in the dashboard JS, not in firmware.
#define SAMPLE_RATE_MS 5       // 200 Hz sampling rate (5 ms)
#define DISPLAY_TIMEOUT_MS 30000 // 30 seconds display activity

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// --- DATA STRUCTURE TRANSMIT PACKET (IMU) ---
typedef struct struct_imu_data {
  uint8_t foot_id;
  float gyro_x;    // Pitch rotation (gy — roll-off)
  float accel_z;   // Vertical acceleration (impact)
  float accel_y;   // Longitudinal acceleration (forward/backward)
  float gyro_roll; // Lateral roll rotation (gx — pronation/supination)
  float accel_x;   // Lateral acceleration (for 3D impact magnitude)
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
volatile uint32_t lastMasterAckTime = 0; // Zeitstempel der letzten Master-Antwort
const uint32_t MASTER_TIMEOUT_MS = 3000;   // Nach 3s ohne ACK gilt der Master als offline

volatile bool pendingUIUpdate = false; // Flag für entkoppeltes UI Update

uint8_t currentChannel = 1;
bool masterConnected = false;
unsigned long lastRescanTime = 0;
const unsigned long RESCAN_INTERVAL_MS = 60000; // 60 Sekunden Re-Scan bei fehlender Verbindung

unsigned long lastBatteryUpdate = 0;
unsigned long displayTurnOnTime = 0;
bool isDisplayOn = true;
int bgColour = BLACK;

void updateChannelDisplay() {
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE, bgColour);
  
  M5.Display.fillRect(0, 45, M5.Display.width(), 30, bgColour);

  if (masterConnected) {
    char chBuf[32];
    snprintf(chBuf, sizeof(chBuf), "Ch: %d", currentChannel);
    M5.Display.drawString(chBuf, M5.Display.width() / 2, 60);
  } else {
    M5.Display.drawString("No Master", M5.Display.width() / 2, 60);
  }
}

void updateBatteryDisplay() {
  int level = M5.Power.getBatteryLevel();
  
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE, bgColour);
  
  char battBuf[16];
  snprintf(battBuf, sizeof(battBuf), "BATT: %d%% ", level);
  M5.Display.drawString(battBuf, M5.Display.width() / 2, 95);
}

uint8_t scanForMaster() {
  sensorData.foot_id = FOOT_ID;
  sensorData.gyro_x  = 0;
  sensorData.accel_z = 1.0;
  sensorData.accel_y = 0.0;

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.encrypt = false;

  uint8_t foundCh = 0;

  for (uint8_t ch = 1; ch <= 13; ch++) {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    
    peerInfo.channel = ch;

    if (esp_now_is_peer_exist(broadcastAddress)) {
      esp_now_del_peer(broadcastAddress);
    }
    esp_now_add_peer(&peerInfo);

    delay(15); // Einschwingzeit für Funkchip

    for (int attempt = 0; attempt < 5; attempt++) {
      masterAckReceived = false;
      esp_now_send(broadcastAddress, (uint8_t *)&sensorData, sizeof(sensorData));

      uint32_t startWait = millis();
      while (millis() - startWait < 50) { // 50ms Warten
        if (masterAckReceived) {
          foundCh = (masterConfirmedChannel > 0) ? masterConfirmedChannel : ch;
          break;
        }
        delay(1);
      }
      if (foundCh > 0) break;
      delay(5);
    }

    if (foundCh > 0) break;
  }

  return foundCh;
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
      lastMasterAckTime = millis(); // Zeitstempel aktualisieren

      // Wenn wir noch nicht connected waren, setzen wir nur das Flag und behandeln das UI in loop()!
      if (!masterConnected) {
        masterConnected = true;
        currentChannel = (ack.master_channel > 0) ? ack.master_channel : currentChannel;
        pendingUIUpdate = true;
      }
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
  M5.Display.setTextSize(2);
  M5.Display.drawString("Search Master...", M5.Display.width() / 2, 55);

  uint8_t foundChannel = scanForMaster();

  // --- LOCK CHANNEL RESULT ---
  M5.Display.fillScreen(bgColour);
  M5.Display.setTextSize(2);
  M5.Display.drawString((FOOT_ID == 1) ? "FOOT: LEFT" : "FOOT: RIGHT", M5.Display.width() / 2, 25);

    if (foundChannel > 0) {
    masterConnected = true;
    currentChannel = foundChannel;
    lastMasterAckTime = millis();
  } else {
    masterConnected = false;
    currentChannel = 1; // Default Fallback
  }

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  peerInfo.channel = currentChannel;
  if (esp_now_is_peer_exist(broadcastAddress)) {
    esp_now_del_peer(broadcastAddress);
  }
  esp_now_add_peer(&peerInfo);

  updateChannelDisplay();
  updateBatteryDisplay();

  lastRescanTime = millis();
  displayTurnOnTime = millis();
}

void loop() {
  M5.update();

  // --- ENT KOPPELTES UI UPDATE NACH VERBINDUNGSAUFBAU ---
  if (pendingUIUpdate) {
    pendingUIUpdate = false;
    if (isDisplayOn) {
      updateChannelDisplay();
    }
  }

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

    // --- CONNECTION TIMEOUT CHECK (PRÜFEN OB MASTER VERLOREN GEGANGEN IST) ---
    if (masterConnected && (millis() - lastMasterAckTime > MASTER_TIMEOUT_MS)) {
      masterConnected = false;
      lastRescanTime = millis(); // Sofortigen Rescan-Timer zurücksetzen
      if (isDisplayOn) {
        updateChannelDisplay();
      }
    }

    // --- AUTOMATIC RE-SCAN EVERY 60s IF NO MASTER CONNECTED ---
    if (!masterConnected && (millis() - lastRescanTime > RESCAN_INTERVAL_MS)) {
      lastRescanTime = millis();
    
      if (isDisplayOn) {
        M5.Display.setTextSize(2);
        M5.Display.setTextColor(WHITE, bgColour);
        M5.Display.fillRect(0, 45, M5.Display.width(), 30, bgColour);
        M5.Display.drawString("Scanning...", M5.Display.width() / 2, 60);
      }

      uint8_t foundCh = scanForMaster();

            if (foundCh > 0) {
        masterConnected = true;
        currentChannel = foundCh;
      
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
        esp_wifi_set_promiscuous(false);

        peerInfo.channel = currentChannel;
        if (esp_now_is_peer_exist(broadcastAddress)) {
          esp_now_del_peer(broadcastAddress);
        }
        esp_now_add_peer(&peerInfo);
      } else {
        // Fallback: Wieder auf früheren Kanal zurückschalten, falls der Scan erfolglos war
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
        esp_wifi_set_promiscuous(false);

        peerInfo.channel = currentChannel;
        if (esp_now_is_peer_exist(broadcastAddress)) {
          esp_now_del_peer(broadcastAddress);
        }
        esp_now_add_peer(&peerInfo);
      }

      if (isDisplayOn) {
        updateChannelDisplay();
      }
    }

    // --- ACQUIRE IMU DATA AND SEND TO MASTER (RUNS CONTINUOUSLY) ---
  float gx, gy, gz;
  float ax, ay, az;

  M5.Imu.getGyro(&gx, &gy, &gz);
  M5.Imu.getAccel(&ax, &ay, &az);

  sensorData.gyro_x    = gy;
  sensorData.accel_z   = az;
  sensorData.accel_y   = ay;
  sensorData.gyro_roll = gx;
  sensorData.accel_x   = ax;

    esp_now_send(broadcastAddress, (uint8_t *)&sensorData, sizeof(sensorData));

  // Maintain 200 Hz sampling (5 ms) using standard delay.
  // Note: Avoid esp_light_sleep_start() in the high-speed loop as light sleep disables 
  // the Wi-Fi/ESP-NOW RF hardware modem, causing dropped ACK packets and connection loss.
  delay(SAMPLE_RATE_MS);
}
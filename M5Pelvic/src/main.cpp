#include <M5Unified.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_sleep.h>

// --- CONFIGURATION ---
#define SENSOR_ID 4            // 4 = Pelvis
#define SAMPLE_RATE_MS 5       // 200 Hz sampling rate (5 ms)
#define DISPLAY_TIMEOUT_MS 30000 // 30 seconds display activity

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// --- DATA STRUCTURE TRANSMIT PACKET (IMU) ---
// Reuses struct_imu_data layout for master compatibility (foot_id field carries SENSOR_ID).
// Field repurposing for pelvis:
//   gyro_x    = gy  — sagittal pitch rate (anterior/posterior tilt)
//   accel_z   = az  — vertical acceleration
//   accel_y   = ay  — anterior-posterior acceleration
//   gyro_roll = gz  — yaw rate (transverse rotation / hip swing) [differs from foot usage]
//   accel_x   = ax  — lateral acceleration
typedef struct struct_imu_data {
  uint8_t foot_id;
  float gyro_x;    // Sagittal pitch rate (gy)
  float accel_z;   // Vertical acceleration
  float accel_y;   // Anterior-posterior acceleration
  float gyro_roll; // Yaw rate (gz — transverse rotation)
  float accel_x;   // Lateral acceleration
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
volatile uint32_t lastMasterAckTime = 0;
const uint32_t MASTER_TIMEOUT_MS = 3000;

volatile bool pendingUIUpdate = false;

uint8_t currentChannel = 1;
bool masterConnected = false;
unsigned long lastRescanTime = 0;
const unsigned long RESCAN_INTERVAL_MS = 60000;

unsigned long lastBatteryUpdate = 0;
unsigned long displayTurnOnTime = 0;
bool isDisplayOn = true;

void updateChannelDisplay() {
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE, PURPLE);
  M5.Display.fillRect(0, 45, M5.Display.width(), 30, PURPLE);

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
  M5.Display.setTextColor(WHITE, PURPLE);
  char battBuf[16];
  snprintf(battBuf, sizeof(battBuf), "BATT: %d%% ", level);
  M5.Display.drawString(battBuf, M5.Display.width() / 2, 95);
}

uint8_t scanForMaster() {
  sensorData.foot_id = SENSOR_ID;
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

    delay(15);

    for (int attempt = 0; attempt < 5; attempt++) {
      masterAckReceived = false;
      esp_now_send(broadcastAddress, (uint8_t *)&sensorData, sizeof(sensorData));

      uint32_t startWait = millis();
      while (millis() - startWait < 50) {
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

// --- ESP-NOW RECEIVE CALLBACK ---
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
      lastMasterAckTime = millis();

      if (!masterConnected) {
        masterConnected = true;
        currentChannel = (ack.master_channel > 0) ? ack.master_channel : currentChannel;
        pendingUIUpdate = true;
      }
    }
  }
}

void setup() {
  setCpuFrequencyMhz(80);

  auto cfg = M5.config();
  cfg.internal_imu = true;
  M5.begin(cfg);

  M5.Display.setRotation(1);
  M5.Display.setTextDatum(middle_center);
  M5.Display.setBrightness(64);

  M5.Display.fillScreen(PURPLE);
  M5.Display.setTextColor(WHITE, PURPLE);
  M5.Display.setTextSize(2);
  M5.Display.drawString("PELVIS", M5.Display.width() / 2, 25);

  updateBatteryDisplay();
  displayTurnOnTime = millis();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.setTxPower(WIFI_POWER_11dBm);

  if (esp_now_init() != ESP_OK) {
    M5.Display.setTextSize(1);
    M5.Display.drawString("ESP-NOW Error!", M5.Display.width() / 2, 55);
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  M5.Display.setTextSize(2);
  M5.Display.drawString("Search Master...", M5.Display.width() / 2, 55);

  uint8_t foundChannel = scanForMaster();

  M5.Display.fillScreen(PURPLE);
  M5.Display.setTextSize(2);
  M5.Display.drawString("PELVIS", M5.Display.width() / 2, 25);

  if (foundChannel > 0) {
    masterConnected = true;
    currentChannel = foundChannel;
    lastMasterAckTime = millis();
  } else {
    masterConnected = false;
    currentChannel = 1;
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

  if (pendingUIUpdate) {
    pendingUIUpdate = false;
    if (isDisplayOn) {
      updateChannelDisplay();
    }
  }

  if (M5.BtnA.wasPressed()) {
    isDisplayOn = true;
    displayTurnOnTime = millis();
    M5.Display.setBrightness(128);
    updateBatteryDisplay();
  }

  if (M5.BtnB.wasHold()) {
    M5.Display.setBrightness(128);
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setTextSize(2);
    M5.Display.drawString("OFF...", M5.Display.width() / 2, M5.Display.height() / 2);
    delay(500);
    M5.Power.powerOff();
  }

  if (isDisplayOn && (millis() - displayTurnOnTime > DISPLAY_TIMEOUT_MS)) {
    isDisplayOn = false;
    M5.Display.setBrightness(0);
  }

  if (isDisplayOn && (millis() - lastBatteryUpdate > 2000)) {
    lastBatteryUpdate = millis();
    updateBatteryDisplay();
  }

  if (masterConnected && (millis() - lastMasterAckTime > MASTER_TIMEOUT_MS)) {
    masterConnected = false;
    lastRescanTime = millis();
    if (isDisplayOn) {
      updateChannelDisplay();
    }
  }

  if (!masterConnected && (millis() - lastRescanTime > RESCAN_INTERVAL_MS)) {
    lastRescanTime = millis();

    if (isDisplayOn) {
      M5.Display.setTextSize(2);
      M5.Display.setTextColor(WHITE, PURPLE);
      M5.Display.fillRect(0, 45, M5.Display.width(), 30, PURPLE);
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

  // --- ACQUIRE IMU DATA AND SEND TO MASTER ---
  float gx, gy, gz;
  float ax, ay, az;

  M5.Imu.getGyro(&gx, &gy, &gz);
  M5.Imu.getAccel(&ax, &ay, &az);

  sensorData.foot_id   = SENSOR_ID;
  sensorData.gyro_x    = gy;   // Sagittal pitch rate
  sensorData.accel_z   = az;   // Vertical acceleration
  sensorData.accel_y   = ay;   // Anterior-posterior acceleration
  sensorData.gyro_roll = gz;   // Yaw rate (transverse rotation)
  sensorData.accel_x   = ax;   // Lateral acceleration

  esp_now_send(broadcastAddress, (uint8_t *)&sensorData, sizeof(sensorData));

  delay(SAMPLE_RATE_MS);
}

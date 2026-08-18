#include <M5Unified.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WebServer.h>

#if __has_include("secrets.h")
  #include "secrets.h"
#else
  #define STAMMI_SSID "DanceNet"
  #define STAMMI_PASS "12345678"
#endif

#include "index.h"
#include "solo.h"

// --- DATA STRUCTURE FOR FEET (ID 1 & 2) ---
typedef struct struct_imu_data {
  uint8_t foot_id;  // 1 = Left, 2 = Right
  float gyro_x;     // Pitch rotation (gy — roll-off)
  float accel_z;    // Z-acceleration (impact)
  float accel_y;    // Longitudinal acceleration (forward/backward)
  float gyro_roll;  // Lateral roll rotation (gx — pronation/supination)
  float accel_x;    // Lateral acceleration (for 3D impact magnitude)
} struct_imu_data;

// --- DATA STRUCTURE FOR HAND/SCALE (ID 3) ---
typedef struct struct_hand_data {
  uint8_t hand_id; // 3 = Hand / Scale
  float weight;    // Push/Pull in grams
  float accel_x;
  float accel_y;
  float accel_z;
} struct_hand_data;

// --- DATA STRUCTURE FOR HANDSHAKE / CHANNEL CONFIRMATION ---
typedef struct struct_handshake_ack {
  uint8_t master_channel;
  uint8_t confirmed;
} struct_handshake_ack;

struct_handshake_ack ackPacket;

// --- THRESHOLDS FOR ERROR ALARM (FEET) ---
const float GYRO_MIN  = 80.0f;   // Degrees/second (minimum roll-off)
const float ACCEL_MAX = 1.5f;    // g-force (maximum impact)

// Live measurement values
float leftGyro = 0, leftAccel = 0, leftAccelY = 0, leftGyroRoll = 0, leftAccelX = 0;
float rightGyro = 0, rightAccel = 0, rightAccelY = 0, rightGyroRoll = 0, rightAccelX = 0;
float pelvicGyro = 0, pelvicAccel = 0, pelvicAccelY = 0, pelvicYaw = 0, pelvicAccelX = 0;
float handWeight = 0, handAx = 0, handAy = 0, handAz = 0;

// --- TIMEOUT TRACKING FOR CONNECTED SENSORS ---
uint32_t lastSeenLeft   = 0;
uint32_t lastSeenRight  = 0;
uint32_t lastSeenHand   = 0;
uint32_t lastSeenPelvic = 0;
const uint32_t SENSOR_TIMEOUT_MS = 3500;

// Helper variables for Jerk calculation on M5
float prevHandWeight = 0;
float prevHandAx = 0, prevHandAy = 0, prevHandAz = 1.0;
volatile bool isJerkAlert = false;

// Warning status for visual feedback
enum ErrorState { NONE = 0, ERR_LEFT = 1, ERR_RIGHT = 2, ERR_BOTH = 3 };
volatile ErrorState currentError = NONE;
volatile uint32_t errorStartTime = 0;


// Web server on port 80
WebServer server(80);

// Web server Endpoint: Trigger hardware tare on scale sensor
void handleTare() {
    uint8_t tare_cmd = 0xA1;
    uint8_t bcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    esp_now_peer_info_t peerTemp;
    memset(&peerTemp, 0, sizeof(peerTemp));
    memcpy(peerTemp.peer_addr, bcast, 6);
    peerTemp.channel = WiFi.channel();
    peerTemp.encrypt = false;
    if (!esp_now_is_peer_exist(bcast)) {
        esp_now_add_peer(&peerTemp);
    }
    esp_now_send(bcast, &tare_cmd, 1);
    server.send(200, "text/plain", "OK");
}

// Web server Endpoint: Send JSON data
void handleData() {
  bool leftOk   = (millis() - lastSeenLeft   < SENSOR_TIMEOUT_MS);
  bool rightOk  = (millis() - lastSeenRight  < SENSOR_TIMEOUT_MS);
  bool handOk   = (millis() - lastSeenHand   < SENSOR_TIMEOUT_MS);
  bool pelvicOk = (millis() - lastSeenPelvic < SENSOR_TIMEOUT_MS);

    // Send zero/defaults if sensor is offline to prevent frozen "phantom" values on dashboard
    float sendLG   = leftOk  ? leftGyro      : 0.0f;
    float sendLA   = leftOk  ? leftAccel     : 0.0f;
    float sendLAy  = leftOk  ? leftAccelY    : 0.0f;
    float sendLGr  = leftOk  ? leftGyroRoll  : 0.0f;
    float sendLAx  = leftOk  ? leftAccelX    : 0.0f;
    float sendRG   = rightOk ? rightGyro     : 0.0f;
    float sendRA   = rightOk ? rightAccel    : 0.0f;
    float sendRAy  = rightOk ? rightAccelY   : 0.0f;
    float sendRGr  = rightOk ? rightGyroRoll : 0.0f;
    float sendRAx  = rightOk ? rightAccelX   : 0.0f;
    float sendPG   = pelvicOk ? pelvicGyro   : 0.0f;
    float sendPA   = pelvicOk ? pelvicAccel  : 0.0f;
    float sendPAy  = pelvicOk ? pelvicAccelY : 0.0f;
    float sendPYaw = pelvicOk ? pelvicYaw    : 0.0f;
    float sendPAx  = pelvicOk ? pelvicAccelX : 0.0f;
    float sendHW   = handOk  ? handWeight : 0.0f;
    float sendAx   = handOk  ? handAx     : 0.0f;
    float sendAy   = handOk  ? handAy     : 0.0f;
    float sendAz   = handOk  ? handAz     : 1.0f;

    char buf[550];
    snprintf(buf, sizeof(buf),
      "{\"lG\":%.1f,\"lA\":%.2f,\"lAy\":%.2f,\"lGr\":%.1f,\"lAx\":%.2f,\"rG\":%.1f,\"rA\":%.2f,\"rAy\":%.2f,\"rGr\":%.1f,\"rAx\":%.2f,\"pG\":%.1f,\"pA\":%.2f,\"pAy\":%.2f,\"pYaw\":%.1f,\"pAx\":%.2f,\"hW\":%.1f,\"hAx\":%.2f,\"hAy\":%.2f,\"hAz\":%.2f,\"lOk\":%s,\"rOk\":%s,\"pOk\":%s,\"hOk\":%s,\"err\":%d,\"jerk\":%s}",
      sendLG, sendLA, sendLAy, sendLGr, sendLAx, sendRG, sendRA, sendRAy, sendRGr, sendRAx,
      sendPG, sendPA, sendPAy, sendPYaw, sendPAx, sendHW, sendAx, sendAy, sendAz,
      leftOk ? "true" : "false", rightOk ? "true" : "false",
      pelvicOk ? "true" : "false", handOk ? "true" : "false",
      (int)currentError, isJerkAlert ? "true" : "false"
    );
    server.send(200, "application/json", buf);
}

// ESP-NOW Receive Callback with automatic acknowledgement
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
  const uint8_t *src_mac = recv_info->src_addr;
#else
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
  const uint8_t *src_mac = mac_addr;
#endif

      // --- DATA PROCESSING ---
  if (len == sizeof(struct_hand_data)) {
    struct_hand_data handData;
    memcpy(&handData, data, sizeof(handData));

    handWeight   = handData.weight;
    handAx       = handData.accel_x;
    handAy       = handData.accel_y;
    handAz       = handData.accel_z;
    lastSeenHand = millis();

    // --- CALCULATE LEAD SMOOTHNESS / JERK FOR THE HAND ---
    float dWeight = fabsf(handWeight - prevHandWeight);
    prevHandWeight = handWeight;

    float dAx = handAx - prevHandAx;
    float dAy = handAy - prevHandAy;
    float dAz = handAz - prevHandAz;

    prevHandAx = handAx;
    prevHandAy = handAy;
    prevHandAz = handAz;

    float accelJerk = sqrtf(dAx*dAx + dAy*dAy + dAz*dAz);
    float fuehrungshaerteRaw = (dWeight / 50.0f) + (accelJerk * 15.0f);

    if (fuehrungshaerteRaw > 12.0) {
      isJerkAlert = true;
    } else {
      isJerkAlert = false;
    }
  } else if (len >= 12) {
    struct_imu_data footData;
    memset(&footData, 0, sizeof(footData));
    memcpy(&footData, data, std::min((size_t)len, sizeof(footData)));

    bool isLeft = (footData.foot_id == 1);
    float gyroVal  = fabsf(footData.gyro_x);
    float accelVal = fabsf(footData.accel_z);

    if (isLeft) {
      leftGyro     = footData.gyro_x;
      leftAccel    = footData.accel_z;
      leftAccelY   = footData.accel_y;
      leftGyroRoll = footData.gyro_roll;
      leftAccelX   = footData.accel_x;
      lastSeenLeft = millis();
    } else if (footData.foot_id == 4) {
      pelvicGyro   = footData.gyro_x;
      pelvicAccel  = footData.accel_z;
      pelvicAccelY = footData.accel_y;
      pelvicYaw    = footData.gyro_roll;
      pelvicAccelX = footData.accel_x;
      lastSeenPelvic = millis();
    } else {
      rightGyro     = footData.gyro_x;
      rightAccel    = footData.accel_z;
      rightAccelY   = footData.accel_y;
      rightGyroRoll = footData.gyro_roll;
      rightAccelX   = footData.accel_x;
      lastSeenRight = millis();
    }

    if (footData.foot_id != 4 && accelVal > ACCEL_MAX && gyroVal < GYRO_MIN) {
      if (isLeft) {
        currentError = (currentError == ERR_RIGHT) ? ERR_BOTH : ERR_LEFT;
      } else {
        currentError = (currentError == ERR_LEFT) ? ERR_BOTH : ERR_RIGHT;
      }
      errorStartTime = millis();
    }
  }

  // --- HANDSHAKE / SEND ACKNOWLEDGEMENT BACK ---
  esp_now_peer_info_t peerTemp;
  memset(&peerTemp, 0, sizeof(peerTemp));
  memcpy(peerTemp.peer_addr, src_mac, 6);
  peerTemp.channel = WiFi.channel();
  peerTemp.encrypt = false;

  if (!esp_now_is_peer_exist(src_mac)) {
    esp_now_add_peer(&peerTemp);
  }

  ackPacket.master_channel = WiFi.channel();
  ackPacket.confirmed = 1;
  esp_now_send(src_mac, (uint8_t *)&ackPacket, sizeof(ackPacket));
}

void setup() {
  // Reduce CPU from 240 MHz default — 160 MHz is sufficient for WiFi AP+STA + ESP-NOW + HTTP
  setCpuFrequencyMhz(160);

  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Speaker.begin();
  M5.Speaker.setVolume(128);

  M5.Display.setRotation(1);
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setBrightness(64); // ~25% brightness, enough to read in a studio

  // --- WIFI SETUP (AP + STA) ---
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(STAMMI_SSID, STAMMI_PASS);
  
  M5.Display.setCursor(10, 10);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.println("WiFi Connecting...");

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(250);
    timeout++;
  }

  uint8_t currentChannel = 1;

    M5.Display.fillScreen(BLACK);
  if (WiFi.status() == WL_CONNECTED) {
    currentChannel = WiFi.channel();
    // Home WiFi active: leave TX power at default — router may be in another room.

    M5.Display.setCursor(10, 10);
    M5.Display.setTextColor(GREEN, BLACK);
    M5.Display.println("HOME-WIFI OK");

    M5.Display.setCursor(10, 35);
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.printf("Channel: %d", currentChannel);

    M5.Display.setCursor(10, 60);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.printf("IP: %s", WiFi.localIP().toString().c_str());
  } else {
    WiFi.disconnect();
    currentChannel = 1;
    // AP-only mode: sensors and phone are all within 2-3 m — 11 dBm is sufficient.
    WiFi.setTxPower(WIFI_POWER_11dBm);

    M5.Display.setCursor(10, 10);
    M5.Display.setTextColor(ORANGE, BLACK);
    M5.Display.println("AP MODE ONLY");

    M5.Display.setCursor(10, 35);
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.drawString("Channel: 1", 10, 35);

    M5.Display.setCursor(10, 60);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.drawString("IP: 192.168.4.1", 10, 60);
  }

  WiFi.softAP("M5-Dance-Master", "12345678", currentChannel);

  if (esp_now_init() != ESP_OK) {
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(RED, BLACK);
    M5.Display.drawString("ESP-NOW ERR", 10, 25);
    return;
  }

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  esp_now_register_recv_cb(OnDataRecv);

  delay(500);

    server.on("/", []() { server.send(200, "text/html", HTML_PAGE); });
    server.on("/solo", []() { server.send(200, "text/html", HTML_SOLO_PAGE); });
    server.on("/data", handleData);
    server.on("/tare", handleTare);
    server.begin();
  
  M5.Display.fillScreen(BLACK);
}

void loop() {
  M5.update();
  server.handleClient();

  if (M5.BtnB.wasHold()) {
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setTextSize(2);
    M5.Display.drawString("OFF...", M5.Display.width() / 2, M5.Display.height() / 2);
    delay(500);
    M5.Power.powerOff();
  }

  if (currentError != NONE && (millis() - errorStartTime > 200)) {
    currentError = NONE;
  }

  uint16_t leftBg  = (currentError == ERR_LEFT  || currentError == ERR_BOTH) ? (uint16_t)RED   : (uint16_t)BLACK;
  uint16_t rightBg = (currentError == ERR_RIGHT || currentError == ERR_BOTH) ? (uint16_t)RED   : (uint16_t)BLACK;
  uint16_t leftFg  = (currentError == ERR_LEFT  || currentError == ERR_BOTH) ? (uint16_t)WHITE : (uint16_t)BLUE;
  uint16_t rightFg = (currentError == ERR_RIGHT || currentError == ERR_BOTH) ? (uint16_t)WHITE : (uint16_t)RED;

  uint32_t now = millis();
  bool leftOnline   = (now - lastSeenLeft   < SENSOR_TIMEOUT_MS);
  bool rightOnline  = (now - lastSeenRight  < SENSOR_TIMEOUT_MS);
  bool handOnline   = (now - lastSeenHand   < SENSOR_TIMEOUT_MS);
  bool pelvicOnline = (now - lastSeenPelvic < SENSOR_TIMEOUT_MS);

  M5.Display.setTextSize(2);

  // Sentinels — only track online/error state (no live values needed on display).
  static bool     dLOn = true, dROn = true, dHOn = true, dPOn = true;
  static uint16_t dLBg = 0xFFFF, dRBg = 0xFFFF;
  static uint32_t lastBatUpdate = 0;

  // --- DISPLAY LEFT FOOT (only on status change) ---
  if (leftOnline != dLOn || leftBg != dLBg) {
    dLOn = leftOnline; dLBg = leftBg;
    M5.Display.setCursor(5, 5);
    M5.Display.setTextColor(leftFg, leftBg);
    M5.Display.printf("L: %-14s", leftOnline ? "ONLINE" : "OFFLINE");
  }

  // --- DISPLAY RIGHT FOOT (only on status change) ---
  if (rightOnline != dROn || rightBg != dRBg) {
    dROn = rightOnline; dRBg = rightBg;
    M5.Display.setCursor(5, 30);
    M5.Display.setTextColor(rightFg, rightBg);
    M5.Display.printf("R: %-14s", rightOnline ? "ONLINE" : "OFFLINE");
  }

  // --- DISPLAY HAND (only on status change) ---
  if (handOnline != dHOn) {
    dHOn = handOnline;
    M5.Display.setCursor(5, 55);
    M5.Display.setTextColor(GREEN, BLACK);
    M5.Display.printf("H: %-14s", handOnline ? "ONLINE" : "OFFLINE");
  }

  // --- DISPLAY PELVIS (only on status change) ---
  if (pelvicOnline != dPOn) {
    dPOn = pelvicOnline;
    M5.Display.setCursor(5, 80);
    M5.Display.setTextColor(PURPLE, BLACK);
    M5.Display.printf("P: %-14s", pelvicOnline ? "ONLINE" : "OFFLINE");
  }

  // --- BATTERY (every 10 s — level changes on the order of minutes) ---
  if (millis() - lastBatUpdate > 10000) {
    lastBatUpdate = millis();
    int batLevel = M5.Power.getBatteryLevel();
    M5.Display.setCursor(5, 105);
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.printf("BAT: %3d%%        ", batLevel);
  }

  delay(20);
}
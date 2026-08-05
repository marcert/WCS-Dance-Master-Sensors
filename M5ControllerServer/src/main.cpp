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

// --- DATA STRUCTURE FOR FEET (ID 1 & 2) ---
typedef struct struct_imu_data {
  uint8_t foot_id; // 1 = Left, 2 = Right
  float gyro_x;    // Y-rotation from transmitter (roll-off)
  float accel_z;   // Z-acceleration (impact)
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
float GYRO_MIN  = 80.0;   // Degrees/second (minimum roll-off)
float ACCEL_MAX = 1.5;    // g-force (maximum impact)

// Live measurement values
float leftGyro = 0, leftAccel = 0;
float rightGyro = 0, rightAccel = 0;
float handWeight = 0, handAx = 0, handAy = 0, handAz = 0;

// --- TIMEOUT TRACKING FOR CONNECTED SENSORS ---
uint32_t lastSeenLeft  = 0;
uint32_t lastSeenRight = 0;
uint32_t lastSeenHand  = 0;
const uint32_t SENSOR_TIMEOUT_MS = 3500; // Erhöht auf 3.5s, um kurze Tarierpausen der Waage zu überbrücken

// Helper variables for Jerk calculation on M5
float prevHandWeight = 0;
float prevHandAx = 0, prevHandAy = 0, prevHandAz = 1.0;
bool isJerkAlert = false;

// Warning status for visual feedback
enum ErrorState { NONE = 0, ERR_LEFT = 1, ERR_RIGHT = 2, ERR_BOTH = 3 };
volatile ErrorState currentError = NONE;
volatile uint32_t errorStartTime = 0;

// Pending Audio Tone Trigger (entschärft Interrupt / Callback Context)
volatile uint16_t pendingToneFreq = 0;
volatile uint32_t pendingToneDuration = 0;

// Web server on port 80
WebServer server(80);

// Web server Endpoint: Send JSON data
void handleData() {
  bool leftOk  = (millis() - lastSeenLeft  < SENSOR_TIMEOUT_MS);
  bool rightOk = (millis() - lastSeenRight < SENSOR_TIMEOUT_MS);
  bool handOk  = (millis() - lastSeenHand  < SENSOR_TIMEOUT_MS);

    // Send zero/defaults if sensor is offline to prevent frozen "phantom" values on dashboard
  float sendLG  = leftOk  ? leftGyro   : 0.0f;
  float sendLA  = leftOk  ? leftAccel  : 0.0f;
  float sendRG  = rightOk ? rightGyro  : 0.0f;
  float sendRA  = rightOk ? rightAccel : 0.0f;
  float sendHW  = handOk  ? handWeight : 0.0f;
  float sendAx  = handOk  ? handAx     : 0.0f;
  float sendAy  = handOk  ? handAy     : 0.0f;
  float sendAz  = handOk  ? handAz     : 1.0f;

  char buf[256];
  snprintf(buf, sizeof(buf),
    "{\"lG\":%.1f,\"lA\":%.2f,\"rG\":%.1f,\"rA\":%.2f,\"hW\":%.1f,\"hAx\":%.2f,\"hAy\":%.2f,\"hAz\":%.2f,\"lOk\":%s,\"rOk\":%s,\"hOk\":%s,\"err\":%d,\"jerk\":%s}",
    sendLG, sendLA, sendRG, sendRA, sendHW, sendAx, sendAy, sendAz,
    leftOk ? "true" : "false", rightOk ? "true" : "false", handOk ? "true" : "false",
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
  if (len == sizeof(struct_imu_data)) {
    struct_imu_data footData;
    memcpy(&footData, data, sizeof(footData));

    bool isLeft = (footData.foot_id == 1);
    float gyroVal  = abs(footData.gyro_x);
    float accelVal = abs(footData.accel_z);

    if (isLeft) {
      leftGyro     = footData.gyro_x;
      leftAccel    = footData.accel_z;
      lastSeenLeft = millis();
    } else {
      rightGyro     = footData.gyro_x;
      rightAccel    = footData.accel_z;
      lastSeenRight = millis();
    }

        // WCS ERROR CONDITION (Stomping without roll-off)
    if (accelVal > ACCEL_MAX && gyroVal < GYRO_MIN) {
      currentError = isLeft ? ERR_LEFT : ERR_RIGHT;
      errorStartTime = millis();
      pendingToneFreq = isLeft ? 1800 : 2500;
      pendingToneDuration = 100;
    }
  } 
  else if (len == sizeof(struct_hand_data)) {
    struct_hand_data handData;
    memcpy(&handData, data, sizeof(handData));

    handWeight   = handData.weight;
    handAx       = handData.accel_x;
    handAy       = handData.accel_y;
    handAz       = handData.accel_z;
    lastSeenHand = millis();

    // --- CALCULATE LEAD SMOOTHNESS / JERK FOR THE HAND ---
    float dWeight = abs(handWeight - prevHandWeight);
    prevHandWeight = handWeight;

    float dAx = handAx - prevHandAx;
    float dAy = handAy - prevHandAy;
    float dAz = handAz - prevHandAz;

    prevHandAx = handAx;
    prevHandAy = handAy;
    prevHandAz = handAz;

    float accelJerk = sqrt(dAx*dAx + dAy*dAy + dAz*dAz);
    float fuehrungshaerteRaw = (dWeight / 50.0) + (accelJerk * 15.0);

    // Tone on Jerk (> 12.0)
    if (fuehrungshaerteRaw > 12.0) {
      isJerkAlert = true;
      if (currentError == NONE && pendingToneFreq == 0) {
        pendingToneFreq = 1000;
        pendingToneDuration = 100;
      }
    } else {
      isJerkAlert = false;
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
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Speaker.begin();
  M5.Speaker.setVolume(255);

  M5.Display.setRotation(1);
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);

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

  delay(3000);

  server.on("/", []() { server.send(200, "text/html", HTML_PAGE); });
  server.on("/data", handleData);
  server.begin();
  
  M5.Display.fillScreen(BLACK);
}

void loop() {
  M5.update();
  server.handleClient();

  if (pendingToneFreq > 0) {
    M5.Speaker.tone(pendingToneFreq, pendingToneDuration);
    pendingToneFreq = 0;
  }

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

  uint16_t leftBg  = (currentError == ERR_LEFT)  ? RED : BLACK;
  uint16_t rightBg = (currentError == ERR_RIGHT) ? RED : BLACK;

  uint16_t leftFg  = (currentError == ERR_LEFT)  ? WHITE : BLUE;
  uint16_t rightFg = (currentError == ERR_RIGHT) ? WHITE : RED;

  uint32_t now = millis();
  bool leftOnline  = (now - lastSeenLeft  < SENSOR_TIMEOUT_MS);
  bool rightOnline = (now - lastSeenRight < SENSOR_TIMEOUT_MS);
  bool handOnline  = (now - lastSeenHand  < SENSOR_TIMEOUT_MS);

  M5.Display.setTextSize(2);

  // --- DISPLAY LEFT FOOT ---
  M5.Display.setCursor(5, 5);
  M5.Display.setTextColor(leftFg, leftBg);
  if (leftOnline) {
    M5.Display.printf("L: G:%4.0f A:%3.1f  ", leftGyro, leftAccel);
  } else {
    M5.Display.printf("L: -- OFFLINE -- ");
  }

  // --- DISPLAY RIGHT FOOT ---
  M5.Display.setCursor(5, 30);
  M5.Display.setTextColor(rightFg, rightBg);
  if (rightOnline) {
    M5.Display.printf("R: G:%4.0f A:%3.1f  ", rightGyro, rightAccel);
  } else {
    M5.Display.printf("R: -- OFFLINE -- ");
  }

  // --- DISPLAY HAND / SCALE ---
  M5.Display.setCursor(5, 55);
  M5.Display.setTextColor(GREEN, BLACK);
  if (handOnline) {
    M5.Display.printf("H: %4.0f g        ", handWeight);
  } else {
    M5.Display.printf("H: -- OFFLINE -- ");
  }

    // --- BATTERY DISPLAY ---
  int batLevel = M5.Power.getBatteryLevel();
  M5.Display.setCursor(5, 80);
  M5.Display.setTextColor(YELLOW, BLACK);
  M5.Display.printf("BAT: %3d%%        ", batLevel);

  delay(20);
}
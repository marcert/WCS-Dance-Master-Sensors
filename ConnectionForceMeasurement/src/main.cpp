#include <M5GFX.h>
#include <M5Unified.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "HX711.h" // Bibliothek für die Kraftmessdose (Unit Weight)

// --- PINS FÜR DEINE KRAFTMESSDOSE (UNIT WEIGHT) ---
#define HX711_DOUT_PIN 33
#define HX711_SCK_PIN  32

// Neuer korrigierter Faktor:
#define SCALE_FACTOR 129.1f

// --- ESP-NOW DATENSTRUKTUR SENDE-PAKET (HAND / WAAGE) ---
typedef struct struct_hand_data {
  uint8_t hand_id; // Immer 3 für Hand
  float weight;    // Zug/Druck in Gramm/kg
  float accel_x;   // Beschleunigung X
  float accel_y;   // Beschleunigung Y
  float accel_z;   // Beschleunigung Z
} struct_hand_data;

// --- DATENSTRUKTUR EMPFANGS-PAKET (HANDSHAKE ACK VOM MASTER) ---
typedef struct struct_handshake_ack {
  uint8_t master_channel;
  uint8_t confirmed;
} struct_handshake_ack;

struct_hand_data sendData;
esp_now_peer_info_t peerInfo;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Status-Variablen für Handshake
volatile bool masterAckReceived = false;
volatile uint8_t masterConfirmedChannel = 0;
volatile uint32_t lastMasterAckTime = 0;
const uint32_t MASTER_TIMEOUT_MS = 3000;

volatile bool pendingUIUpdate = false;
volatile bool pendingTare = false;

uint8_t currentChannel = 1;
bool masterConnected = false;
uint32_t lastRescanTime = 0;
const uint32_t RESCAN_INTERVAL_MS = 60000; // Alle 60 Sekunden Re-Scan bei fehlendem Master

// HX711 Objekt
HX711 scale;

// Funktion zum Zeichnen der kleinen Verbindungs-LED (2x2 px Dot oben in der Mitte)
void drawConnectionStatus() {
  uint16_t dotColor = masterConnected ? GREEN : RED;
  M5.Display.fillRect(150, 5, 8, 8, dotColor);
}

uint8_t scanForMaster() {
  sendData.hand_id = 3;
  sendData.weight  = 0.0f;
  sendData.accel_x = 0.0f;
  sendData.accel_y = 0.0f;
  sendData.accel_z = 1.0f;

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
      esp_now_send(broadcastAddress, (uint8_t *)&sendData, sizeof(sendData));

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

// Globale Variablen für das Display-Diagramm
int graphX = 0;
int lastY = 80;

// Variablen zur Erkennung von Wertänderungen
float lastSentWeight = -9999.0f;
float lastSentAx = 0.0f, lastSentAy = 0.0f, lastSentAz = 0.0f;
uint32_t lastSendTime = 0;

// Timer für Akku-Anzeige
uint32_t lastBatCheck = 0;

// --- ESP-NOW EMPFANGS-CALLBACK FÜR HANDSHAKE ACK VOM MASTER ---
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
  } else if (len == 1 && data[0] == 0xA1) {
    pendingTare = true;
  }
}

void setup() {
  // 1. CPU-Frequenz senken (Spart massiv Akku, 80MHz reicht völlig aus)
  setCpuFrequencyMhz(80);

  auto cfg = M5.config();
  cfg.internal_imu = true; // IMU aktivieren
  M5.begin(cfg);

    // --- DISPLAY SETUP ---
  M5.Display.setRotation(3);
  M5.Display.setBrightness(64); // Display gedimmt auf ~25% (Spart Akku, bleibt gut lesbar)
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.drawString("INIT WEIGHT...", 10, 5);

  // --- KRAFTMESSDOSE (HX711) SETUP ---
  scale.begin(HX711_DOUT_PIN, HX711_SCK_PIN);
  scale.set_scale(SCALE_FACTOR);
  delay(1000);
  scale.tare(); // Automatischen Nullpunkt beim Start setzen

  // --- ESP-NOW & WIFI SETUP ---
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // 2. Sendeleistung auf 11 dBm reduzieren (3-5m Reichweite, spart Strom)
  WiFi.setTxPower(WIFI_POWER_11dBm);

  if (esp_now_init() != ESP_OK) {
    M5.Display.drawString("ESP-NOW ERR", 10, 5);
    return;
  }

  // Receive-Callback registrieren für den Master-Scan
  esp_now_register_recv_cb(OnDataRecv);

      // --- AUTOMATISCHER KANAL-SUCHLAUF (KANAL 1 BIS 13) ---
  M5.Display.fillScreen(BLACK);
  M5.Display.drawString("Search Master...", 10, 5);

  uint8_t foundChannel = scanForMaster();

  // --- KANAL-ERGEBNIS FIXIEREN ---
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE, BLACK);

    if (foundChannel > 0) {
    masterConnected = true;
    currentChannel = foundChannel;
    lastMasterAckTime = millis();
    M5.Display.printf("Ch %d OK!", foundChannel);
  } else {
    masterConnected = false;
    currentChannel = 1;
    M5.Display.drawString("No Master", 10, 5);
  }

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  peerInfo.channel = currentChannel;
  if (esp_now_is_peer_exist(broadcastAddress)) {
    esp_now_del_peer(broadcastAddress);
  }
  esp_now_add_peer(&peerInfo);

    delay(1000);
  M5.Display.fillScreen(BLACK);
  drawConnectionStatus();
  lastRescanTime = millis();
}

void drawGraph(float weight) {
  // Mapping von -2500g (-2,5kg) bis +2500g (+2,5kg) auf Y-Pixel (130 bis 30)
  int newY = map((long)weight, -2500, 2500, 130, 30);
  newY = constrain(newY, 30, 130);

  // Zug (Grün) / Druck (Rot)
  uint16_t color = (weight > 0) ? GREEN : RED;

  M5.Display.drawLine(graphX, lastY, graphX + 1, newY, color);
  
  graphX++;
  lastY = newY;

  // Wenn der Bildrand erreicht ist: Bereich löschen und neu anfangen
  if (graphX > 235) {
    M5.Display.fillRect(0, 30, 240, 105, BLACK);
    graphX = 0;
  }
}

void loop() {
  M5.update();

  // --- ENT KOPPELTES UI UPDATE NACH VERBINDUNGSAUFBAU ---
  if (pendingUIUpdate) {
    pendingUIUpdate = false;
    drawConnectionStatus();
  }

  if (pendingTare) {
    pendingTare = false;
    lastMasterAckTime = millis();
    M5.Display.fillRect(0, 0, 240, 25, BLACK);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.drawString("TARE...", 10, 5);
    if (scale.is_ready()) scale.read();
    if (scale.is_ready()) scale.read();
    long zero_offset = scale.read_average(5);
    scale.set_offset(zero_offset);
    lastSentWeight = -9999.0f;
    lastMasterAckTime = millis();
    M5.Display.fillRect(0, 0, 240, 25, BLACK);
    M5.Display.drawString("TARE OK!", 10, 5);
    delay(500);
    M5.Display.fillRect(0, 0, 240, 25, BLACK);
  }

  // --- BUTTON B: AUSSCHALTEN PER LONG-PRESS ---
  if (M5.BtnB.wasHold()) {
    M5.Display.fillScreen(BLACK);
    M5.Display.drawString("OFF...", 10, 5);
    delay(500);
    M5.Power.powerOff();
  }

    // --- BUTTON A: TARIEREN / NULLPUNKT SETZEN ---
    if (M5.BtnA.wasClicked()) {
      M5.Display.fillRect(0, 0, 240, 25, BLACK);
      M5.Display.drawString("RELEASE...", 10, 5);

      lastMasterAckTime = millis(); // reset before blocking — gives full 3s margin during tare

      delay(1500);

      M5.Display.fillRect(0, 0, 240, 25, BLACK);
      M5.Display.drawString("TARE...", 10, 5);

      if (scale.is_ready()) scale.read();
      if (scale.is_ready()) scale.read();

      long zero_offset = scale.read_average(5);
      scale.set_offset(zero_offset);

      lastSentWeight = -9999.0f; 
      lastMasterAckTime = millis(); // Timeout während des Tarierens verhindern!

      M5.Display.fillRect(0, 0, 240, 25, BLACK);
      M5.Display.drawString("TARE OK!", 10, 5);
      delay(500);
      M5.Display.fillRect(0, 0, 240, 25, BLACK);
    }

    // --- AKKUSTAND ANZEIGEN (alle 5 Sekunden) ---
  if (millis() - lastBatCheck > 5000 || lastBatCheck == 0) {
    lastBatCheck = millis();
    int batLevel = M5.Power.getBatteryLevel();

    uint16_t batColor = GREEN;
    if (batLevel < 20)      batColor = RED;
    else if (batLevel < 50) batColor = YELLOW;

    M5.Display.setTextColor(batColor, BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(170, 5);
    M5.Display.printf("%3d%%", batLevel);

    drawConnectionStatus(); // Verbindungs-Punkt mit aktualisieren
  }

  // --- PRÜFEN, OB VERBINDUNG ZUM MASTER ABGEBROCHEN IST ---
  if (masterConnected && (millis() - lastMasterAckTime > MASTER_TIMEOUT_MS)) {
    masterConnected = false;
    lastRescanTime = millis(); // Rescan-Timer zurücksetzen
    drawConnectionStatus();    // Punkt auf ROT schalten
  }

  // --- AUTOMATISCHER RE-SCAN ALLE 60s WENN KEIN MASTER GEFUNDEN ---
  if (!masterConnected && (millis() - lastRescanTime > RESCAN_INTERVAL_MS)) {
    lastRescanTime = millis();
    
    // Kleiner blauer Indikator während des Scans
    M5.Display.fillRect(150, 5, 8, 8, BLUE);

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

    drawConnectionStatus();
  }

  // --- PRÜFEN, OB EIN NEUER HARDWARE-MESSWERT BEREITSTEHT ---
  if (scale.is_ready()) {
    float rawWeight = scale.get_units(1);
    float currentWeight = round(rawWeight); // Auf ganze Gramm runden

    float ax, ay, az;
    M5.Imu.getAccel(&ax, &ay, &az);

    bool weightChanged = (abs(currentWeight - lastSentWeight) >= 1.0f);
    bool accelChanged  = (abs(ax - lastSentAx) > 0.05f || abs(ay - lastSentAy) > 0.05f || abs(az - lastSentAz) > 0.05f);
    bool heartbeat     = (millis() - lastSendTime > 2000);

    if (weightChanged || accelChanged || heartbeat) {
      sendData.hand_id = 3; // always explicit — do not rely on scanForMaster() having run
      sendData.weight  = currentWeight;
      sendData.accel_x = ax;
      sendData.accel_y = ay;
      sendData.accel_z = az;

      esp_now_send(broadcastAddress, (uint8_t *)&sendData, sizeof(sendData));

      lastSentWeight = currentWeight;
      lastSentAx     = ax;
      lastSentAy     = ay;
      lastSentAz     = az;
      lastSendTime   = millis();

      if (currentWeight < 2500 && currentWeight > -2500) {
        drawGraph(currentWeight);
      }

      M5.Display.setCursor(10, 5);
      M5.Display.setTextColor(WHITE, BLACK);
      M5.Display.printf("G:%4.0fg", currentWeight); // Leicht gekürzt, damit Platz für Akku bleibt
    }
  }

  delay(12); // HX711 default 80 SPS = 12.5 ms/sample; polling faster wastes CPU with no gain
}
#include <M5Unified.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WebServer.h>
#include "secrets.h"

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

// Helper variables for Jerk calculation on M5
float prevHandWeight = 0;
float prevHandAx = 0, prevHandAy = 0, prevHandAz = 1.0;
bool isJerkAlert = false;

// Warning status for visual feedback
enum ErrorState { NONE = 0, ERR_LEFT = 1, ERR_RIGHT = 2, ERR_BOTH = 3 };
volatile ErrorState currentError = NONE;
volatile uint32_t errorStartTime = 0;

// Web server on port 80
WebServer server(80);

// --- HTML DASHBOARD WITH SYNCHRONOUS ERROR LINES ---
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>WCS Ultra-Performance Dashboard</title>
    <style>
        body { 
            background: transparent; 
            color: #fff; 
            font-family: 'Arial Black', Gadget, sans-serif; 
            margin: 0; 
            padding: 10px;
            overflow: hidden; 
            box-sizing: border-box;
            height: 100vh;
            display: flex;
            flex-direction: column;
            justify-content: space-between;
            text-shadow: 2px 2px 4px #000; 
        }
        
        #camera-feed {
            position: fixed;
            top: 0;
            left: 0;
            width: 100vw;
            height: 100vh;
            object-fit: cover;
            z-index: -1;
        }

        #header-container {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 0 10px;
            height: 12vh;
        }
        
        #wert { 
            font-size: 8vh; 
            font-weight: 900; 
            line-height: 0.8; 
            margin: 0; 
            padding-bottom: 5px; 
        }
        
        .btn-group {
            display: flex;
            gap: 8px;
            margin: 0; 
            flex-wrap: wrap; 
            justify-content: flex-end;
        }

        .action-btn {
            background-color: rgba(34, 34, 34, 0.6); 
            color: #fff;
            border: 2px solid rgba(255, 255, 255, 0.3);
            padding: 6px 12px;
            font-size: 3.5vw;
            font-family: 'Arial Black', sans-serif;
            font-weight: bold;
            border-radius: 8px;
            cursor: pointer;
            backdrop-filter: blur(4px);
            -webkit-backdrop-filter: blur(4px); 
        }
        
        #cam-btn { background-color: rgba(0, 122, 255, 0.6); }
        #wide-btn { background-color: rgba(0, 200, 120, 0.6); display: none; }
        #full-btn { background-color: rgba(80, 80, 80, 0.6); }

        #debug-info {
            position: fixed;
            bottom: 5px;
            left: 5px;
            right: 5px;
            font-size: 11px;
            font-family: monospace;
            background: rgba(0,0,0,0.8);
            padding: 6px 8px;
            border-radius: 6px;
            max-height: 30vh;
            overflow-y: auto;
            white-space: pre-wrap;
            z-index: 10;
            display: none;
        }

        #freeze-btn.frozen {
            background-color: rgba(255, 59, 48, 0.8);
            animation: pulse 1.5s infinite;
        }
        
        @keyframes pulse {
            0% { box-shadow: 0 0 0 0 rgba(255, 59, 48, 0.6); }
            70% { box-shadow: 0 0 0 12px rgba(255, 59, 48, 0); }
            100% { box-shadow: 0 0 0 0 rgba(255, 59, 48, 0); }
        }

        .graph-container { width: 100%; height: 36vh; display: flex; flex-direction: column; }
        .label { font-size: 13px; color: #ddd; margin-bottom: 2px; text-transform: uppercase; letter-spacing: 1px; }
        
        canvas { 
            background-color: rgba(0, 0, 0, 0.4); 
            border: 2px solid rgba(255, 255, 255, 0.2); 
            width: 100%; 
            height: 100%; 
            display: block; 
            border-radius: 8px; 
        }
        
        .legende { display: flex; gap: 15px; font-size: 11px; margin-top: 4px; color: #ddd; }
        .legende-item { display: flex; align-items: center; gap: 4px; }
        .box-left { width: 12px; height: 12px; background-color: #00ffff; border-radius: 2px; }
        .box-right { width: 12px; height: 12px; background-color: #ff00ff; border-radius: 2px; }
        .box-jerk { width: 12px; height: 12px; background-color: #ffff00; border-radius: 2px; }
        .box-impact { width: 12px; height: 12px; background-color: #ff0000; border-radius: 2px; }
    </style>
</head>
<body>

    <video id="camera-feed" autoplay playsinline></video>

    <div id="header-container">
        <div id="wert">0 g</div>
        <div class="btn-group">
            <button id="cam-btn" class="action-btn" onclick="startCamera()">START CAM</button>
            <button id="full-btn" class="action-btn" onclick="toggleFullscreen()">FULL</button>
            <button id="wide-btn" class="action-btn" onclick="tryUltraWide()">WIDE</button>
            <button id="freeze-btn" class="action-btn" onclick="toggleFreeze()">FREEZE</button>
        </div>
    </div>

    <div id="debug-info"></div>

    <div class="graph-container">
        <div class="label">Connection Force (-3.5 kg to +3.5 kg)</div>
        <canvas id="graph_kraft" width="1000" height="250"></canvas>
    </div>

    <div class="graph-container">
        <div class="label">Combined Analysis (Roll-off Quality, Jerk & Synchronous Error Lines)</div>
        <canvas id="graph_kombi" width="1000" height="250"></canvas>
        <div class="legende">
            <div class="legende-item"><div class="box-left"></div> Sound/Error Left</div>
            <div class="legende-item"><div class="box-right"></div> Sound/Error Right</div>
            <div class="legende-item"><div class="box-jerk"></div> Hand Jerk (Sound 1000 Hz)</div>
        </div>
    </div>

<script>
        // --- FULLSCREEN LOGIC ---
        const fullBtn = document.getElementById('full-btn');
        function toggleFullscreen() {
            if (!document.fullscreenElement && !document.webkitFullscreenElement) {
                if (document.documentElement.requestFullscreen) document.documentElement.requestFullscreen();
                else if (document.documentElement.webkitRequestFullscreen) document.documentElement.webkitRequestFullscreen();
                fullBtn.innerText = "EXIT";
            } else {
                if (document.exitFullscreen) document.exitFullscreen();
                else if (document.webkitExitFullscreen) document.webkitExitFullscreen();
                fullBtn.innerText = "FULL";
            }
        }

        // --- CAMERA LOGIC ---
        const videoElement = document.getElementById('camera-feed');
        const camBtn = document.getElementById('cam-btn');
        const wideBtn = document.getElementById('wide-btn');
        const debugInfo = document.getElementById('debug-info');

        document.getElementById('wert').addEventListener('click', () => {
            debugInfo.style.display = (debugInfo.style.display === 'none' || !debugInfo.style.display) ? 'block' : 'none';
        });

        function updateDebugInfo(stream) {
            const track = stream.getVideoTracks()[0];
            const settings = track.getSettings ? track.getSettings() : {};
            const caps = track.getCapabilities ? track.getCapabilities() : {};

            debugInfo.innerText =
                `Active Lens: ${track.label || 'n/a'}\n` +
                `facingMode: ${settings.facingMode || caps.facingMode || 'n/a'}`;

            if (caps.zoom && caps.zoom.min < 1) wideBtn.style.display = 'block';
            else wideBtn.style.display = 'none';
        }

        async function startCamera() {
            if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
                try {
                    const stream = await navigator.mediaDevices.getUserMedia({ video: { facingMode: "environment" } });
                    videoElement.srcObject = stream;
                    camBtn.style.display = 'none'; 
                    updateDebugInfo(stream);
                } catch (error) { alert("Camera access failed: " + error.message); }
            }
        }

        async function tryUltraWide() {
            if (!videoElement.srcObject) return;
            const track = videoElement.srcObject.getVideoTracks()[0];
            const caps = track.getCapabilities ? track.getCapabilities() : {};

            if (caps.zoom && caps.zoom.min < 1) {
                try {
                    await track.applyConstraints({ advanced: [{ zoom: caps.zoom.min }] });
                    updateDebugInfo(videoElement.srcObject);
                } catch (error) { alert("Wide angle failed: " + error.message); }
            }
        }

        // --- FETCH & GRAPH LOGIC ---
        const canvasKraft = document.getElementById('graph_kraft');
        const ctxKraft = canvasKraft.getContext('2d');
        const canvasKombi = document.getElementById('graph_kombi');
        const ctxKombi = canvasKombi.getContext('2d');
        const wertAnzeige = document.getElementById('wert');
        const freezeBtn = document.getElementById('freeze-btn');
        
        const intervalMs = 20;
        const totalDurationSec = 10;
        const maxPoints = (1000 / intervalMs) * totalDurationSec; // 500 points
        const xStep = canvasKraft.width / maxPoints;               // 2 pixels per step

        let kraftPoints     = new Array(maxPoints).fill(125);
        
        let leftFootPoints  = new Array(maxPoints).fill().map(() => ({ y: 125, isError: false }));
        let rightFootPoints = new Array(maxPoints).fill().map(() => ({ y: 125, isError: false }));
        let jerkPoints      = new Array(maxPoints).fill().map(() => ({ y: 245, isJerkPeak: false }));

        let targetW = 0, targetLG = 0, targetLA = 1, targetRG = 0, targetRA = 1;
        let targetAx = 0, targetAy = 0, targetAz = 1;
        let serverError = 0;
        let serverJerk = false;

        let currentW = 0, currentLG = 0, currentLA = 1, currentRG = 0, currentRA = 1;
        let currentAx = 0.0, currentAy = 0.0, currentAz = 1.0;
        
        let prevAx = 0, prevAy = 0, prevAz = 1;
        let prevWeight = 0;

        let isFrozen = false;

        function toggleFreeze() {
            isFrozen = !isFrozen;
            if (isFrozen) {
                freezeBtn.innerText = "RUNNING";
                freezeBtn.classList.add('frozen');
            } else {
                freezeBtn.innerText = "FREEZE";
                freezeBtn.classList.remove('frozen');
            }
        }

        function fetchSensorData() {
            if (!isFrozen) {
                fetch('/data')
                    .then(response => response.json())
                    .then(data => {
                        targetW  = data.hW;
                        targetLG = data.lG;
                        targetLA = data.lA;
                        targetRG = data.rG;
                        targetRA = data.rA;
                        targetAx = data.hAx;
                        targetAy = data.hAy;
                        targetAz = data.hAz;
                        serverError = data.err;
                        serverJerk  = data.jerk; 
                    })
                    .catch(err => console.log(err));
            }
        }

        setInterval(fetchSensorData, intervalMs);

        setInterval(function() {
            if (isFrozen) return;

            // Smoothing
            currentW  += (targetW - currentW) * 0.4;
            currentLG += (targetLG - currentLG) * 0.4;
            currentLA += (targetLA - currentLA) * 0.4;
            currentRG += (targetRG - currentRG) * 0.4;
            currentRA += (targetRA - currentRA) * 0.4;

            currentAx += (targetAx - currentAx) * 0.4;
            currentAy += (targetAy - currentAy) * 0.4;
            currentAz += (targetAz - currentAz) * 0.4;

            wertAnzeige.innerText = Math.round(targetW) + " g";
            wertAnzeige.style.color = targetW >= 0 ? "#00ff00" : "#ff0000";

            // 1. Force (Scaling adjusted to -3.5 kg to +3.5 kg)
            let y_kraft = 125 - (currentW / 3500) * 125;
            y_kraft = Math.max(0, Math.min(250, y_kraft));
            kraftPoints.shift(); kraftPoints.push(y_kraft);

            // 2. Combined Foot Metric (Roll-off weighted with Impact)
            let leftImpactDev = Math.abs(Math.abs(currentLA) - 1.0);
            let rightImpactDev = Math.abs(Math.abs(currentRA) - 1.0);

            let leftQuality  = Math.abs(currentLG) / (1.0 + leftImpactDev * 2.0);
            let rightQuality = Math.abs(currentRG) / (1.0 + rightImpactDev * 2.0);

            let y_lg = 125 - (leftQuality / 300) * 125;
            let y_rg = 125 - (rightQuality / 300) * 125;

            y_lg = Math.max(0, Math.min(250, y_lg));
            y_rg = Math.max(0, Math.min(250, y_rg));

            let isLeftErr  = (serverError === 1) || (Math.abs(currentLA) > 1.5 && Math.abs(currentLG) < 80.0);
            let isRightErr = (serverError === 2) || (Math.abs(currentRA) > 1.5 && Math.abs(currentRG) < 80.0);

            leftFootPoints.shift();  leftFootPoints.push({ y: y_lg, isError: isLeftErr });
            rightFootPoints.shift(); rightFootPoints.push({ y: y_rg, isError: isRightErr });

            // 3. Lead Smoothness / Jerk
            let dWeight = Math.abs(currentW - prevWeight);
            prevWeight = currentW;

            let dAx = currentAx - prevAx;
            let dAy = currentAy - prevAy;
            let dAz = currentAz - prevAz;
            
            prevAx = currentAx;
            prevAy = currentAy;
            prevAz = currentAz;

            let accelJerk = Math.sqrt(dAx*dAx + dAy*dAy + dAz*dAz);
            let fuehrungshaerteRaw = (dWeight / 50.0) + (accelJerk * 15.0);

            let y_jerk = 245 - (fuehrungshaerteRaw * 8.0); 
            y_jerk = Math.max(5, Math.min(245, y_jerk));
            
            let isJerkPeak = serverJerk || (fuehrungshaerteRaw > 12.0); 
            jerkPoints.shift(); 
            jerkPoints.push({ y: y_jerk, isJerkPeak: isJerkPeak });

        }, intervalMs); 

        function draw() {
            if (!isFrozen) {
                drawKraftGraph();
                drawKombiGraph();
            }
            requestAnimationFrame(draw);
        }
        requestAnimationFrame(draw);

        function drawKraftGraph() {
            ctxKraft.clearRect(0, 0, canvasKraft.width, canvasKraft.height);
            ctxKraft.strokeStyle = 'rgba(255, 255, 255, 0.2)'; 
            ctxKraft.lineWidth = 2;
            ctxKraft.beginPath(); ctxKraft.moveTo(0, 125); ctxKraft.lineTo(canvasKraft.width, 125); ctxKraft.stroke();

            ctxKraft.lineWidth = 6;
            let isGreen = (kraftPoints[0] < 125);
            ctxKraft.beginPath();
            ctxKraft.strokeStyle = isGreen ? '#00ff00' : '#ff0000';
            ctxKraft.moveTo(0, kraftPoints[0]);

            for(let i = 1; i < kraftPoints.length; i++) {
                let x = i * xStep;
                let nextIsGreen = (kraftPoints[i] < 125);
                ctxKraft.lineTo(x, kraftPoints[i]);
                
                if (nextIsGreen !== isGreen) {
                    ctxKraft.stroke();
                    ctxKraft.beginPath();
                    ctxKraft.strokeStyle = nextIsGreen ? '#00ff00' : '#ff0000';
                    ctxKraft.moveTo(x, kraftPoints[i]);
                    isGreen = nextIsGreen;
                }
            }
            ctxKraft.stroke();
        }

        function drawKombiGraph() {
            ctxKombi.clearRect(0, 0, canvasKombi.width, canvasKombi.height);
            
            // Zero line
            ctxKombi.strokeStyle = 'rgba(255, 255, 255, 0.2)';
            ctxKombi.lineWidth = 2;
            ctxKombi.beginPath(); ctxKombi.moveTo(0, 125); ctxKombi.lineTo(canvasKombi.width, 125); ctxKombi.stroke();

            // STEP 1: VERTICAL LINES ON SOUND/ERROR EVENTS (BACKGROUND)
            for (let i = 0; i < maxPoints; i++) {
                let x = i * xStep;
                let leftErr  = leftFootPoints[i].isError;
                let rightErr = rightFootPoints[i].isError;
                let jerkPeak = jerkPoints[i].isJerkPeak;

                if (leftErr || rightErr || jerkPeak) {
                    ctxKombi.beginPath();
                    ctxKombi.setLineDash([]); 

                    if (leftErr && rightErr) {
                        ctxKombi.strokeStyle = '#ff0000'; 
                        ctxKombi.lineWidth = 3;
                    } else if (leftErr) {
                        ctxKombi.strokeStyle = 'rgba(0, 255, 255, 0.9)'; 
                        ctxKombi.lineWidth = 2;
                    } else if (rightErr) {
                        ctxKombi.strokeStyle = 'rgba(255, 0, 255, 0.9)'; 
                        ctxKombi.lineWidth = 2;
                    } else if (jerkPeak) {
                        ctxKombi.strokeStyle = 'rgba(255, 255, 0, 0.6)'; 
                        ctxKombi.setLineDash([4, 4]);
                        ctxKombi.lineWidth = 2;
                    }

                    ctxKombi.moveTo(x, 0);
                    ctxKombi.lineTo(x, canvasKombi.height);
                    ctxKombi.stroke();
                    ctxKombi.setLineDash([]); 
                }
            }

            // STEP 2: DRAW ANALYSIS CURVES ON TOP
            ctxKombi.lineWidth = 4;
            ctxKombi.strokeStyle = '#00ffff';
            ctxKombi.beginPath();
            ctxKombi.moveTo(0, leftFootPoints[0].y);
            for(let i = 1; i < leftFootPoints.length; i++) { ctxKombi.lineTo(i * xStep, leftFootPoints[i].y); }
            ctxKombi.stroke();

            ctxKombi.lineWidth = 4;
            ctxKombi.strokeStyle = '#ff00ff';
            ctxKombi.beginPath();
            ctxKombi.moveTo(0, rightFootPoints[0].y);
            for(let i = 1; i < rightFootPoints.length; i++) { ctxKombi.lineTo(i * xStep, rightFootPoints[i].y); }
            ctxKombi.stroke();

            ctxKombi.lineWidth = 3;
            ctxKombi.strokeStyle = '#ffff00';
            ctxKombi.beginPath();
            ctxKombi.moveTo(0, jerkPoints[0].y);
            for(let i = 1; i < jerkPoints.length; i++) { ctxKombi.lineTo(i * xStep, jerkPoints[i].y); }
            ctxKombi.stroke();
        }
    </script>
</body>
</html>
)rawliteral";

// Web server Endpoint: Send JSON data
void handleData() {
  String json = "{";
  json += "\"lG\":" + String(leftGyro) + ",";
  json += "\"lA\":" + String(leftAccel) + ",";
  json += "\"rG\":" + String(rightGyro) + ",";
  json += "\"rA\":" + String(rightAccel) + ",";
  json += "\"hW\":" + String(handWeight) + ",";
  json += "\"hAx\":" + String(handAx) + ",";
  json += "\"hAy\":" + String(handAy) + ",";
  json += "\"hAz\":" + String(handAz) + ",";
  json += "\"err\":" + String((int)currentError) + ",";
  json += "\"jerk\":" + String(isJerkAlert ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

// ESP-NOW Receive Callback with automatic acknowledgement (response to transmitter)
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
      leftGyro  = footData.gyro_x;
      leftAccel = footData.accel_z;
    } else {
      rightGyro  = footData.gyro_x;
      rightAccel = footData.accel_z;
    }

    // WCS ERROR CONDITION (Stomping without roll-off)
    if (accelVal > ACCEL_MAX && gyroVal < GYRO_MIN) {
      currentError = isLeft ? ERR_LEFT : ERR_RIGHT;
      errorStartTime = millis();
      uint16_t freq = isLeft ? 1800 : 2500;
      M5.Speaker.tone(freq, 100); 
    }
  } 
  else if (len == sizeof(struct_hand_data)) {
    struct_hand_data handData;
    memcpy(&handData, data, sizeof(handData));

    handWeight = handData.weight;
    handAx     = handData.accel_x;
    handAy     = handData.accel_y;
    handAz     = handData.accel_z;

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

    // Third tone on Jerk (> 12.0) – Lower tone at 1000 Hz
    if (fuehrungshaerteRaw > 12.0) {
      isJerkAlert = true;
      if (currentError == NONE) {
        M5.Speaker.tone(1000, 100); 
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
    
    M5.Display.setTextColor(GREEN, BLACK);
    M5.Display.drawString("HOME-WIFI OK", 10, 10);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.drawString("IP:", 10, 35);
    M5.Display.drawString(WiFi.localIP().toString(), 10, 60);
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.printf("Channel: %d", currentChannel);
  } else {
    WiFi.disconnect();
    currentChannel = 1;
    
    M5.Display.setTextColor(ORANGE, BLACK);
    M5.Display.drawString("AP MODE ONLY", 10, 10);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.drawString("IP: 192.168.4.1", 10, 35);
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.drawString("Channel: 1", 10, 60);
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

  int batLevel = M5.Power.getBatteryLevel();

  M5.Display.setTextSize(2);

  M5.Display.setCursor(5, 5);
  M5.Display.setTextColor(leftFg, leftBg);
  M5.Display.printf("L: G:%4.0f A:%3.1f ", leftGyro, leftAccel);

  M5.Display.setCursor(5, 30);
  M5.Display.setTextColor(rightFg, rightBg);
  M5.Display.printf("R: G:%4.0f A:%3.1f ", rightGyro, rightAccel);

  M5.Display.setCursor(5, 55);
  M5.Display.setTextColor(GREEN, BLACK);
  M5.Display.printf("H: %4.0f g        ", handWeight);

  M5.Display.setCursor(5, 80);
  M5.Display.setTextColor(YELLOW, BLACK);
  M5.Display.printf("BAT: %3d%%       ", batLevel);

  delay(20);
}
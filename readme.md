# 🕺 WCS Ultra-Performance Dance & Movement Analysis System

A real-time wireless sensor network and video feedback system built specifically for **West Coast Swing (WCS)** dancers and coaches.

---

## 🎯 Purpose & Goal

Improving connection, timing, and footwork in West Coast Swing often relies on subjective feeling or delayed video review. This system tries to solve that problem by giving **instant physical telemetry and visual feedback** while you dance.

### What it measures in real-time:
* **Footwork & Roll-off Quality (Left & Right Foot):** Measures foot roll-off dynamics via gyroscope angular rate and detects hard impacts / stomping (Z-axis g-force spikes).
* **Connection & Lead/Follow Smoothness (Hand Unit):** Measures pull and push tension in grams via a strain-gauge load cell (-3.5 kg to +3.5 kg) and detects abrupt, harsh movements (jerk index).
* **Live Acoustic & Visual Feedback:** Triggers immediate audio alert tones (e.g., 1000 Hz beep on connection jerk or poor roll-off with heavy impact) and displays synchronized overlay lines directly on a live video feed.

If you want to build that by your own check the required parts [here](partslist.md).
---

## 🚀 Quick Start / How to Use

1. **Power On:** Turn on the **Master Unit** first so it establishes its Wi-Fi channel (either the WiFi around- has to be maintained in secrets.h or spans its own WiFi).
2. **Connect Sensors:** Power on the Foot and Hand sensors. They automatically scan channels (1–13), pair with the Master, and lock onto the channel.
3. **Open Dashboard:** Connect your tablet, phone, or laptop to the Master's Wi-Fi network and open the IP address in any web browser.
4. **Train & Review:**
   * Tap **START CAM** to overlay the telemetry graphs directly over your live camera feed.
   * Use **FREEZE** during or after a pattern to pause graph rendering and analyze connection/footwork spikes frame-by-frame.
   * Press **Button A** on the Hand Unit at any time to tare (zero-out) the weight scale.

---

## 🛠️ Getting Started (Setup & Installation)

### Hardware Requirements
* **1x Master Unit:** M5Stack main unit (M5StickC PLUS / PLUS2 / Core).
* **2x Foot Sensors:** M5Stack units with internal 6-axis IMU.
* **1x Hand Sensor:** M5Stack unit + **HX711** load cell amplifier + Strain-Gauge Load Cell (connected to GPIO 33 DOUT / GPIO 32 SCK).

### Software Requirements
Install the following libraries in Arduino IDE / PlatformIO:
* `M5Unified`
* `WiFi.h` & `esp_wifi.h`
* `esp_now.h`
* `WebServer.h`
* `HX711.h`

### Firmware Flashing
1. **Master Unit:**
   * Create a `secrets.h` file containing your Wi-Fi credentials (`STAMMI_SSID` & `STAMMI_PASS`).
   * Flash the Master code. (If Wi-Fi fails, it falls back to standalone Access Point mode at `192.168.4.1`).
2. **Foot Sensors:**
   * Flash Unit 1 with `#define FOOT_ID 1` (Left Foot).
   * Flash Unit 2 with `#define FOOT_ID 2` (Right Foot).
3. **Hand Sensor:**
   * Flash with `#define HAND_ID 3` (Scale factor default is set to `129.1f`).

---

## 🏗️ System Architecture

```text
                       +-------------------------+
                       |   Foot Sensor (Left)    |
                       | ID: 1 | M5Stack + IMU   |
                       +------------+------------+
                                    |
                                    | (ESP-NOW Broadcast / Channel Auto-Sync)
                                    v
+------------------------+     +----+--------------------+     +-------------------------+
|   Hand / Scale Unit    |---->|   Central Master Unit   |<----|   Foot Sensor (Right)   |
| ID: 3 | HX711 + IMU    |     |    M5Stack Master       |     | ID: 2 | M5Stack + IMU   |
+------------------------+     +----------+--------------+     +-------------------------+
                                          |
                                          | (Wi-Fi AP / STA + WebServer)
                                          v
                              +-----------+--------------+
                              |   Web Dashboard (HTML5)  |
                              | Real-time Canvas Graphs  |
                              | Live Camera Overlay      |
                              +--------------------------+

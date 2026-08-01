```markdown
# WCS Ultra-Performance Dance & Movement Analysis System

A real-time wireless sensor network and dashboard system built for analyzing body dynamics, foot roll-off quality, foot strike impacts, and hand lead smoothness (jerk/tension) in dance styles such as **West Coast Swing (WCS)**.

The system uses **M5Stack hardware (ESP32)** communicating over an automated, low-latency **ESP-NOW** radio protocol to stream telemetry to a central master unit. The master unit hosts a lightweight web-based heads-up dashboard with live graph visualization and integrated web-camera streaming.

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

```

---

## 🌟 Key Features

* **Automatic ESP-NOW Channel Discovery:** Sensors dynamically scan Wi-Fi channels (1–13) at startup to locate and pair with the Master unit using a two-way handshake payload.
* **Foot Dynamics Tracking (IDs 1 & 2):**
* Measures foot roll-off (Gyroscope Y-rotation).
* Measures foot strike impact intensity (Accelerometer Z-axis).
* Real-time audio-visual error alerts for poor roll-off combined with heavy impact (stomping).


* **Hand & Lead Tension Tracking (ID 3):**
* Strain-gauge load cell integration via **HX711** measuring pull/push force (-2.5 kg to +2.5 kg).
* Calculates lead smoothness / jerk index by combining force variation and 3D acceleration changes.
* Instant acoustic feedback (1000 Hz alert tone) on harsh connection pulls or abrupt movements.


* **Power-Optimized Firmware:**
* CPU clock frequency reduced to **80 MHz** on transmitter nodes for extended battery life.
* Transmit power tuned to **11 dBm** (ideal range for 3–5 meters).
* Automatic screen dimming (30-second display timeout) on sensors to conserve energy.


* **Interactive HTML5 Dashboard:**
* Synchronous dual-canvas real-time charting (Connection Force & Combined Foot/Jerk Analysis).
* Background camera overlay using WebRTC/getUserMedia (with rear-facing lens selector and wide-angle support).
* Synchronized vertical event marker lines for immediate visual correlation of errors and jerk spikes.
* Freeze-frame control to pause visualization for deep-dive analysis.



---

## 🛠️ Hardware Requirements

* **Master Unit:**
* M5Stack Main Unit (e.g., M5StickC PLUS, M5StickC PLUS2, or M5Stack Core series).


* **Foot Sensors (2 Units):**
* 2x M5Stack devices with internal IMU (6-axis motion sensor).


* **Hand Sensor Unit (1 Unit):**
* 1x M5Stack device with internal IMU.
* 1x Load Cell (Strain Gauge) + **HX711** amplifier module connected to GPIO Pins `33` (DOUT) and `32` (SCK).



---

## 📦 Software Dependencies & Libraries

Ensure the following libraries are installed in your Arduino IDE or PlatformIO environment:

* [M5Unified](https://github.com/m5stack/M5Unified) – Universal hardware driver for M5Stack devices.
* `WiFi.h` & `esp_wifi.h` – Built-in ESP32 Wi-Fi stack.
* `esp_now.h` – ESP-NOW wireless protocol.
* `WebServer.h` – Embedded web server.
* `HX711.h` – Load cell driver library.

---

## ⚙️ Configuration & Setup

### 1. Master Unit Setup

1. Create a `secrets.h` file in the master sketch directory containing your Wi-Fi credentials:
```cpp
#ifndef SECRETS_H
#define SECRETS_H

const char* STAMMI_SSID = "Your_WiFi_SSID";
const char* STAMMI_PASS = "Your_WiFi_Password";

#endif

```


2. Upload the **Master Code** to the central M5Stack unit.
3. Upon booting:
* It attempts to connect to the designated Wi-Fi network.
* If connected, it serves the dashboard over the local network IP.
* If Wi-Fi fails, it falls back to Access Point mode (`SSID: M5-Dance-Master`, `IP: 192.168.4.1`).



### 2. Foot Sensors Setup

1. Open the **Foot Sensor Code**.
2. Set `#define FOOT_ID 1` for the **Left Foot** unit.
3. Flash the unit.
4. Set `#define FOOT_ID 2` for the **Right Foot** unit.
5. Flash the unit.

### 3. Hand / Scale Unit Setup

1. Open the **Hand Sensor Code**.
2. Verify load cell wiring:
* `HX711 DOUT` -> Pin `33`
* `HX711 SCK` -> Pin `32`


3. Calibrate `SCALE_FACTOR` if necessary (default: `129.1f`).
4. Flash the unit.

---

## 🚀 Usage

1. Power on the **Master Unit** first so it establishes its Wi-Fi channel.
2. Power on the **Foot Units** and **Hand Unit**. The devices will scan channels 1–13, complete a handshake with the master, and lock onto the master's operating channel.
3. Open a browser on a phone, tablet, or PC connected to the same network and navigate to the Master's IP address.
4. **Dashboard Controls:**
* **START CAM:** Enables real-time camera preview as background overlay.
* **FULL:** Toggles fullscreen display mode.
* **WIDE:** Switches to ultra-wide camera lens if supported by the device.
* **FREEZE:** Pauses graph rendering to analyze recent movements.
* **Button A (Hand Unit):** Recalibrates (tares) the zero point for force measurements.
* **Button A (Foot Units):** Wakes the local display for 30 seconds.
* **Button B (Long Press, All Units):** Power off node.



---

## 📄 Data Protocols

### ESP-NOW Packets

#### Foot IMU Packet (`struct_imu_data` - 9 Bytes)

| Field | Type | Description |
| --- | --- | --- |
| `foot_id` | `uint8_t` | `1` = Left Foot, `2` = Right Foot |
| `gyro_x` | `float` | Y-axis angular rate (Roll-off metric) |
| `accel_z` | `float` | Vertical impact force in g-force |

#### Hand Scale Packet (`struct_hand_data` - 17 Bytes)

| Field | Type | Description |
| --- | --- | --- |
| `hand_id` | `uint8_t` | `3` = Hand/Scale unit |
| `weight` | `float` | Force reading in grams (+ = Push, - = Pull) |
| `accel_x` / `y` / `z` | `float` | 3D motion tracking values |

#### Handshake Ack Packet (`struct_handshake_ack` - 2 Bytes)

| Field | Type | Description |
| --- | --- | --- |
| `master_channel` | `uint8_t` | Operating Wi-Fi channel of the Master |
| `confirmed` | `uint8_t` | Handshake confirmation flag (`1` = Valid) |

---

## 📜 License

Distributed under the MIT License. See `LICENSE` for more information.

```

```
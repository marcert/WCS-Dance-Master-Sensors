
# 🛠️ Bill of Materials & Hardware Setup

This document outlines all required hardware components, 3D/mechanical parts, and wiring instructions for building the **WCS Ultra-Performance Dance & Movement Analysis System**.

---

## 📦 Required Components List

| Component | Qty | Purpose / Role | Product Link / Specs |
| :--- | :---: | :--- | :--- |
| **M5Stick S3** | 1 | **Central Master Unit** (Web Server, Dashboard Host & ESP-NOW Receiver) | [M5Stack Shop](https://shop.m5stack.com/products/m5stamp-s3-pin-header) |
| **M5Stick S3** | 2 | **Foot IMU Sensors** (Left Foot ID 1, Right Foot ID 2) | [M5Stack Shop](https://shop.m5stack.com/products/m5stamp-s3-pin-header) |
| **M5StickC PLUS2** | 1 | **Hand / Scale Sensor Node** (Reads HX711 & local graph display) | [M5Stack Shop](https://shop.m5stack.com/products/m5stickc-plus2-esp32-pico-mini-iot-development-kit) |
| **M5Stack Weight Sensor Unit (HX711)** | 1 | ADC Amplifier for reading strain gauge load cell differential signals | [M5Stack Weight Sensor Unit](https://shop.m5stack.com/products/weight-sensor-unit?variant=16804759404634) |
| **S-Type Load Cell (Tension / Compression)** | 1 | Measures push (+ force) and pull (- force) lead connection up to 10 kg | [S-Type M8 Load Cell](https://www.amazon.de/dp/B0G4C7QRQK) |
| **T-Bar Handles (M8 Thread)** | 2 | Ergonomic connection handles for Leader & Follower hands during tension tests | [M8 T-Handles (Amazon)](https://www.amazon.de/dp/B0C5JB4N8M) |
| **M8 Threaded Rod / Adapters** | 2 | Connects the T-handles to the top and bottom eyelets of the S-Type Load Cell | Hardware store / Standard M8 |
| **M5Stack Mounts for shoes** | 2 | Securely fastens the Foot IMU units to the top of the dance shoes ![shoemount](/Attachments/Sensor-002.jpg) | [3D print shoe mount](/Attachments/M5-shoe-mount.stl) |
| **Housing for the scale** | 1 | housing for the load cell and mounting for the M5 ![Scale](/Attachments/Sensor-001.jpg) | [3D print scale](/Attachments/Scale-box.stl) |

---

## 🔌 Wiring & Pinout Guide

### Hand / Scale Unit Connection (HX711 to M5StickC PLUS2)

The **M5Stack Weight Sensor Unit** plugs directly into the Grove port of the M5StickC PLUS2 assigned to the hand unit (Node ID 3).

```text
  +----------------------+            +----------------------+
  |  S-Type Load Cell    |            |   M5Stack HX711      |
  |  (4-Wire Strain)     |            |   Weight Unit        |
  +----------------------+            +----------------------+
  | Red   (Excitation +) | ---------> | E+                   |
  | Black (Excitation -) | ---------> | E-                   |
  | White (Signal -)     | ---------> | S-                   |
  | Green (Signal +)     | ---------> | S+                   |
  +----------------------+            +----------+-----------+
                                                 |
                                         GROVE / GPIO Cable
                                                 |
                                                 v
                                      +----------------------+
                                      |   M5StickC PLUS2     |
                                      |   (Hand Unit ID: 3)  |
                                      +----------------------+
                                      | GPIO 33  <-- DOUT    |
                                      | GPIO 32  <-- SCK     |
                                      | 5V       <-- VCC     |
                                      | GND      <-- GND     |
                                      +----------------------+

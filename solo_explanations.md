# WCS Solo-Training Dashboard: Biomechanical & Technical Documentation

This document provides an in-depth breakdown of the biomechanical metrics, sensor fusion algorithms, threshold configurations, state machine lockout logic, and acoustic biofeedback mechanisms implemented in the **WCS Solo-Training Dashboard** (`/solo`).

<p align="center">
<img src="https://raw.githubusercontent.com/marcert/WCS-Dance-Master-Sensors/refs/heads/main/Attachments/Solo-Dashboard.jpg" width="300">
</p>
 ---

## 1. System Architecture & High-Speed Acquisition

The Solo Training System operates as a high-frequency biomechanical feedback loop designed for West Coast Swing footwork analysis:

```text
 +--------------------------+           +--------------------------+
 |  Left Foot Sensor (ID 1) |           | Right Foot Sensor (ID 2) |
 |  M5Stick S3 @ 200 Hz     |           | M5Stick S3 @ 200 Hz      |
 |  Standard Mounting       |           | 180° Inverted Mounting   |
 +------------+-------------+           +------------+-------------+
              |                                      |
              +-----------------+  +-----------------+
                                |  |  ESP-NOW (5 ms Interval)
                                v  v
                 +--------------------------------+
                 | Central Master Unit (Gateway)  |
                 | M5Stick S3 Web Server          |
                 +---------------+----------------+
                                 |
                                 | Web HTTP / JSON Stream (`/data`) @ 50 Hz
                                 v
                 +--------------------------------+
                 | Web Browser Dashboard (`/solo`)|
                 | Transparent WebRTC Overlay     |
                 | Web Audio API Biofeedback      |
                 +--------------------------------+
```

* **Foot Nodes (IDs 1 & 2):** M5Stick S3 units equipped with 6-axis IMUs (BMI270 / MPU6886). Firmware operates at **200 Hz (5 ms sampling interval)** to capture ultra-fast transient impact peaks during heel-strikes and toe-landings.
* **Central Master Unit:** Aggregates ESP-NOW streams and delivers JSON data packets (`lG`, `lA`, `lAy`, `rG`, `rA`, `rAy`) to the browser via the `/data` endpoint.
* **Web Dashboard (`/solo`):** Client-side JavaScript executes state machine filtering, direction mapping, pitch integration, stance timeline calculations, and Web Audio API feedback.

---

## 2. Mathematical & Biomechanical Definitions

### A. Foot Strike Angle ($\theta$) & Landing Articulation
Proper West Coast Swing foot mechanics require a heel-strike when moving forward and a toe/ball-strike when moving backward or anchoring.

```text
Forward Step (Heel-Strike):           Backward Step (Toe-Landing):
      /  (Positive Pitch Angle θ)            \  (Negative/Flat Pitch Angle θ)
     /                                        \
____/_____ (Floor)                      _______/___ (Floor)
   Heel                                   Toe / Ball
```

1. **Pitch Angle Estimation via Complementary Filter ($\theta_{\text{raw}}$):**
   Combines short-term gyro integration with long-term accelerometer angle correction to prevent gyro drift:
   
   $$\theta_{\text{raw}}(t) = 0.98 \times \bigl(\theta_{\text{raw}}(t - \Delta t) + \omega_{\text{pitch}} \times \Delta t\bigr) + 0.02 \times \arctan2(a_Y, a_Z) \times \frac{180}{\pi}$$
   
   The 2 % accelerometer contribution corrects up to ~1°/s drift per frame without degrading dynamic response. The right sensor is mounted 180° inverted, so its accelerometer term uses $\arctan2(a_Y, -a_Z)$.

2. **Zero-Tare Compensation ($\theta_{\text{calibrated}}$):**
   To adjust for individual instep shoe slopes, the `📐 ZERO` button captures static mounting offsets ($\text{leftMountOffset}$, $\text{rightMountOffset}$):
   
   $$\theta = \theta_{\text{raw}} - \text{mountOffset}$$

3. **Strike Angle Evaluation Rules:**
   * **Forward Step:**
     * $10^\circ \le \theta \le 25^\circ \longrightarrow$ `OPTIMAL HEEL` (Clean heel articulation)
     * $5^\circ \le \theta < 10^\circ \longrightarrow$ `FLAT` (Borderline flat landing)
     * $\theta < 5^\circ \longrightarrow$ `FLAT-FOOT!` (Harsh flat-foot placement; triggers 1200 Hz audio warning)
   * **Backward Step:**
     * $-20^\circ \le \theta \le 5^\circ \longrightarrow$ `OPTIMAL TOE` (Clean toe-ball roll-off)
     * $5^\circ < \theta \le 10^\circ \longrightarrow$ `FLAT` (Borderline flat landing)
     * $\theta > 10^\circ \longrightarrow$ `HEEL LANDING!` (Biomechanical error: heel landing while moving backward; triggers 1200 Hz audio warning)

---

### B. Impact Jerk ($J_{\text{impact}}$) & Shock Absorption
Impact Jerk quantifies the rate of change of vertical impact acceleration ($aZ$ in $g$) upon step landing. It measures how effectively the knee and ankle joints cushion foot placement:

$$J_{\text{impact}} = \left| \frac{aZ_{\text{current}} - aZ_{\text{previous}}}{\Delta t} \right| \quad [\text{g/s}]$$

* **Soft Cushioning ($< 40\text{ g/s}$):** Good joint absorption.
* **Moderate Shock ($40\text{ to }80\text{ g/s}$):** Acceptable impact level.
* **Harsh Stomping ($> 80\text{ g/s}$):** Excessive shock transmitted to joints; triggers a 500 Hz low-frequency impact click.

> Thresholds use the native 200 Hz sensor time step ($\Delta t = 0.005\text{ s}$) as the denominator — this yields values 4× higher than a poll-rate-based calculation would, which is why the numbers are larger than comparable literature figures using longer time windows.

---

### C. Double Stance Overlap ($\Delta t_{\text{double-stance}}$) & Grounding Ratio
West Coast Swing emphasizes a continuous, grounded "rolling" weight transfer rather than abrupt hopping or lifting off the floor prematurely. 

$$\text{Stance Ratio} = \left( \frac{\Delta t_{\text{double-stance}}}{t_{\text{step}}} \right) \times 100\%$$

#### Why Overlap Matter in WCS Mechanics:
* **Grounded Rolling Action:** In West Coast Swing, weight transfer is gradual. As one foot leaves the floor, the other receives weight, creating a natural bilateral overlap phase where both soles touch the ground ($|aZ| > 0.85g$).
* **Elastic Extension & Timing:** A healthy overlap ratio ($18\%\text{ to }38\%$) creates the characteristic "elastic" stretch and smooth momentum transfer in WCS. Too little overlap indicates rushing or bouncing, while too much overlap results in heavy, sluggish transitions.

| Ratio Range (%) | Badge Rating | Biomechanical Meaning |
| :---: | :---: | :--- |
| **18% to 38%** | `OPTIMAL ROLL` | Ideal grounded roll-off phase for walks and extensions. |
| **< 18%** | `HECTIC` | Rushed weight transfer; lack of rolling articulation through the foot. |
| **> 38%** | `SLUGGISH` | Over-invested ground contact; sluggish tempo transition. |

---

### D. Roll-off Symmetry Index (ASI) & Smoothness Index

1. **Asymmetry Index (ASI):**
   Uses the standard biomechanical ASI formula — symmetric regardless of which foot is dominant:
   
   $$\text{ASI} = \frac{2 \left| \int |\omega_{\text{left}}| \, dt - \int |\omega_{\text{right}}| \, dt \right|}{\int |\omega_{\text{left}}| \, dt + \int |\omega_{\text{right}}| \, dt} \times 100\%$$
   
   | Badge | Range | Biomechanical Meaning |
   | :---: | :---: | :--- |
   | `SYMMETRIC` (Green) | $\le 10\%$ | Equal roll-off articulation on both legs. |
   | `MINOR ASYM` (Yellow) | $\le 25\%$ | Mild bilateral difference; monitor for compensation patterns. |
   | `ASYMMETRIC` (Red) | $> 25\%$ | Significant side-dominance; risk of overuse injury. |

2. **Roll-Smoothness Index (0 = rough, 100 = smooth):**
   Measures bilateral angular jerk $(d\omega / dt)$ from both feet and inverts the scale so higher values mean smoother movement, averaged over a 25-frame (0.5 s) sliding window:
   
   $$\text{Jerkiness} = \text{clamp}_{0\text{–}100}\!\left[\left(\left|\frac{\Delta \omega_{\text{L}}}{\Delta t}\right| + \left|\frac{\Delta \omega_{\text{R}}}{\Delta t}\right|\right) \times 0.075\right]$$
   $$\text{Smoothness} = 100 - \text{Mean}_{25}(\text{Jerkiness})$$
   
   | Badge | Range | Biomechanical Meaning |
   | :---: | :---: | :--- |
   | `SMOOTH` (Green) | $\ge 65$ | Fluid, continuous ankle articulation without micro-stutters. |
   | `MODERATE` (Yellow) | $\ge 40$ | Occasional abrupt changes; acceptable for learning phases. |
   | `ROUGH` (Red) | $< 40$ | Frequent micro-stutters; indicates tension or lack of ankle mobility. |

---

## 3. Configured Thresholds & Audio Biofeedback Summary

| Metric / Parameter | Value / Range | Visual Badge / State | Audio Biofeedback |
| :--- | :--- | :--- | :--- |
| **Forward Heel Angle** | $10^\circ \text{ to } 25^\circ$ | `OPTIMAL HEEL` (Green) | None |
| **Forward Flat Foot** | $< 5^\circ$ | `FLAT-FOOT!` (Red) | 1200 Hz Sine Click (80 ms) |
| **Backward Toe Angle** | $-20^\circ \text{ to } +5^\circ$ | `OPTIMAL TOE` (Green) | None |
| **Backward Heel Error** | $> 10^\circ$ | `HEEL LANDING!` (Red) | 1200 Hz Warning Beep (80 ms) |
| **Impact Jerk ($J_{\text{impact}}$)** | $> 80\text{ g/s}$ | Flash Card Boundary | 500 Hz Low Impact Click (80 ms) |
| **Double Stance Ratio** | 18% to 38% | `OPTIMAL ROLL` (Green) | None |
| **Double Stance Hectic** | $< 18\%$ | `HECTIC` (Yellow) | None |
| **Double Stance Sluggish**| $> 38\%$ | `SLUGGISH` (Yellow) | None |
| **Per-Foot Lockout Window**| $800\text{ ms}$ | Suppresses same-foot re-trigger | None |
| **Symmetry Index (ASI)** | $\le 10\%$ | `SYMMETRIC` (Green) | None |
| **Symmetry Index (ASI)** | $\le 25\%$ | `MINOR ASYM` (Yellow) | None |
| **Symmetry Index (ASI)** | $> 25\%$ | `ASYMMETRIC` (Red) | None |
| **Roll-Smoothness** | $\ge 65$ | `SMOOTH` (Green) | None |
| **Roll-Smoothness** | $\ge 40$ | `MODERATE` (Yellow) | None |
| **Roll-Smoothness** | $< 40$ | `ROUGH` (Red) | None |

---

## 4. Signal Filtering & Lockout Concept

To prevent false secondary step triggers caused by micro-taps, foot unweighting, or floor vibrations, the DSP pipeline executes a **Dual-Stage Filtering & Lockout Concept**:

1. **Transient Signal Candidate Sensing:**
   The system continuously monitors vertical impact acceleration ($|aZ| > 1.08g$) and angular pitch velocity ($|\omega_{\text{pitch}}| > 80\text{ deg/s}$). When both foot sensors report threshold breaches in the same sampling frame, the algorithm dynamically selects the dominant foot based on peak angular momentum.

2. **Independent Per-Foot Lockout State Machine ($800\text{ ms}$):**
   * **The Problem:** During a single foot contact or roll-off, secondary micro-impacts (e.g. heel strike followed by ball contact) can cause multiple false step events on the *same* leg.
   * **The Lockout Concept:** The system maintains independent last-step timestamps for each leg (`lastStepTimeLeft` and `lastStepTimeRight`). Whenever a candidate step is detected for a leg, the state machine checks if the time elapsed since the previous step *on that specific leg* is less than $800\text{ ms}$.
   * **Behavior:** If the elapsed time is $< 800\text{ ms}$ on the same leg, the event is suppressed as a secondary vibration. If the dancer switches feet (`Left -> Right`), the opposite leg's timer is checked, allowing rapid alternating steps (such as Triple Steps at $220\text{ to }250\text{ ms}$ intervals) to pass without latency.

---

## 5. UI Architecture & Camera HUD Overlay

The Solo Training Dashboard is optimized for mobile browser use (tablets/smartphones mounted on a tripod facing the dancer):

* **Transparent WebRTC Camera HUD:** The HTML video element is fixed in the background (`z-index: -1`). Dashboard cards utilize **35% background opacity** (`rgba(10, 14, 22, 0.35)`) and **2px backdrop blur** (`backdrop-filter: blur(2px)`), allowing the dancer to view their body alignment directly behind the live telemetry curves.
* **Controls Header:** 
  * `📷 CAM`: Activates WebRTC user media video stream.
  * `🔄 FLIP`: Toggles between front (`user`) and rear (`environment`) cameras.
  * `⛶ FULL`: Triggers Native Fullscreen API to maximize screen real estate.
  * `📐 ZERO`: Recalibrates static instep pitch angles for both feet.
  * `🔊 Audio`: Toggles Web Audio API synthesized biofeedback tones ON/OFF.

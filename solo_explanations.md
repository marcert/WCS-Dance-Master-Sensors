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

1. **Pitch Angle Integration ($\theta_{\text{raw}}$):**
   Continuously integrated from gyro pitch angular velocity ($\omega_{\text{pitch}}$ in $\text{deg/s}$):
   
   $$\theta_{\text{raw}}(t) = \theta_{\text{raw}}(t - \Delta t) + (\omega_{\text{pitch}} \times \Delta t)$$

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

* **Soft Cushioning ($1\text{ to }15\text{ g/s}$):** Excellent joint absorption (`SOFT`).
* **Moderate Shock ($15\text{ to }20\text{ g/s}$):** Acceptable impact.
* **Harsh Stomping ($> 20\text{ g/s}$):** Excessive shock transmitted to joints; triggers a 500 Hz low-frequency impact click.

---

### C. Double Stance Overlap ($\Delta t_{\text{double-stance}}$) & Grounding Ratio
West Coast Swing emphasizes a continuous, grounded "rolling" weight transfer rather than abrupt hopping or lifting off the floor prematurely. 

$$\text{Stance Ratio} = \left( \frac{\Delta t_{\text{double-stance}}}{t_{\text{step}}} \right) \times 100\%$$

#### Why Overlap Matter in WCS Mechanics:
* **Grounded Rolling Action:** In West Coast Swing, weight transfer is gradual. As one foot leaves the floor, the other receives weight, creating a natural bilateral overlap phase where both soles touch the ground ($|aZ| > 0.65g$).
* **Elastic Extension & Timing:** A healthy overlap ratio ($18\%\text{ to }38\%$) creates the characteristic "elastic" stretch and smooth momentum transfer in WCS. Too little overlap indicates rushing or bouncing, while too much overlap results in heavy, sluggish transitions.

| Ratio Range (%) | Badge Rating | Biomechanical Meaning |
| :---: | :---: | :--- |
| **18% to 38%** | `OPTIMAL ROLL` | Ideal grounded roll-off phase for walks and extensions. |
| **< 18%** | `HECTIC` | Rushed weight transfer; lack of rolling articulation through the foot. |
| **> 38%** | `SLUGGISH` | Over-invested ground contact; sluggish tempo transition. |

---

### D. Roll-off Symmetry Index (ASI) & Smoothness Index

1. **Asymmetry Index (ASI):**
   Compares total angular work integrated across Left and Right foot roll-off cycles:
   
   $$\text{ASI} = \left| 1.0 - \frac{\int |\omega_{\text{left}}| \, dt}{\int |\omega_{\text{right}}| \, dt} \right| \times 100\%$$
   
   * **Target:** $< 15\%$ (Indicates equal roll-off articulation on both legs).

2. **Roll-Smoothness Index:**
   Measures angular acceleration jerk $(d\omega / dt)$ smoothed over a 25-frame ($0.5\text{ s}$) sliding window:
   
   $$\text{Smoothness} = \text{Mean}_{25}\left(\left| \frac{\Delta \omega_{\text{pitch}}}{\Delta t} \right| \times 0.15\right)$$

   * **Target:** Lower values ($0\text{ to }15$) indicate fluid, continuous ankle articulation without micro-stutters.

---

## 3. Configured Thresholds & Audio Biofeedback Summary

| Metric / Parameter | Value / Range | Visual Badge / State | Audio Biofeedback |
| :--- | :--- | :--- | :--- |
| **Forward Heel Angle** | $10^\circ \text{ to } 25^\circ$ | `OPTIMAL HEEL` (Green) | None |
| **Forward Flat Foot** | $< 5^\circ$ | `FLAT-FOOT!` (Red) | 1200 Hz Sine Click (80 ms) |
| **Backward Toe Angle** | $-20^\circ \text{ to } +5^\circ$ | `OPTIMAL TOE` (Green) | None |
| **Backward Heel Error** | $> 10^\circ$ | `HEEL LANDING!` (Red) | 1200 Hz Warning Beep (80 ms) |
| **Impact Jerk ($J_{\text{impact}}$)** | $> 20\text{ g/s}$ | Flash Card Boundary | 500 Hz Low Impact Click (80 ms) |
| **Double Stance Ratio** | 18% to 38% | `OPTIMAL ROLL` (Green) | None |
| **Double Stance Hectic** | $< 18\%$ | `HECTIC` (Yellow) | None |
| **Double Stance Sluggish**| $> 38\%$ | `SLUGGISH` (Yellow) | None |
| **Per-Foot Lockout Window**| $800\text{ ms}$ | Suppresses same-foot re-trigger | None |

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

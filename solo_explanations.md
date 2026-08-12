# WCS Solo-Training Dashboard: Biomechanical & Technical Documentation

This document provides a comprehensive breakdown of the biomechanical metrics, sensor fusion algorithms, threshold configurations, state machine lockout logic, and acoustic biofeedback mechanisms implemented in the **WCS Solo-Training Dashboard** (`/solo`).

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

### A. Kinematic Roll-off Phases (Rocker Model)
West Coast Swing foot mechanics follow a 4-phase biomechanical roll-off model (*Rocker Model*) during forward and backward weight transfers:

```text
Forward Step (Heel-Strike / Heel-Ball-Toe):
  [Initial Contact]  -->  [Loading Response]  -->  [Midstance]  -->  [Push-off (Windlass)]
    (Heel Strike)          (Eccentric Control)     (Foot Flat)        (Toe Propulsion)
      θ > 10°                  Plantarflexion       θ ≈ 0°            -ω_pitch > 120°/s

Backward Step (Toe-Landing / Toe-Ball-Heel):
  [Toe Contact]      -->  [Lowering Phase]    -->  [Full Weight Investment (Anchor Settle)]
    (Ball / Scouts)        (Eccentric Ankle)       (Heel Kisses Floor & COM Shift)
      -20° ≤ θ ≤ 5°           θ → 0°                  θ ≈ -2° to 5°, aZ > 0.85g
```

---

### B. Foot Strike Angle ($\theta$) & Landing Articulation

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
     * $\theta > 35^\circ \longrightarrow$ `HEEL SPIKE` (Gyro overshoot during fast swing)
   * **Backward Step / Anchor:**
     * $-2^\circ \le \theta \le 5^\circ$ AND $aZ > 0.85g \longrightarrow$ `ANCHOR SETTLE` (Full weight investment & leverage)
     * $-20^\circ \le \theta \le 5^\circ \longrightarrow$ `OPTIMAL TOE` (Clean toe-ball roll-off)
     * $5^\circ < \theta \le 10^\circ \longrightarrow$ `HEEL DROP` (Borderline flat landing)
     * $\theta > 10^\circ \longrightarrow$ `HEEL LANDING!` (Biomechanical error: heel landing while moving backward; triggers 1200 Hz audio warning)
   * **Ambiguous / Touch Step:**
     * $|\theta| < 5^\circ \longrightarrow$ `TOUCH / TAP` (Superficial touch step without full weight transfer)

---

### C. Terminal Stance & Power Push Propulsion
West Coast Swing propulsion requires an active toe push-off (*Windlass Mechanism*) from the trailing leg at the end of the stance phase:

* **Detection Condition:**
  $$-\omega_{\text{pitch}} > 120^\circ/\text{s} \quad \text{AND} \quad aY > 0.15g$$
* **Dashboard Feedback:** Displays **`POWER PUSH 🚀`** badge, rewarding dynamic forward propulsion over the ball of the trailing foot.

---

### D. Impact Jerk ($J_{\text{impact}}$) & Shock Absorption
Impact Jerk quantifies the rate of change of vertical impact acceleration ($aZ$ in $g$) upon step landing. It measures how effectively the knee and ankle joints cushion foot placement:

$$J_{\text{impact}} = \left| \frac{aZ_{\text{current}} - aZ_{\text{previous}}}{\Delta t} \right| \quad [\text{g/s}]$$

* **Soft Cushioning ($1\text{ to }15\text{ g/s}$):** Excellent joint absorption (`SOFT`).
* **Moderate Impact ($15\text{ to }30\text{ g/s}$):** Acceptable step impact.
* **Harsh Stomping ($> 30\text{ g/s}$ or $J_{\text{native}} > 120$):** Excessive shock transmitted to joints; triggers a 500 Hz low-frequency impact click.

---

### E. Double Stance Overlap ($\Delta t_{\text{double-stance}}$) & Grounding Ratio
West Coast Swing emphasizes a continuous, grounded "rolling" weight transfer rather than abrupt hopping or lifting off the floor prematurely. Ground contact is registered when vertical acceleration exceeds static gravity baseline ($|aZ| > 0.55g$).

$$\text{Stance Ratio} = \left( \frac{\Delta t_{\text{double-stance}}}{t_{\text{step}}} \right) \times 100\%$$

#### Why Overlap Matters in WCS Mechanics:
* **Grounded Rolling Action:** In West Coast Swing, weight transfer is gradual. As one foot leaves the floor, the other receives weight, creating a natural bilateral overlap phase where both soles touch the ground ($|aZ| > 0.55g$).
* **Elastic Extension & Timing:** A healthy overlap ratio ($18\%\text{ to }38\%$) creates the characteristic "elastic" stretch and smooth momentum transfer in WCS. Too little overlap indicates rushing or bouncing, while too much overlap results in heavy, sluggish transitions.

| Ratio Range (%) | Badge Rating | Biomechanical Meaning |
| :---: | :---: | :--- |
| **18% to 38%** | `OPTIMAL ROLL` | Ideal grounded roll-off phase for walks and extensions. |
| **< 18%** | `HECTIC` | Rushed weight transfer; lack of rolling articulation through the foot. |
| **> 38%** | `SLUGGISH` | Over-invested ground contact; sluggish tempo transition. |

---

### F. Roll-off Symmetry Index (ASI) & Smoothness Index

1. **Asymmetry Index (ASI):**
   Compares total angular work integrated across Left and Right foot roll-off cycles while feet are actively moving ($|\omega_{\text{pitch}}| > 15^\circ/\text{s}$):
   $$\text{ASI} = \left| 1.0 - \frac{\int |\omega_{\text{left}}| \, dt}{\int |\omega_{\text{right}}| \, dt} \right| \times 100\%$$
   * **Target:** $< 10\%$ (`SYMMETRIC`), $11\text{--}25\%$ (`MINOR ASYM`), $>25\%$ (`ASYMMETRIC`).

2. **Roll-Smoothness Index:**
   Measures angular acceleration jerk $\left(\frac{d\omega}{dt}\right)$ smoothed over a 25-frame ($0.5\text{ s}$) sliding window:
   $$\text{Smoothness} = 100 - \text{Mean}_{25}\left(\left| \frac{\Delta \omega_{\text{pitch}}}{\Delta t} \right| \times 0.018\right)$$
   * **Target:** Higher values ($\ge 65$) indicate `SMOOTH`, continuous ankle articulation without micro-stutters.

---

## 3. Configured Thresholds & Audio Biofeedback Summary

| Metric / Parameter | Value / Range | Visual Badge / State | Audio Biofeedback |
| :--- | :--- | :--- | :--- |
| **Forward Heel Angle** | $10^\circ \text{ to } 25^\circ$ | `OPTIMAL HEEL` (Green) | None |
| **Forward Flat Foot** | $< 5^\circ$ | `FLAT-FOOT!` (Red) | 1200 Hz Sine Click (80 ms) |
| **Anchor Settle** | $-2^\circ \text{ to } +5^\circ$ AND $aZ > 0.85g$ | `ANCHOR SETTLE` (Green) | None |
| **Backward Toe Angle** | $-20^\circ \text{ to } +5^\circ$ | `OPTIMAL TOE` (Green) | None |
| **Backward Heel Error** | $> 10^\circ$ | `HEEL LANDING!` (Red) | 1200 Hz Warning Beep (80 ms) |
| **Trailing Foot Push-off**| $-\omega_{\text{pitch}} > 120^\circ/\text{s}$ AND $aY > 0.15g$ | `POWER PUSH 🚀` (Green) | None |
| **Impact Jerk ($J_{\text{impact}}$)** | $> 30\text{ g/s}$ | Flash Card Boundary | 500 Hz Low Impact Click (80 ms) |
| **Double Stance Ratio** | 18% to 38% | `OPTIMAL ROLL` (Green) | None |
| **Double Stance Hectic** | $< 18\%$ | `HECTIC` (Yellow) | None |
| **Double Stance Sluggish**| $> 38\%$ | `SLUGGISH` (Yellow) | None |
| **Per-Foot Lockout Window**| $220\text{ ms}$ + Alternation Guard | Suppresses same-foot re-trigger | None |

---

## 4. Signal Filtering & Lockout Concept

To prevent false secondary step triggers caused by micro-taps, foot unweighting, or floor vibrations, the DSP pipeline executes a **Dual-Stage Filtering & Lockout Concept**:

1. **Transient Signal Candidate Sensing:**
   The system continuously monitors vertical impact acceleration ($|aZ| > 1.08g$) and angular pitch velocity ($|\omega_{\text{pitch}}| > 80\text{ deg/s}$ with $preJerk > 8$). When both foot sensors report threshold breaches in the same sampling frame, the algorithm dynamically selects the dominant foot based on peak ground reaction force ($|aZ|$).

2. **Per-Foot 220 ms Lockout & Alternation Guard:**
   * **The Lockout Concept:** The system maintains independent last-step timestamps for each leg (`lastStepTimeLeft` and `lastStepTimeRight`). Whenever a candidate step is detected for a leg, the state machine checks if the time elapsed since the previous step *on that specific leg* is less than $220\text{ ms}$.
   * **Alternation Guard:** Steps must alternate (`Left -> Right -> Left`). If the same foot fires twice without the opposite foot making contact in between, it is discarded as a liftoff re-detection or vibration ghost.

---

## 5. UI Architecture & Camera HUD Overlay

The Solo Training Dashboard is optimized for mobile browser use (tablets/smartphones mounted on a tripod facing the dancer):

* **Transparent WebRTC Camera HUD:** The HTML video element is fixed in the background (`z-index: -1`). Dashboard cards utilize **35% background opacity** (`rgba(10, 14, 22, 0.35)`), allowing the dancer to view their body alignment directly behind the live telemetry curves.
* **Responsive Portrait & Landscape Split:**
  * **Portrait:** Vertical layout for tripod viewing.
  * **Landscape:** 2-column split view (Live Pitch Graph on the left, 2x2 Metric Grid on the right) with zero vertical scrolling.
* **Controls Header:** 
  * `📷 CAM`: Activates WebRTC user media video stream.
  * `🔄 FLIP`: Toggles between front (`user`) and rear (`environment`) cameras.
  * `⛶ FULL`: Triggers Native Fullscreen API to maximize screen real estate.
  * `📐 ZERO`: Recalibrates static instep pitch angles for both feet.
  * `🔊 Audio`: Toggles Web Audio API synthesized biofeedback tones ON/OFF.

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
   
   $$\theta_{\text{raw}}(t) = 0.94 \times \bigl(\theta_{\text{raw}}(t - \Delta t) + \omega_{\text{pitch}} \times \Delta t\bigr) + 0.06 \times \arctan2(a_Y, a_Z) \times \frac{180}{\pi}$$
   
   The 6 % accelerometer contribution corrects up to ~1°/s drift per frame without degrading dynamic response. The right sensor is mounted with its Z-axis reading +1g at rest (rotation around the shoe's vertical axis leaves Z unchanged), so its accelerometer term uses $\arctan2(a_Y, a_Z)$ — the same formula as the left sensor, without negation.

2. **Zero-Tare Compensation ($\theta_{\text{calibrated}}$):**
   To adjust for individual instep shoe slopes, the `📐 ZERO` button captures static mounting offsets ($\text{leftMountOffset}$, $\text{rightMountOffset}$):
   
   $$\theta = \theta_{\text{raw}} - \text{mountOffset}$$

3. **Strike Angle Evaluation Rules:**
   * **Forward Step:**
     * $10^\circ \le \theta \le 35^\circ \longrightarrow$ `OPTIMAL HEEL` (Clean heel articulation)
     * $5^\circ \le \theta < 10^\circ \longrightarrow$ `FLAT` (Borderline flat landing)
     * $\theta < 5^\circ$ (including negative) $\longrightarrow$ `FLAT-FOOT!` (Toe-down or flat landing; triggers 1200 Hz audio only when impact jerk > 40 g/s)
   * **Backward Step:**
     * $-20^\circ \le \theta \le 5^\circ \longrightarrow$ `OPTIMAL TOE` (Clean toe-ball roll-off)
     * $5^\circ < \theta < 10^\circ \longrightarrow$ `HEEL DROP` (Heel beginning to sink — caution)
     * $\theta \ge 10^\circ \longrightarrow$ `HEEL LANDING!` (Biomechanical error: heel landing while moving backward; triggers 1200 Hz audio)

   > **Sign convention:** $\theta$ is the signed pitch angle relative to the tare zero. Positive = heel higher than toe; negative = toe higher than heel. The HUD displays the raw signed value so the polarity immediately explains the badge (e.g. $-8°$ FLAT-FOOT! = toe dipped during a forward step).

---

### B. Impact Jerk ($J_{\text{impact}}$) & Shock Absorption
Impact Jerk quantifies the rate of change of vertical impact acceleration ($aZ$ in $g$) upon step landing. It measures how effectively the knee and ankle joints cushion foot placement:

$$J_{\text{impact}} = \left| \frac{aZ_{\text{current}} - aZ_{\text{previous}}}{\Delta t} \right| \quad [\text{g/s}]$$

* **Soft Cushioning ($< 60\text{ g/s}$):** Good joint absorption.
* **Moderate Shock ($60\text{ to }120\text{ g/s}$):** Acceptable impact level.
* **Harsh Stomping ($> 120\text{ g/s}$):** Excessive shock transmitted to joints; triggers a 500 Hz low-frequency impact click.

> Thresholds use the native 200 Hz sensor time step ($\Delta t = 0.005\text{ s}$) as the denominator — this yields values 4× higher than a poll-rate-based calculation would, which is why the numbers are larger than comparable literature figures using longer time windows.

---

### C. Double Stance Overlap ($\Delta t_{\text{double-stance}}$) & Grounding Ratio
West Coast Swing emphasizes a continuous, grounded "rolling" weight transfer rather than abrupt hopping or lifting off the floor prematurely. 

$$\text{Stance Ratio} = \left( \frac{\Delta t_{\text{double-stance}}}{t_{\text{step}}} \right) \times 100\%$$

#### Why Overlap Matter in WCS Mechanics:
* **Grounded Rolling Action:** In West Coast Swing, weight transfer is gradual. As one foot leaves the floor, the other receives weight, creating a natural bilateral overlap phase where both soles touch the ground ($|aZ| > 0.70g$).
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
   
   $$\text{Jerkiness} = \text{clamp}_{0\text{–}100}\!\left[\left(\left|\frac{\Delta \omega_{\text{L}}}{\Delta t}\right| + \left|\frac{\Delta \omega_{\text{R}}}{\Delta t}\right|\right) \times 0.018\right]$$
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
| **Forward Heel Angle** | $10^\circ \text{ to } 35^\circ$ | `OPTIMAL HEEL` (Green) | None |
| **Forward Flat Foot** | $< 5^\circ$ (incl. negative) | `FLAT-FOOT!` (Red) | 1200 Hz Sine Click — only when jerk > 40 g/s |
| **Backward Toe Angle** | $-20^\circ \text{ to } +5^\circ$ | `OPTIMAL TOE` (Green) | None |
| **Backward Heel Drop** | $5^\circ < \theta < 10^\circ$ | `HEEL DROP` (Yellow) | None |
| **Backward Heel Error** | $\ge 10^\circ$ | `HEEL LANDING!` (Red) | 1200 Hz Warning Beep (80 ms) |
| **Impact Jerk ($J_{\text{impact}}$)** | $> 120\text{ g/s}$ | Flash Card Boundary | 500 Hz Low Impact Click (80 ms) |
| **Double Stance Ratio** | 18% to 38% | `OPTIMAL ROLL` (Green) | None |
| **Double Stance Hectic** | $< 18\%$ | `HECTIC` (Yellow) | None |
| **Double Stance Sluggish**| $> 38\%$ | `SLUGGISH` (Yellow) | None |
| **Per-Foot Lockout Window**| $220\text{ ms}$ | Suppresses same-foot re-trigger | None |
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

2. **Independent Per-Foot Lockout State Machine ($220\text{ ms}$):**
   * **The Problem:** During a single foot contact or roll-off, secondary micro-impacts (e.g. heel strike followed by ball contact) can cause multiple false step events on the *same* leg.
   * **The Lockout Concept:** The system maintains independent last-step timestamps for each leg (`lastStepTimeLeft` and `lastStepTimeRight`). Whenever a candidate step is detected for a leg, the state machine checks if the time elapsed since the previous step *on that specific leg* is less than $220\text{ ms}$.
   * **Behavior:** If the elapsed time is $< 220\text{ ms}$ on the same leg, the event is suppressed as a secondary vibration. If the dancer switches feet (`Left -> Right`), the opposite leg's timer is checked, allowing rapid alternating steps (such as Triple Steps at $220\text{ to }250\text{ ms}$ intervals) to pass without latency.

---

## 5. UI Architecture & Camera HUD Overlay

The Solo Training Dashboard is optimized for mobile browser use (tablets/smartphones mounted on a tripod facing the dancer):

* **Transparent WebRTC Camera HUD:** The HTML video element is fixed in the background (`z-index: -1`). Dashboard cards use **15% background opacity** and **no backdrop blur**, so the dancer can see foot placement against the floor without the camera image being distorted.
* **Controls Header:** 
  * `📷 CAM`: Activates WebRTC user media video stream.
  * `🔄 FLIP`: Toggles between front (`user`) and rear (`environment`) cameras.
  * `⛶ FULL`: Triggers Native Fullscreen API to maximize screen real estate.
  * `📐 ZERO`: Recalibrates static instep pitch angles for both feet.
  * `🔊 Audio`: Toggles Web Audio API synthesized biofeedback tones ON/OFF.

---

## 6. How to Train with the Dashboard

This chapter explains how to set up a solo session, what each metric tells you in real time, and how to use the feedback to improve your WCS footwork.

---

### 6.1 Session Setup

1. **Mount the device** in landscape orientation on a tripod or stand, facing the floor area where you practice. Landscape layout is recommended — the pitch graph fills the left column and all four metric cards are visible on the right without scrolling.
2. **Tap 📷 CAM** to activate the camera overlay. You should see your feet and the floor behind the transparent HUD cards.
3. **Stand still on both feet in your natural dance stance.** Wait 2–3 seconds for the sensors to settle, then tap **📐 ZERO**. This captures the resting pitch angle of each shoe as the reference baseline. All subsequent step angles are measured relative to this zero — so zeroing in a relaxed, neutral position gives the most meaningful feedback.
4. **Tap 🔊 Biofeedback: ON** to enable audio alerts. The system uses two distinct tones:
   - **1200 Hz click** — biomechanical warning (FLAT-FOOT! on a hard impact, or HEEL LANDING! on any backward step)
   - **500 Hz thud** — high-impact jerk warning (stomping, excessive force)
5. Tap **⛶ FULL** for fullscreen if using a phone, so none of the cards are cut off.

> Re-zero any time you change shoes, adjust how the sensor sits on your foot, or after a long break mid-session.

---

### 6.2 Reading the Step Badge Card

This is the primary real-time feedback card. It updates on every detected foot contact.

#### What the numbers mean

| Display element | What it tells you |
| :--- | :--- |
| **➡️ FORWARD / ⬅️ BACKWARD** | Direction of the step that just landed |
| **θ (signed angle)** | Pitch of your foot at the moment of landing, relative to flat. Positive = heel higher than toe; negative = toe higher than heel. |
| **Badge** | Classification of that landing (see table below) |
| **Jerk (g/s)** | Rate of impact force — how hard your foot hit the floor |

#### Forward step badges

| Badge | θ | What you did | What to aim for |
| :--- | :--- | :--- | :--- |
| `OPTIMAL HEEL` ✅ | +10° to +35° | Clean heel strike — forefoot lifted, heel contacts first | This is the target for all forward walks and breaks |
| `FLAT` ⚠️ | +5° to +10° | Slight heel lead, foot nearly flat | Acceptable but try to increase heel articulation |
| `FLAT-FOOT!` ❌ | < +5° | Foot landed flat or toe-first on a forward step | Common cause: rushing the step, tense ankles, or insufficient hip extension |
| `HEEL SPIKE` ⚠️ | > +35° | Extremely steep heel angle | Usually a fast, aggressive step; reduce drive force or relax the ankle |

#### Backward step badges

| Badge | θ | What you did | What to aim for |
| :--- | :--- | :--- | :--- |
| `OPTIMAL TOE` ✅ | −20° to +5° | Clean toe-ball landing — foot rolled onto the floor from the toe | Target for all backward walks, anchors, and extensions |
| `HEEL DROP` ⚠️ | +5° to +9° | Heel beginning to drop before full weight transfer | Slow down the transfer; keep the ankle dorsiflexed a moment longer |
| `HEEL LANDING!` ❌ | ≥ +10° | Heel struck first on a backward step | The most common WCS technique error — pulls the partner off-axis and kills momentum |
| `HEEL SPIKE` ⚠️ | < −20° | Extremely steep toe angle | Over-pointed foot; moderate the extension slightly |

#### Impact Jerk bar

- The bar below the step badge shows impact force on a colour scale.
- **Short bar, no 500 Hz thud** → soft, controlled landing. Ideal for most steps.
- **Full red bar + thud** → excessive stomping. Focus on bending the knee as the foot meets the floor and absorbing with the ankle rather than dropping the foot.

---

### 6.3 Reading the Double Stance Overlap Card

This card measures how long both feet are on the floor at the same time during each weight transfer.

| Badge | Ratio | What it means | Training implication |
| :--- | :--- | :--- | :--- |
| `OPTIMAL ROLL` ✅ | 18%–38% | Smooth, grounded weight transfer with natural bilateral overlap | This is the characteristic WCS "rolling" connection — maintain it |
| `HECTIC` ⚠️ | < 18% | Rushed transfer — one foot leaves before the other is secure | Slow down; think of rolling through the foot before lifting |
| `SLUGGISH` ⚠️ | > 38% | Prolonged double contact — hesitation or heavy stance | Common in beginners; work on committing weight earlier |

**Practical tip:** Watch this card during your triple steps and walks. A consistent `OPTIMAL ROLL` reading across a full 8-count pattern means your weight transfer timing is on. `HECTIC` appearing on your anchor step often means you are rushing out of the anchor before completing the connection.

---

### 6.4 Reading the ASI Card (Roll Symmetry)

The ASI compares the total rotational energy of your left foot against your right foot over a rolling window. A high ASI means one leg is doing noticeably more of the work.

| Badge | Range | Training implication |
| :--- | :--- | :--- |
| `SYMMETRIC` ✅ | ≤ 10% | Both legs contribute equally | Ideal — keep practicing at this intensity |
| `MINOR ASYM` ⚠️ | 11%–25% | Mild side dominance | Common and acceptable during learning; monitor for compensation patterns |
| `ASYMMETRIC` ❌ | > 25% | Significant side dominance | One leg is passive or over-active; drills to isolate the weaker side recommended |

**What causes high ASI?**
- Protecting an injury on one side
- Habitually favouring the dominant leg during turns or anchors
- Uneven shoe mounting (re-zero if ASI is consistently high on the same side from the start of a session)

**How to train it down:** Run the same pattern back-to-back, deliberately focusing attention on the lagging foot. The ASI card responds within a few seconds of changed movement quality.

---

### 6.5 Reading the Smoothness Index

This card measures how smoothly and continuously your feet articulate through each step. High jerkiness in the angular velocity of both feet registers as a low score.

| Badge | Score | Training implication |
| :--- | :--- | :--- |
| `SMOOTH` ✅ | ≥ 65 | Fluid, continuous ankle and knee articulation | Aim to spend most of a drill session in this range |
| `MODERATE` ⚠️ | 40–64 | Occasional abrupt transitions | Normal during direction changes; concern if it persists through straight walks |
| `ROUGH` ❌ | < 40 | Frequent micro-stutters | Indicates tension — usually tight ankles, locked knees, or rushing |

**Practical tip:** The smoothness score drops sharply during abrupt stops or when you break connection and restart. Use it as a "relaxation indicator" — if it stays in `ROUGH` throughout a drill, consciously soften your knees and let the ankles absorb rather than resist.

---

### 6.6 Pitch Graph (Live Roll-off Dynamics)

The two lines on the graph show the real-time pitch angular velocity of each foot (cyan = left, magenta = right, in deg/s). This is not a step counter — it is a continuous velocity signal showing how actively each foot is rotating at any given moment.

- **Large spikes** → fast foot rotations (heel strikes, toe push-offs)
- **Flat near zero** → foot stationary on the floor or in the air with little rotation
- **Symmetric spike patterns** on both lines during a triple step = good bilateral engagement
- **One line nearly flat while the other spikes** = passive trailing foot — a common indicator of side dominance that will also show up in the ASI card

---

### 6.7 Common Patterns and How to Fix Them

| What you see on the HUD | Likely cause | Drill to fix |
| :--- | :--- | :--- |
| Repeated `FLAT-FOOT!` on forward steps | Ankle not dorsiflexed on walks; rushing the step | Slow walks in place, exaggerating the heel-lead; pause at the moment of contact and check your foot angle |
| Repeated `HEEL LANDING!` on backward steps | Stepping back onto the heel instead of the toe-ball | Practice slow backward walks in socks on a smooth floor, consciously reaching the toe to the floor first |
| `HECTIC` stance ratio throughout | Over-quick weight transfer; bouncing style | Pause counts: hold each weight transfer an extra beat before lifting the free foot |
| `SLUGGISH` stance ratio throughout | Not committing weight; staying on both feet too long | Triple-step isolations, focusing on a clear single-foot weight commitment on every beat |
| ASI > 25% consistently | Side dominance | Practise patterns starting on the non-dominant foot; mirror the drill |
| Smoothness stays `ROUGH` | Tension in ankles or knees | Reduce tempo by 20–30 BPM; focus on soft, bent knees throughout |
| Jerk values consistently > 120 g/s | Stomping; impact not absorbed | Barefoot slow walks concentrating on the ankle absorbing the landing; visualise landing on a thin layer of foam |

---

### 6.8 Recommended Drill Sequence

The following sequence lets you build up complexity while monitoring one metric at a time:

1. **Forward walk isolations (8 counts × 4 reps)**
   *Watch: Step badge. Goal: every forward step shows `OPTIMAL HEEL`.*

2. **Backward walk isolations (8 counts × 4 reps)**
   *Watch: Step badge. Goal: every backward step shows `OPTIMAL TOE`. Zero `HEEL LANDING!` alerts.*

3. **Continuous walks (forward + backward, no pause)**
   *Watch: Double Stance card. Goal: `OPTIMAL ROLL` on every transition.*

4. **Triple-step sequence (WCS basic pattern)**
   *Watch: Smoothness + ASI. Goal: Smoothness ≥ 65 across the full pattern; ASI ≤ 25%.*

5. **Full 6-count WCS basic at tempo**
   *Watch: all cards simultaneously. Use audio alerts as passive warnings — the goal is to complete a full minute without a single `HEEL LANDING!` or high-jerk `FLAT-FOOT!` alarm.*

6. **Speed challenge: gradually increase tempo**
   *Watch whether `FLAT-FOOT!` and `HECTIC` increase as tempo rises — this identifies the speed threshold where technique breaks down.*


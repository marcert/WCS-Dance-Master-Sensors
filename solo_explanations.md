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
   
   $$\theta_{\text{raw}}(t) = 0.94 \times \bigl(\theta_{\text{raw}}(t - \Delta t) + \omega_{\text{pitch}} \times \Delta t\bigr) + 0.06 \times \theta_{\text{accel}}(t)$$
   
   where the accelerometer angle term $\theta_{\text{accel}}$ differs per sensor due to a confirmed physical mounting difference in the aY axis:
   
   | Sensor | Accelerometer Term | Reason |
   | :--- | :--- | :--- |
   | **Right foot** | $\arctan2(a_Y,\; a_Z) \times \tfrac{180}{\pi}$ | Standard convention: $a_Y > 0$ when heel is lower than toe |
   | **Left foot** | $\arctan2(-a_Y,\; a_Z) \times \tfrac{180}{\pi}$ | Left sensor's $a_Y$ axis is physically inverted; negation restores the shared sign convention |
   
   After this correction both sensors share the same convention: $\theta > 0$ means heel lower than toe (forward-step geometry), $\theta < 0$ means toe lower than heel (backward-step geometry). The **mount offset** at rest differs accordingly: right sensor ≈ 0°, left sensor ≈ −28° (auto-corrected by the TARE function).
   
   The 6% accelerometer contribution corrects up to ~1°/s drift per frame without degrading dynamic response.

2. **Zero-Tare Compensation ($\theta_{\text{calibrated}}$):**
   To adjust for individual instep shoe slopes, the `📐 ZERO` button captures static mounting offsets ($\text{leftMountOffset}$, $\text{rightMountOffset}$):
   
   $$\theta = \theta_{\text{raw}} - \text{mountOffset}$$

3. **Strike Angle Evaluation Rules:**

   > **Priority override — AMBIGUOUS / FLAT state:** If $|\theta| < 5°$ at trigger time, direction is unconditionally overridden to `↔️ FLAT` and the badge is `FLAT-FOOT!` regardless of foot movement direction. This occurs when the foot lands essentially flat and the sensor cannot reliably distinguish heel-first from toe-first contact.

   * **Forward Step (➡️ FORWARD):**
     * $\theta > 35^\circ \longrightarrow$ `HEEL SPIKE` ⚠️ (Excessive heel angle; reduce drive force or relax the ankle)
     * $10^\circ \le \theta \le 35^\circ \longrightarrow$ `OPTIMAL HEEL` ✅ (Clean heel articulation — target for all forward walks)
     * $5^\circ \le \theta < 10^\circ \longrightarrow$ `FLAT` ⚠️ (Borderline flat landing)
     * $\theta < 5^\circ$ (including negative) $\longrightarrow$ `FLAT-FOOT!` ❌ (Toe-down or flat landing; triggers 1200 Hz audio only when impact jerk > 40 g/s)
   * **Backward Step (⬅️ BACKWARD):**
     * $\theta \ge 10^\circ \longrightarrow$ `HEEL LANDING!` ❌ (Biomechanically impossible in correct WCS technique; triggers 1200 Hz audio)
     * $5^\circ \le \theta < 10^\circ \longrightarrow$ `HEEL DROP` ⚠️ (Heel sinking before full weight transfer — caution)
     * $-20^\circ \le \theta < 5^\circ \longrightarrow$ `OPTIMAL TOE` ✅ (Clean toe-ball roll-off — target for all backward walks)
     * $\theta < -20^\circ \longrightarrow$ `HEEL SPIKE` ⚠️ (Extremely steep toe angle; weight too far forward)
   * **Flat Contact (↔️ FLAT, $|\theta| < 5°$):**
     * Always shows `FLAT-FOOT!` badge. Direction is displayed as `↔️ FLAT` regardless of movement.

   > **Sign convention:** $\theta = \theta_{\text{CF}}(t{-}1) - \text{mountOffset}$. Positive = heel lower than toe (forward-step geometry); negative = toe lower than heel (backward-step geometry). Both sensors share this convention after the left-sensor $a_Y$ negation. Note that $\theta$ is sampled from the **T-1 complementary-filter angle** (see Section 4), not from the instantaneous accelerometer reading at the trigger frame.

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
| **Flat Contact Override** | $|\theta| < 5°$ | `↔️ FLAT` + `FLAT-FOOT!` (overrides direction) | 1200 Hz click — only when jerk > 40 g/s |
| **Forward Heel Spike** | $\theta > 35°$ | `HEEL SPIKE` (Yellow) | None |
| **Forward Optimal Heel** | $10° \text{ to } 35°$ | `OPTIMAL HEEL` (Green) | None |
| **Forward Flat Foot** | $< 5°$ (incl. negative) | `FLAT-FOOT!` (Red) | 1200 Hz Sine Click — only when jerk > 40 g/s |
| **Backward Heel Error** | $\ge 10°$ | `HEEL LANDING!` (Red) | 1200 Hz Warning Beep (80 ms) |
| **Backward Heel Drop** | $5° < \theta < 10°$ | `HEEL DROP` (Yellow) | None |
| **Backward Optimal Toe** | $-20° \text{ to } +5°$ | `OPTIMAL TOE` (Green) | None |
| **Backward Toe Spike** | $< -20°$ | `HEEL SPIKE` (Yellow) | None |
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

## 4. Signal Filtering & Step Detection Pipeline

To reliably detect genuine foot contacts while suppressing false triggers (vibration, liftoff ghosts, swing-phase inertia), the pipeline executes four sequential stages on every 20 ms poll frame.

---

### Stage 1 — Impact Signal Candidates

Both feet are evaluated independently each frame:

$$\text{signal}_f = \text{online}_f \;\wedge\; \Bigl(|aZ_f| > 1.08g \;\;\vee\;\; \bigl(|\omega_f| > 80\tfrac{°}{s} \;\wedge\; \text{preJerk}_f > 8\bigr)\Bigr)$$

where:

$$\text{preJerk}_f = \frac{|aZ_{f,\,t} - aZ_{f,\,t-1}|}{0.005\text{ s}}$$

**Why two trigger paths?**
- The $|aZ| > 1.08g$ path catches hard heel and ball-of-foot impacts.
- The $|\omega| > 80°/s$ path catches soft or fast toe contacts where the vertical spike is mild but foot rotation is high.
- The **preJerk gate** ($> 8$) on the gyro path suppresses **liftoff and rotation ghosts**: high-gPitch events where the foot is rotating in the air or pivoting on the floor without any associated vertical impact. Without this gate, liftoff events produce false same-foot re-detections.

---

### Stage 2 — Dual-Foot Tiebreaker

If both feet breach their thresholds simultaneously, the foot with higher **vertical ground-reaction force** wins:

$$\text{detectedFoot} = \underset{f \in \{L, R\}}{\operatorname{argmax}}\; |aZ_f|$$

The landing foot always receives the floor's reaction force; the swinging or pushing-off foot can have high $|\omega|$ but lower $|aZ|$. Using $|aZ|$ as the tiebreaker makes the selection robust to asymmetric swing dynamics.

---

### Stage 3 — Per-Foot Lockout + Alternation Guard

Two independent 220 ms timers prevent rapid same-foot re-triggers. An additional **alternation guard** rejects any two consecutive detections on the same foot without an intervening detection on the opposite foot (L→L or R→R sequences). This eliminates liftoff-vibration chains where a zero-jerk ghost on the opposite sensor resets the alternation state and allows the same foot to fire twice.

| Condition | Action |
| :--- | :--- |
| Same foot, $\Delta t < 220\text{ ms}$ | Hard lockout — event suppressed |
| Same foot twice in a row (L→L or R→R) | Alternation guard — event suppressed |
| Alternating foot, $\Delta t \ge 220\text{ ms}$ | Event accepted |

---

### Stage 4 — T-1 CF Angle: Direction & Theta Snapshot

At the trigger frame $t$, both $\theta$ and direction are read from the **previous frame's complementary-filter angle** $\theta_{\text{CF}}(t{-}1)$, not from the instantaneous accelerometer reading at $t$:

$$\theta_{\text{active}} = \theta_{\text{CF}}(t{-}1) - \text{mountOffset}$$

$$\text{direction} = \begin{cases} \text{BACKWARD} & \text{if } \theta_{\text{CF}}(t{-}1) < \text{mountOffset} \\ \text{FORWARD} & \text{otherwise} \end{cases}$$

**Why T-1?**
The $|aZ| > 1.08g$ trigger fires at **weight transfer** — a late-landing event that occurs after the foot has already rolled from its initial contact geometry toward a more neutral or heel-down position (the *roll-through artifact*). Reading $\theta$ at frame $t$ would capture this post-roll orientation. At frame $t{-}1$ the CF angle tracks the foot's pre-contact approach:

| Step type | T-1 foot orientation | $\theta_{\text{active}}$ | Direction |
| :--- | :--- | :--- | :--- |
| Forward (heel-first) | Heel lower than toe | Positive | ➡️ FORWARD |
| Backward (toe-first) | Toe lower than heel | Negative | ⬅️ BACKWARD |

The CF angle at T-1 is preferred over the raw accelerometer angle because the 94% gyro weighting makes it immune to single-frame accelerometer noise while still tracking rapid foot rotation faithfully.

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
| **➡️ FORWARD** | The step that just landed was a forward step |
| **⬅️ BACKWARD** | The step that just landed was a backward step |
| **↔️ FLAT** | Foot landed nearly flat (&#124;θ&#124; < 5°) — direction is ambiguous; the sensor cannot reliably distinguish forward from backward at this angle |
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


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
 |  Inverted aY Mounting    |           | Standard Mounting        |
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
* **Central Master Unit:** Aggregates ESP-NOW streams and delivers JSON data packets (`lG`, `lA`, `lAy`, `lGr`, `rG`, `rA`, `rAy`, `rGr`) to the browser via the `/data` endpoint.
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

### B. Foot Pitch Angle ($\theta$) & Landing Articulation

> **Measurement note:** θ measures the sagittal inclination of the *foot segment* relative to gravity (Foot Inclination Angle), not the anatomical ankle (talocrural) joint angle. The anatomical ankle angle would require a second sensor on the tibia. Statements like "dorsiflexion" below refer to the foot-segment interpretation.

1. **Complementary Filter Pitch Angle ($\theta_{\text{raw}}$):**
   Gyro and accelerometer are fused at α = 0.94 (τ ≈ 78 ms). The T-1 snapshot (taken one frame *before* the impact trigger) already protects the θ estimate from impact corruption — higher α values were tested but caused dθ compression and gyro spike artifacts. The accelerometer reference angle differs per sensor due to physical mounting:

   | Sensor | Accelerometer reference | Reason |
   | :--- | :--- | :--- |
   | Right (ID 2) | $\theta_{\text{accel}} = \text{atan2}(aY_R,\; aZ_R)$ | Standard orientation |
   | Left (ID 1) | $\theta_{\text{accel}} = \text{atan2}(-aY_L,\; aZ_L)$ | aY axis physically inverted by mounting |

   $$\theta_{\text{raw}}(t) = \alpha \cdot \bigl(\theta_{\text{raw}}(t-\Delta t) + \omega_{\text{pitch}} \cdot \Delta t\bigr) + (1-\alpha) \cdot \theta_{\text{accel}}, \quad \alpha = 0.94$$

   **T-1 Snapshot for step classification:** At the moment of impact, the angle from the *previous frame* (T-1) is used — not the instantaneous value. The aZ > 1.08 g trigger fires after partial weight loading when roll-through has already begun; the T-1 frame captures pre-contact foot orientation before distortion.

2. **Zero-Tare Compensation ($\theta_{\text{calibrated}}$):**
   To adjust for individual instep shoe slopes, the `📐 ZERO` button captures static mounting offsets ($\text{leftMountOffset}$, $\text{rightMountOffset}$):
   $$\theta = \theta_{\text{raw}} - \text{mountOffset}$$

3. **Direction Classification & Strike Angle Evaluation:**

   Direction is determined in two stages. Stage 1 uses the T-1 calibrated pitch angle to resolve the unambiguous extremes; Stage 2 applies a 160 ms pitch-angle trend (dθ) to resolve the ambiguous zone.

   **Stage 1 — θ zone classification:**

   $$\text{activeDirection} = \begin{cases} \text{BACKWARD} & \theta \le -5° \\ \text{AMBIGUOUS} & -5° < \theta < 10° \\ \text{FORWARD} & \theta \ge 10° \end{cases}$$

   **Why asymmetric:** Negative θ reliably indicates toe-first contact (unambiguously backward); positive θ below +10° is genuinely ambiguous — a flat forward step and a backward step with an early heel drop both land in the same angle range (+5° to +9°).

   **Stage 2 — dθ pitch trend (Ambiguous Zone only):**

   When Stage 1 yields AMBIGUOUS ($-5° < \theta < 10°$), the system computes a 160 ms pitch-angle trend from the pre-contact ring buffer (10 calibrated-θ samples at 50 Hz):
   $$d\theta = \theta_{\text{calibrated}}[T{-1}] - \theta_{\text{calibrated}}[T{-8}]$$

   $$\text{activeDirection} = \begin{cases} \text{FORWARD} & d\theta > +2° \\ \text{BACKWARD} & d\theta < -2° \\ \text{AMBIGUOUS} & |d\theta| \le 2° \end{cases}$$

   Forward swing = dorsiflexion → θ rises → positive dθ. Backward swing = plantarflexion → θ falls → negative dθ. Validated threshold: forward dθ +2.6° to +7.9°, backward dθ −2.3° to −3.1°, neutral/anchor −0.4° to +1.9°.

   * **Forward Step (FORWARD via $[\theta]$ or $[d\theta]$):**
     * $\theta > 35° \longrightarrow$ `HEEL SPIKE` (extreme dorsiflexion)
     * $10° \le \theta \le 35° \longrightarrow$ `OPTIMAL HEEL` (clean heel articulation)
     * BRUSH+HEEL reclassification window (200 ms) can upgrade any prior `FLAT-FOOT!` to `BRUSH+HEEL` if a second aZ > 1.05g peak with accelAngle > 8° is detected on the same foot.
   * **Backward Step (BACKWARD via $[\theta]$ or $[d\theta]$):**
     * Via $[\theta]$: $-5° > \theta \ge -20° \longrightarrow$ `OPTIMAL TOE`; $\theta < -20° \longrightarrow$ `HEEL SPIKE`
     * Via $[d\theta]$: `OPTIMAL TOE` for any θ not matching the sub-conditions below
     * $-2° \le \theta \le +5°$ via $[d\theta]$ AND $aZ > 0.85g$ → `ANCHOR SETTLE` (full weight, settled contact)
     * $+5° < \theta \le +9°$ via $[d\theta]$ → `HEEL DROP` (heel contacts early on backward step)
   * **Ambiguous ($|d\theta| \le 2°$, or ring buffer < 9 samples):**
     * ↔️ FLAT + `FLAT-FOOT!` (Red) — direction unresolvable by θ or dθ; also opens the 200 ms brush+heel reclassification window.
     * Covers: flat forward steps, backward steps without sufficient pitch trend, and genuinely flat landings.

---

### C. Terminal Stance & Power Push Propulsion
West Coast Swing propulsion requires an active toe push-off (*Windlass Mechanism*) from the trailing leg at the end of the stance phase. The optimal plantarflexion angular velocity depends on movement direction — forward propulsion demands more drive than the subtler redistribution at an anchor or backward walk.

* **Detection — two complementary signals:**
  * **Instantaneous peak:** $-\omega_{\text{pitch}} \ge 120^\circ/\text{s}$ AND $aY > 0.15g$ — catches short explosive pushes.
  * **Energy integral:** $\Phi_{\text{push}} = \int -\omega_{\text{pitch}}\,dt$ while $aY > 0.15g$, accumulated since last landing, reset at each step trigger — catches sustained lower-amplitude drives that the peak detector alone would miss.
  
  The $aY > 0.15g$ gate confirms floor shear force (Newton's Third Law translational component) and suppresses unweighted swing-leg artefacts. The final push level is the maximum of both signals — either a high peak *or* a sufficient integral qualifies as POWER PUSH.

* **Graded Feedback (holds 400 ms) — direction-dependent optimal thresholds:**

| Trailing foot last step | Peak $-\omega_{\text{pitch}}$ | Integral $\Phi_{\text{push}}$ | Badge | Meaning |
| :---: | :---: | :---: | :---: | :--- |
| BACKWARD (→ forward walk) | $\ge 200^\circ/\text{s}$ | $\ge 20°$ | `🚀 POWER PUSH` (Green) | Strong forward propulsion — target for walks and passes |
| FORWARD (→ anchor / backward walk) | $\ge 160^\circ/\text{s}$ | $\ge 16°$ | `🚀 POWER PUSH` (Green) | Sufficient redistribution — lower drive expected at anchor |
| Either direction | peak $120\text{–}199^\circ/\text{s}$ OR integral $\ge 12°$ | | `↗ PUSH` (Yellow) | Push-off detected but below directional optimum |
| Either direction | peak $< 120^\circ/\text{s}$ AND integral $< 12°$ | | `— PUSH-OFF` (Grey) | No significant push-off detected |

> **Note:** The $-\omega_{\text{pitch}}$ values are foot-segment angular velocities, not anatomical ankle-joint velocities. Literature values (~250°/s) are for barefoot/athletic gait; 200°/s and 160°/s are performance thresholds calibrated for dance shoes on studio floors. The integral thresholds (12°/16°/20°) approximate 100 ms of sustained push at the corresponding peak velocities.

---

### D. Impact Jerk ($J_{\text{impact}}$) & Shock Absorption
Impact Jerk quantifies the rate of change of vertical acceleration ($aZ$ in $g$) at step landing — a proxy for how abruptly the kinetic chain receives load:

$$J_{\text{impact}} = \left| \frac{aZ_{\text{current}} - aZ_{\text{previous}}}{\Delta t} \right| \quad [\text{g/s}]$$

> **Unit note:** This $J$ is in $g/\text{s}$, not in $N/\text{s}$ or $\text{BW/s}$ as used in ground-reaction-force literature. The thresholds below are device- and algorithm-specific heuristics, not direct equivalents of GRF loading rate studies.

* **Soft Cushioning ($1\text{ to }15\text{ g/s}$):** Excellent joint absorption (`SOFT`).
* **Moderate Impact ($15\text{ to }30\text{ g/s}$):** Acceptable step impact.
* **Harsh Stomping ($> 30\text{ g/s}$ or $J_{\text{native}} > 120$):** Excessive shock transmitted to joints; triggers a 500 Hz low-frequency impact click.

---

### E. Double Stance Overlap ($\Delta t_{\text{double-stance}}$) & Grounding Ratio
West Coast Swing emphasizes a continuous, grounded "rolling" weight transfer rather than abrupt hopping or lifting off the floor prematurely. Ground contact is registered when vertical acceleration exceeds static gravity baseline ($|aZ| > 0.55g$).

> **Signal note:** $|aZ| > 0.55g$ is a sensor heuristic for bilateral ground contact — not a direct force measurement. Dynamic foot rotations can shift $aZ$ independently of actual floor contact. The thresholds below are calibrated empirically for this constraint.

$$\text{Stance Ratio} = \left( \frac{\Delta t_{\text{double-stance}}}{t_{\text{step}}} \right) \times 100\%$$

#### Why Overlap Matters in WCS Mechanics:
* **Grounded Rolling Action:** In West Coast Swing, weight transfer is gradual. As one foot leaves the floor, the other receives weight, creating a natural bilateral overlap phase where both soles touch the ground ($|aZ| > 0.55g$).
* **Elastic Extension & Timing:** A healthy overlap ratio ($18\%\text{ to }38\%$) creates the characteristic "elastic" stretch and smooth momentum transfer in WCS. Too little overlap indicates rushing or bouncing, while too much overlap results in heavy, sluggish transitions.
* **Note on scientific literature:** Biomechanical sources cite a single-foot stance phase of ~60% of the gait cycle. This is a different measurement — it describes how long *one* foot stays on the ground. The metric here measures the *simultaneous bilateral contact* ratio (both feet on the ground at the same time within one step cycle), which is a subset of and distinctly different from the single-foot stance phase.

| Ratio Range (%) | Badge Rating | Biomechanical Meaning |
| :---: | :---: | :--- |
| **18% to 38%** | `OPTIMAL ROLL` | Ideal grounded roll-off phase for walks and extensions. |
| **< 18%** | `HECTIC` | Rushed weight transfer; lack of rolling articulation through the foot. |
| **> 38%** | `SLUGGISH` | Over-invested ground contact; sluggish tempo transition. |

---

### F. Roll-off Symmetry Index (ASI) & Smoothness Index

1. **Asymmetry Index (ASI):**
   Compares total angular work integrated across Left and Right foot roll-off cycles while feet are actively moving ($|\omega_{\text{pitch}}| > 15^\circ/\text{s}$):
   $$\text{ASI} = \frac{2 \cdot \left|\int|\omega_{\text{left}}|\,dt - \int|\omega_{\text{right}}|\,dt\right|}{\int|\omega_{\text{left}}|\,dt + \int|\omega_{\text{right}}|\,dt} \times 100\%$$
   * **Target:** $< 10\%$ (`SYMMETRIC`), $11\text{--}25\%$ (`MINOR ASYM`), $>25\%$ (`ASYMMETRIC`).

2. **Roll-Smoothness Index:**
   Measures combined angular acceleration jerk of both feet smoothed over a 25-frame (0.5 s) sliding window:
   $$\text{Smoothness} = 100 - \text{Mean}_{25}\!\left(\min\!\left(100,\;\left(\left|\frac{\Delta\omega_L}{\Delta t}\right| + \left|\frac{\Delta\omega_R}{\Delta t}\right|\right) \times 0.018\right)\right)$$
   * **Target:** Higher values ($\ge 65$) indicate `SMOOTH`, continuous ankle articulation without micro-stutters.

---

### G. Weight Transfer Gradient & Ankle Shock Absorption

Both metrics are computed from a post-impact monitoring window that opens immediately after each step trigger.

**Weight Transfer Gradient** (240 ms window, 12 samples):
$$\text{loadRise} = \overline{aZ}_{[160\text{–}240\,\text{ms}]} - \overline{aZ}_{[0\text{–}80\,\text{ms}]}$$

| loadRise | Badge | Biomechanical Meaning |
| :---: | :---: | :--- |
| $> 0.12\,g$ | `SMOOTH LOAD` (Green) | Progressive weight transfer — COM moves gradually over foot |
| $-0.08\text{ to }+0.12\,g$ | `INSTANT LOAD` (Yellow) | Weight transferred immediately at impact — less joint protection |
| $< -0.08\,g$ | `EARLY UNLOAD` (Yellow) | Weight already shifting to next foot before settling — rushed transfer |

**Ankle Shock Absorption + Roll Reversal** (200 ms window, 10 samples of `gRoll`):

> **Measurement note:** `gRoll` measures rotation of the *shoe segment* around the sensor's roll axis, not directly the subtalar joint eversion angle. `rollIntegral` is a foot-rotation proxy for pronatory shock absorption; the reversal check is a proxy for the pronation→supination cycle that pre-loads the Windlass Mechanism. Both are validated as training indicators, not anatomical joint measurements.

Foot roll integral over first 100 ms (samples 0–4):
$$\text{rollIntegral} = \left|\sum_{i=0}^{4} \omega_{\text{roll},i} \times 0.02\,\text{s}\right| \quad [\text{degrees}]$$

Roll reversal check — sign change between early (samples 0–3) and late (samples 6–9) phase with sufficient roll magnitude:
$$\text{rollReversal} = |\overline{\omega}_{[0\text{–}3]}| > 8°/\text{s} \;\;\text{AND}\;\; \overline{\omega}_{[0\text{–}3]} \cdot \overline{\omega}_{[6\text{–}9]} < 0$$

| Condition | Badge | Biomechanical Interpretation |
| :---: | :---: | :--- |
| rollReversal = true | `RIGID LEVER` (Green) | Roll reversal detected — proxy for pronation→supination pre-loading (Windlass Mechanism) |
| rollIntegral $> 4°$ | `ANKLE FLEX` (Green) | Foot roll impulse detected — proxy for shock-absorbing pronation |
| rollIntegral $1°\text{–}4°$ | `MODERATE ROLL` (Yellow) | Some foot mobility, could be increased |
| rollIntegral $< 1°$ | `STIFF ANKLE` (Yellow) | Minimal roll — impact likely transmitted up the kinetic chain |
| Metric / Parameter | Value / Range | Visual Badge / State | Audio Biofeedback |
| :--- | :--- | :--- | :--- |
| **Forward Heel — Optimal** | $10^\circ \le \theta \le 35^\circ$ | `OPTIMAL HEEL` (Green) | None |
| **Forward Heel — Spike** | $\theta > 35^\circ$ | `HEEL SPIKE` (Yellow) | None |
| **Forward Brush+Heel** | flat → accelAngle $> 8^\circ$ within 200 ms | `BRUSH+HEEL` (Green) — reclassified from FLAT-FOOT! | None |
| **Ambiguous flat contact** | $-5° < \theta < 10°$ and $|d\theta| \le 2°$ (or buffer < 9 samples), no heel-set within 200 ms | ↔️ FLAT + `FLAT-FOOT!` (Red) — direction unresolvable by θ or dθ | 1200 Hz Click if Impact Jerk > 40 g/s |
| **Backward Toe — Optimal** | Via [θ]: $-20° \le \theta < -5°$; or via [dθ]: any θ in $-5°$ to $+9°$ not matching ANCHOR SETTLE or HEEL DROP | `OPTIMAL TOE` (Green) | None |
| **Backward Toe — Spike** | $\theta < -20°$ | `HEEL SPIKE` (Yellow) | None |
| **Backward — Anchor Settle** | BACKWARD via [dθ] + $-2° \le \theta \le +5°$ + $aZ > 0.85g$ (load indicator) | `ANCHOR SETTLE` (Green) — full-weight settled contact on backward step | None |
| **Backward — Heel Drop** | BACKWARD via [dθ] + $+5° < \theta \le +9°$ | `HEEL DROP` (Yellow) — heel contacts early on backward step | None |
| **Trailing Foot Push-off (forward, optimal)**| BACKWARD last step + $-\omega_{\text{pitch}} \ge 200^\circ/\text{s}$ AND $aY > 0.15g$ | `🚀 POWER PUSH` (Green) — real-time, holds 400 ms | None |
| **Trailing Foot Push-off (backward/anchor, optimal)**| FORWARD last step + $-\omega_{\text{pitch}} \ge 160^\circ/\text{s}$ AND $aY > 0.15g$ | `🚀 POWER PUSH` (Green) — real-time, holds 400 ms | None |
| **Trailing Foot Push-off (weak)** | Either direction, $120\text{–}159/199^\circ/\text{s}$ AND $aY > 0.15g$ | `↗ PUSH` (Yellow) — real-time, holds 400 ms | None |
| **Impact Jerk ($J_{\text{impact}}$)** | $> 30\text{ g/s}$ | Flash Card Boundary | 500 Hz Low Impact Click (80 ms) |
| **Double Stance Ratio** | 18% to 38% | `OPTIMAL ROLL` (Green) | None |
| **Double Stance Hectic** | $< 18\%$ | `HECTIC` (Yellow) | None |
| **Double Stance Sluggish**| $> 38\%$ | `SLUGGISH` (Yellow) | None |
| **Weight Transfer — Progressive** | loadRise $> 0.12\,g$ | `SMOOTH LOAD` (Green) | None |
| **Weight Transfer — Instant** | $-0.08 \le$ loadRise $\le 0.12$ | `INSTANT LOAD` (Yellow) | None |
| **Weight Transfer — Early Unload** | loadRise $< -0.08\,g$ | `EARLY UNLOAD` (Yellow) | None |
| **Rigid Lever** | pronation $> 8°/\text{s}$ AND sign reversal in 200 ms | `RIGID LEVER` (Green) | None |
| **Ankle Shock Absorption** | rollIntegral $> 4°$ (no reversal) | `ANKLE FLEX` (Green) | None |
| **Ankle Stiffness** | rollIntegral $< 1°$ | `STIFF ANKLE` (Yellow) | None |
| **Per-Foot Lockout Window**| $180\text{–}320\text{ ms}$ (cadence-adaptive) + Alternation Guard | Suppresses same-foot re-trigger | None |

---

## 4. Signal Filtering & Lockout Concept

To prevent false secondary step triggers caused by micro-taps, foot unweighting, or floor vibrations, the DSP pipeline executes a **Dual-Stage Filtering & Lockout Concept**:

1. **Transient Signal Candidate Sensing:**
   Each foot independently qualifies as an impact candidate via OR-logic:
   $$\text{signal}_{\text{foot}} = \bigl(|aZ| > 1.08\,g\bigr) \;\mathbf{OR}\; \bigl(|\omega_{\text{pitch}}| > 80\,\text{deg/s} \;\mathbf{AND}\; \text{preJerk} > 8\bigr)$$
   The `preJerk` gate (`|aZ_t - aZ_{t-1}| / Δt > 8`) on the gyro path suppresses liftoff rotation artefacts that would otherwise ghost as step triggers. When both feet signal in the same frame, the dominant foot is selected by peak ground reaction force: $\text{detectedFoot} = \arg\max(|aZ_L|, |aZ_R|)$.

2. **Per-Foot Cadence-Adaptive Lockout & Alternation Guard:**
   * **The Lockout Concept:** The system maintains independent last-step timestamps for each leg (`lastStepTimeLeft` and `lastStepTimeRight`). Whenever a candidate step is detected for a leg, the state machine checks if the time elapsed since the previous step *on that specific leg* is less than the dynamic lockout window.
   * **Cadence-Adaptive Window:** The lockout scales with the current step period: $t_{\text{lockout}} = \text{clamp}(t_{\text{step}} \times 0.55,\ 180\text{ ms},\ 320\text{ ms})$. At 120 BPM ($t_{\text{step}} = 500\text{ ms}$) this yields 275 ms; at 160 BPM (375 ms) → 206 ms; at 200 BPM (300 ms) → 180 ms (floor). This prevents both ghost triggers at slow tempos and missed steps at high tempos.
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

---

## 6. Dancer's Training Guide

> **This chapter has been extracted into a standalone file: [`dancer_guide_solo.md`](dancer_guide_solo.md)**
>
> The dancer guide is self-contained — dancers can open it directly without reading the technical sections of this document. It covers dashboard layout, the level selector (BEG / INT / ADV), all badge cards, skill-level training progressions, and a common-problems reference.
> For the partner/coach view see [`dancer_guide_partner.md`](dancer_guide_partner.md).
>
> The abbreviated section below is kept for quick cross-reference within this document.

---

### 6.1 Setup Before You Start

1. **Mount your phone or tablet on a tripod** at eye level, facing you. Landscape mode gives the best view.
2. **Tap `📷 CAM`** to activate the camera overlay. You will now see your live body behind the data cards.
3. **Put on your dance shoes** before the next step.
4. **Stand naturally in your dance stance** (feet shoulder-width, slight forward pitch). Tap `📐 ZERO`. The system now knows what "flat foot on the floor" feels like for your specific shoes and instep angle.
5. **Tap `🔊 Audio` ON.** The audio beeps are your real-time alarm system — they fire faster than you can read the screen.
6. Start walking or dancing. Give yourself 10–15 steps to warm up before you analyse anything.

> **Re-tare whenever you change shoes or surfaces.** The `📐 ZERO` calibration is shoe-specific.

---

### 6.2 Reading the Step Badge Card

This is the primary real-time feedback card. It updates on every detected foot contact.

#### What the numbers mean

| Display element | What it tells you |
| :--- | :--- |
| **➡️ FORWARD** | The step that just landed was a forward step |
| **⬅️ BACKWARD** | The step that just landed was a backward step |
| **↔️ FLAT** | Foot landed too flat to classify direction — treat as a flat-foot warning |
| **θ (signed angle)** | Pitch of your foot at the moment of landing, relative to flat. Positive = heel higher than toe; negative = toe higher than heel. |
| **Badge** | Classification of that landing (see table below) |
| **Jerk (g/s)** | Rate of impact force — how hard your foot hit the floor |

#### Forward step badges

| Badge | θ | What you did | What to aim for |
| :--- | :--- | :--- | :--- |
| `OPTIMAL HEEL` ✅ | +10° to +35° | Clean heel strike — forefoot lifted, heel contacts first | This is the target for all forward walks and breaks |
| `FLAT` ⚠️ | +5° to +10° | Slight heel lead, foot nearly flat | Acceptable but increase heel articulation |
| `FLAT-FOOT!` ❌ | < +5° | Foot landed flat or toe-first on a forward step | Common cause: rushing the step, tense ankles, or insufficient hip extension |
| `HEEL SPIKE` ⚠️ | > +35° | Extremely steep heel angle | Usually a fast, aggressive step; reduce drive force or relax the ankle |

#### Backward step badges

| Badge | θ | What you did | What to aim for |
| :--- | :--- | :--- | :--- |
| `OPTIMAL TOE` ✅ | −20° to +5° | Clean toe-ball landing — foot rolled onto the floor from the toe | Target for all backward walks, anchors, and extensions |
| `ANCHOR SETTLE` ✅ | −2° to +5° AND full weight | Heel just kissed the floor with full weight investment | The ideal anchor completion — leverage is built here |
| `HEEL DROP` ⚠️ | +5° to +9° | Heel beginning to drop before full weight transfer | Keep the ankle dorsiflexed a moment longer |
| `HEEL LANDING!` ❌ | ≥ +10° | Heel struck first on a backward step | The most common WCS technique error — pulls partner off-axis and kills momentum |
| `HEEL SPIKE` ⚠️ | < −20° | Over-pointed foot at contact | Moderate the extension slightly |

#### Impact Jerk bar

- **Short bar, no click** → soft, controlled landing. Ideal for most steps.
- **Full red bar + 500 Hz click** → heavy stomping. Bend the knee as the foot meets the floor and absorb with the ankle rather than dropping the foot.

---

### 6.3 Reading the Push-Off & Loading Badges

These three badges appear below the Jerk bar and update together after each step.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `🚀 POWER PUSH` ✅ | Strong, biomechanically optimal push-off from your trailing foot | Maintain — you are actively driving forward |
| `↗ PUSH` ⚠️ | Push-off detected but below optimal force | Drive more actively through the ball of the trailing foot; think "push the floor away" |
| `— PUSH-OFF` | No significant push-off detected | Your trailing leg is passive. Actively extend the ankle at the end of each walk |
| `SMOOTH LOAD` ✅ | Weight transferred progressively onto the landing foot | Good joint mechanics — keep it |
| `INSTANT LOAD` ⚠️ | Full weight dropped onto the landing foot immediately at impact | Slow the COM down; think of "receiving" the floor rather than landing on it |
| `EARLY UNLOAD` ⚠️ | Weight already shifting away before the landing foot is settled | You are rushing to the next step. Complete the current weight investment before moving |
| `ANKLE FLEX` ✅ | Ankle rolling through impact (pronation impulse detected) | Good shock absorption — ankle acting as natural spring |
| `MODERATE ROLL` ⚠️ | Some ankle mobility, could be more | Consciously relax the ankle at landing; avoid bracing the foot rigid |
| `STIFF ANKLE` ⚠️ | Minimal ankle roll — impact transmitted directly up the chain | Focus on landing with a soft, unlocked ankle. Over time this reduces knee and hip load |

---

### 6.4 Reading the Double Stance Card

This card tells you how long both feet are on the floor at the same time during each weight transfer.

| Badge | Ratio | What it means | Training implication |
| :--- | :--- | :--- | :--- |
| `OPTIMAL ROLL` ✅ | 18%–38% | Smooth, grounded weight transfer with natural bilateral overlap | Maintain — this is the characteristic WCS rolling connection |
| `HECTIC` ⚠️ | < 18% | Rushed transfer — one foot leaves before the other is secure | Slow down; roll through the foot before lifting. Think "peel not lift" |
| `SLUGGISH` ⚠️ | > 38% | Prolonged double contact — hesitation or heavy stance | Commit to the weight transfer earlier; move the COM, not just the foot |

**Watch this card during triple steps and walks.** A consistent `OPTIMAL ROLL` across a full 8-count pattern means your weight transfer timing is on. `HECTIC` on the anchor step often means you are rushing out of the anchor before building the connection.

---

### 6.5 Reading the Symmetry & Smoothness Card

| Display | What it tells you | Green target |
| :--- | :--- | :--- |
| **ASI %** | How much difference there is between your left and right foot roll-off | `SYMMETRIC` — below 10% |
| **Smoothness** | How fluid your overall ankle articulation is across both feet | `SMOOTH` — 65 or above |

- A high **ASI** (e.g. `ASYMMETRIC` > 25%) often means one leg is doing most of the work or one ankle is stiffer. This is common when recovering from an old injury or when one foot's technique habit differs from the other.
- A low **Smoothness** score means your ankle movements are jerky or stuttering — not a continuous, fluid arc. Slow your tempo down and focus on rolling through the full foot rather than stepping flat.

---

### 6.6 What to Focus on — by Skill Level

#### Beginner (just starting out)
Ignore all other cards. Focus on one thing only: **Step Badge direction + badge**.

1. Walk forward. Does the badge say `OPTIMAL HEEL`? If not, lift your heel slightly more before the step lands.
2. Walk backward. Does the badge say `OPTIMAL TOE` or `ANCHOR SETTLE`? If not, send your toe out first like a probe — before the body weight follows.
3. If you hear the **1200 Hz beep**, stop and slow down. That sound means `HEEL LANDING!` or hard `FLAT-FOOT!`.

#### Intermediate (technique refinement)
1. Forward walks → aim for consistent `OPTIMAL HEEL` with Jerk bar under 50%.
2. Backward walks → aim for `OPTIMAL TOE` → `ANCHOR SETTLE` sequence on anchor steps.
3. Introduce the **Double Stance card**: work toward `OPTIMAL ROLL` during triples.
4. Watch the **POWER PUSH badge**: is your trailing leg passive? Drive through the ball of the foot on every walk.

#### Advanced (biomechanical optimisation)
1. Use **SMOOTH LOAD vs INSTANT LOAD** to fine-tune how you receive weight — especially on syncopated patterns where COM timing matters.
2. Compare **ASI** between your left and right sides. If one side is consistently worse, isolate that foot with single-foot drills.
3. Use **ANKLE FLEX vs STIFF ANKLE** to monitor fatigue — stiffness increases when ankles tire. This is a natural cue to slow down or rest.
4. Film yourself with `📷 CAM` and replay during pauses. Look for the frame where `HEEL LANDING!` fires — your posture in that moment reveals the root cause (usually upper body tension or early COM shift).

---

### 6.7 Common Problems and How to Fix Them

| What you see | Root cause | Fix |
| :--- | :--- | :--- |
| `FLAT-FOOT!` on every forward walk | Ankle held rigid; no heel articulation | Slow down. Exaggerate the heel-first contact consciously. Drill: walk in place, touching heel then ball in sequence. |
| `HEEL LANDING!` on backward steps | Body weight moving backward too fast before foot scouts | Delay the COM — send the foot first, body follows. Drill: backward walks holding a wall for balance, exaggerating toe-first contact. |
| `HECTIC` Double Stance | Rushing the transfer; foot lifts too early | Think "leave the floor last" — let the whole foot peel up from toe. |
| `SLUGGISH` Double Stance | Hesitating before committing weight | Trust the floor. Move the body, not just the foot. Drill: metronome walks, landing on the beat. |
| `STIFF ANKLE` consistently | Braced ankle at landing | Visualise landing on a sponge. Consciously unlock the ankle joint before contact. |
| `ASYMMETRIC` ASI | One foot stiffer or less articulated | Identify which foot (left or right from the badge) and drill that foot in isolation with slow deliberate rolls. |
| `— PUSH-OFF` (no badge) | Trailing leg passive — no drive | Think "push the floor, don't just lift the foot". Add a conscious toe-extension at the end of each walk. |
| `INSTANT LOAD` on anchors | Dropping weight abruptly at anchor | Slow the settle. Think "melt into the anchor" rather than "land on it". |

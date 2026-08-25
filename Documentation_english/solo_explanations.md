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
 +------------------------+   +-+------------------------------+
 | Pelvis Sensor (opt.)   |-->|  Central Master Unit (Gateway) |
 | ID: 4 | M5Stick @ 200Hz|   |  M5Stick S3 Web Server         |
 +------------------------+   +----------------+---------------+
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
* **Pelvis Node (ID 4, optional):** Same hardware worn on a belt at the sacrum. Streams 3-axis accelerometer (`pA`, `pAy`, `pAx`) and yaw gyroscope (`pYaw`, `pG`) at 200 Hz. Enables hip activation, lateral stability, hip–foot coupling, vertical bounce, pelvic tilt, Anchor Settle, and Hip Settle analysis. When absent, the dashboard operates normally without pelvis cards.
* **Central Master Unit:** Aggregates ESP-NOW streams and delivers JSON data packets (`lG`, `lA`, `lAy`, `lGr`, `rG`, `rA`, `rAy`, `rGr`; optionally `pA`, `pAy`, `pAx`, `pYaw`, `pG`, `pOk`) to the browser via the `/data` endpoint.
* **Web Dashboard (`/solo`):** Client-side JavaScript executes state machine filtering, direction mapping, pitch integration, stance timeline calculations, and Web Audio API feedback.

> 📸 **[Screenshot: Solo Dashboard showing all four sensor nodes connected and live telemetry streaming to the browser]**

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

**The three rockers (Perry & Burnfield model):**

| Rocker | Stance Phase | Pivot | Muscular Control |
| :--- | :--- | :--- | :--- |
| **Heel Rocker** (1st) | Initial Contact → Loading Response | Heel | Eccentric tibialis anterior — lowers foot to floor |
| **Ankle Rocker** (2nd) | Loading Response → Midstance | Ankle joint | Eccentric gastrocnemius/soleus — controls tibial advance |
| **Forefoot Rocker** (3rd) | Terminal Stance → Push-off | Metatarsal heads | Windlass mechanism + concentric plantarflexors |

In WCS **forward steps** all three rockers are present: heel strike → ankle advance → forefoot roll-off. **Backward steps** engage only the forefoot and ankle rockers in reverse (toe-first contact → heel settle). The θ angle at T-1 captures the foot's position at the transition between rocker phases; jerk quantifies how abruptly each transition is executed.

> **Deeper reading:** Perry, J. & Burnfield, J.M. (2010). *Gait Analysis: Normal and Pathological Function* (2nd ed.). SLACK Inc. — the standard clinical reference for all gait phase terminology used here. Freely accessible overview: [Wikipedia — Gait analysis](https://en.wikipedia.org/wiki/Gait_analysis).

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

   **T-1 Snapshot for step classification:** At the moment of impact, the angle from the *previous frame* (T-1) is used — not the instantaneous value. The aZ > 0.95–0.97 g trigger (tempo-adaptive) fires after partial weight loading when roll-through has already begun; the T-1 frame captures pre-contact foot orientation before distortion.

2. **Zero-Tare Compensation ($\theta_{\text{calibrated}}$):**
   To adjust for individual instep shoe slopes, the `📐 ZERO` button captures static mounting offsets ($\text{leftMountOffset}$, $\text{rightMountOffset}$):
   $$\theta = \theta_{\text{raw}} - \text{mountOffset}$$

3. **Direction Display & Landing Quality Badge:**

   **Validated limitation:** Empirical testing (n=15 backward flat-walk steps, pelvis sensor included) confirmed that WCS flat-walk backward steps land consistently at θ = +2° to +9° — the same range as ambiguous or flat forward steps. Neither the dθ pitch trend nor pelvis sagittal acceleration (average directional difference < 0.03 g across all axes) can reliably distinguish backward from forward within this range. The direction badge is therefore shown only where θ gives unambiguous physical evidence:

   | θ at T-1 | Direction badge |
   | :---: | :--- |
   | θ ≥ +8° | ➡️ FORWARD (blue) — reliable heel-first contact |
   | θ < −8° | ⬅️ BACK (purple) — reliable toe-first contact |
   | −8° ≤ θ < +8° | — (grey) — ambiguous; direction not shown |

   **Internal direction classification (Anchor Settle trigger):** Distinct from the badge display, the internal `activeDir` logic uses a narrower ambiguous zone: only `0° < θ < +10°` requires a pitch-angle trend check. For `θ ≤ 0°` (any plantarflexion / toe-first contact), `activeDir` is set to BACKWARD directly without trend analysis. This ensures backward steps landing at θ = -2° to -5° correctly trigger the Anchor Settle evaluation window even though the direction badge still shows "—" (because |θ| < 8°).

   **Landing quality badge — direction-agnostic, based on θ zone + jerk:**

   The strike badge evaluates *how* the foot landed, independent of direction. This is useful for both forward and backward steps: `SOFT ✓` at θ ≈ 0° indicates a controlled backward flat step; `HARD IMPACT ⚠` at θ ≈ 0° means the dancer fell onto the foot.

   Jerk thresholds (same scaling as the Impact Jerk display value ÷ 4):
   - **HARD:** J > 22 g/s (internal > 88)
   - **MODERATE:** 20 g/s < J ≤ 22 g/s (internal 80–88)
   - **SOFT:** J ≤ 20 g/s (internal ≤ 80)

   | θ zone | Jerk | Badge | Meaning |
   | :---: | :---: | :--- | :--- |
   | ≥ +8° (heel) | ≤ 22 g/s | `HEEL STRIKE ✓` (Green) | Clean heel-first landing — correct forward technique |
   | ≥ +8° (heel) | > 22 g/s | `HEEL SLAM ⚠` (Red) | Heel contact but impact too abrupt — absorb with knee/ankle |
   | < −8° (toe) | ≤ 22 g/s | `TOE-FIRST ✓` (Green) | Controlled toe-first landing — correct for deep backward steps or ball-steps |
   | < −8° (toe) | > 22 g/s | `TOE JAM ⚠` (Red) | Toe contact too hard |
   | −8° to +7° (ambiguous) | ≤ 20 g/s | `SOFT ✓` (Green) | Controlled landing — good quality regardless of direction |
   | −8° to +7° (ambiguous) | 20–22 g/s | `MODERATE` (Yellow) | Acceptable; reduce impact |
   | −8° to +7° (ambiguous) | > 22 g/s | `HARD IMPACT ⚠` (Red) | Fell onto foot — triggers 1200 Hz click |

   * **BRUSH+HEEL reclassification (200 ms window):** if a landing in the ambiguous zone is followed within 200 ms by a second aZ > 1.05 g peak with accelAngle > 8° on the same foot, the badge upgrades to `BRUSH+HEEL` (green) and the direction badge shows ➡️ FORWARD.

---

### C. Terminal Stance & Power Push Propulsion
West Coast Swing propulsion requires an active toe push-off (*Windlass Mechanism*) from the trailing leg at the end of the stance phase. The optimal plantarflexion angular velocity depends on movement direction — forward propulsion demands more drive than the subtler redistribution at an anchor or backward walk.

> **The Windlass Mechanism** (Hicks, 1954): As the toes dorsiflex at push-off, the plantar fascia — which wraps under the metatarsal heads — tightens like a rope on a windlass drum. This elevates the medial arch and stiffens the foot into a rigid lever, converting it from a flexible shock absorber into an efficient propulsive structure. In WCS, a `🚀 POWER PUSH` badge confirms the mechanism has loaded: sufficient $-\omega_\text{pitch}$ angular velocity indicates the forefoot rocker has completed and toe-off has generated real propulsive force.
>
> **Reference:** Hicks, J.H. (1954). The mechanics of the foot. *Journal of Anatomy*, 87(4), 345–357. Free overview: [Wikipedia — Windlass mechanism of the foot](https://en.wikipedia.org/wiki/Windlass_mechanism_of_the_foot).

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

> **Tempo-adaptive scaling:** All peak thresholds (120/160/200 °/s at reference tempo) scale with step interval: `scaleFactor = 500 / max(400, stepDurationMs)`. At slow tempo (700 ms/step, scaleFactor ≈ 0.71) the thresholds drop to ≈86/114/142 °/s; at fast tempo (400 ms/step, scaleFactor = 1.25) they rise to ≈150/200/250 °/s. The integral thresholds (12°/16°/20°) represent total angular displacement and are tempo-independent.

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
West Coast Swing emphasizes a continuous, grounded "rolling" weight transfer rather than abrupt hopping or lifting off the floor prematurely. Ground contact is detected via a **hysteresis algorithm**: a foot transitions to "on ground" (●) when $|aZ| > 0.65\,g$ and back to "airborne" (○) when $|aZ| <$ exitAZ **OR** $|\omega_{\text{pitch}}| > 80\,°/\text{s}$ **OR** $|\omega_{\text{roll}}| > 80\,°/\text{s}$, after a minimum contact time has elapsed.

> **Signal note:** The $|aZ|$ threshold is a sensor heuristic for ground reaction force — not a direct force measurement. Dynamic foot rotations can shift $aZ$ independently of actual floor contact. The gyro-exit guard (80 °/s) prevents premature ○ during normal push-off roll, which typically reaches 60–75 °/s. The thresholds are calibrated empirically.

> **Implementation detail — hysteresis parameters:**
> * **Entry:** $|aZ| > 0.65\,g$ → foot marked ● (landed); `landedAt` timer starts
> * **Exit threshold (exitAZ):** $0.48\,g$ when opposing foot $|aZ| > 0.75\,g$ (bearing load), else $0.45\,g$
> * **Minimum contact time (minGnd):** $\text{clamp}(t_{\text{step}} \times 0.40,\;150\,\text{ms},\;300\,\text{ms})$ — exit condition is gated until this time elapses after landing
> * **Maximum contact time (Timed-Exit):** $\text{clamp}(t_{\text{step}} \times 0.75,\;350\,\text{ms},\;600\,\text{ms})$ — foot forced to ○ after this duration regardless of $aZ$
> * **Step interval smoothing (EMA):** $t_{\text{step}} = 0.45 \times t_{\text{step,prev}} + 0.55 \times t_{\text{step,current}}$ — fast-converging EMA (α = 0.55) prevents a single anomalous interval from distorting the DS% denominator

$$\text{Stance Ratio} = \left( \frac{\Delta t_{\text{double-stance}}}{t_{\text{step}}} \right) \times 100\%$$

#### Why Overlap Matters in WCS Mechanics:
* **Grounded Rolling Action:** In West Coast Swing, weight transfer is gradual. As one foot leaves the floor, the other receives weight, creating a natural bilateral overlap phase where both soles register ground contact ($|aZ| > 0.65\,g$ entry threshold).
* **Elastic Extension & Timing:** A healthy overlap ratio ($15\%\text{ to }52\%$) creates the characteristic "elastic" stretch and smooth momentum transfer in WCS. Too little overlap indicates rushing or bouncing, while too much overlap results in heavy, sluggish transitions.
* **Note on scientific literature:** Classic gait analysis (Perry & Burnfield, 2010 — cited in §2A; Winter, D.A., 1990: *Biomechanics and Motor Control of Human Gait*, University of Waterloo Press) establishes stance phase at ~60% and swing phase at ~40% of the gait cycle at comfortable walking speed. This is a different measurement — it describes how long *one* foot stays on the ground during a single gait cycle. The metric here measures the *simultaneous bilateral contact* ratio (both feet on the floor at the same time within one step interval), which is a subset of and distinctly different from the single-foot stance phase. Free overview of gait cycle phases: [Wikipedia — Gait#Phases of gait](https://en.wikipedia.org/wiki/Gait#Phases_of_gait).

| Ratio Range (%) | Badge Rating | Biomechanical Meaning |
| :---: | :---: | :--- |
| **15% to 52%** | `OPTIMAL ROLL` | Ideal grounded roll-off phase for walks and extensions. |
| **< 15%** | `HECTIC` | Rushed weight transfer; lack of rolling articulation through the foot. |
| **> 52%** | `SLUGGISH` | Over-invested ground contact; sluggish tempo transition. |

---

### F. Roll-off Symmetry Index (ASI) & Smoothness Index

1. **Asymmetry Index (ASI):**
   Compares total angular work integrated across Left and Right foot roll-off cycles while feet are actively moving ($|\omega_{\text{pitch}}| > 15^\circ/\text{s}$):
   $$\text{ASI} = \frac{2 \cdot \left|\int|\omega_{\text{left}}|\,dt - \int|\omega_{\text{right}}|\,dt\right|}{\int|\omega_{\text{left}}|\,dt + \int|\omega_{\text{right}}|\,dt} \times 100\%$$
   * **Target:** $< 15\%$ (`SYMMETRIC`), $16\text{--}35\%$ (`MINOR ASYM`), $>35\%$ (`ASYMMETRIC`).

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

> **Measurement note:** `gRoll` measures rotation of the *shoe segment* around the sensor's roll axis, not directly the subtalar joint eversion angle. `rollIntegral` is a foot-rotation proxy for pronatory shock absorption; the reversal check is a proxy for the pronation→supination cycle that pre-loads the Windlass Mechanism. Both serve as training indicators, not anatomical joint measurements.

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
| **Heel zone — clean** | θ ≥ +8°, Jerk ≤ 22 g/s | `HEEL STRIKE ✓` (Green) | None |
| **Heel zone — slam** | θ ≥ +8°, Jerk > 22 g/s | `HEEL SLAM ⚠` (Red) | 1200 Hz Click |
| **Toe zone — clean** | θ < −8°, Jerk ≤ 22 g/s | `TOE-FIRST ✓` (Green) | None |
| **Toe zone — jam** | θ < −8°, Jerk > 22 g/s | `TOE JAM ⚠` (Red) | 1200 Hz Click |
| **Ambiguous zone — soft** | −8° ≤ θ < +8°, Jerk ≤ 20 g/s | `SOFT ✓` (Green) | None |
| **Ambiguous zone — moderate** | −8° ≤ θ < +8°, 20 < Jerk ≤ 22 g/s | `MODERATE` (Yellow) | None |
| **Ambiguous zone — hard** | −8° ≤ θ < +8°, Jerk > 22 g/s | `HARD IMPACT ⚠` (Red) | 1200 Hz Click |
| **Brush+Heel reclassification** | Ambiguous landing + second aZ > 1.05g, accelAngle > 8° within 200 ms | `BRUSH+HEEL` (Green) — upgrades from any ambiguous badge | None |
| **Trailing Foot Push-off (forward, optimal)**| BACKWARD last step + $-\omega_{\text{pitch}} \ge 200^\circ/\text{s}$ AND $aY > 0.15g$ | `🚀 POWER PUSH` (Green) — real-time, holds 400 ms | None |
| **Trailing Foot Push-off (backward/anchor, optimal)**| FORWARD last step + $-\omega_{\text{pitch}} \ge 160^\circ/\text{s}$ AND $aY > 0.15g$ | `🚀 POWER PUSH` (Green) — real-time, holds 400 ms | None |
| **Trailing Foot Push-off (weak)** | Either direction, $120\text{–}159/199^\circ/\text{s}$ AND $aY > 0.15g$ | `↗ PUSH` (Yellow) — real-time, holds 400 ms | None |
| **Impact Jerk ($J_{\text{impact}}$)** | $> 30\text{ g/s}$ | Flash Card Boundary | 500 Hz Low Impact Click (80 ms) |
| **Double Stance Ratio** | 15% to 52% | `OPTIMAL ROLL` (Green) | None |
| **Double Stance Hectic** | $< 15\%$ | `HECTIC` (Yellow) | None |
| **Double Stance Sluggish**| $> 52\%$ | `SLUGGISH` (Yellow) | None |
| **Weight Transfer — Progressive** | loadRise $> 0.12\,g$ | `SMOOTH LOAD` (Green) | None |
| **Weight Transfer — Instant** | $-0.08 \le$ loadRise $\le 0.12$ | `INSTANT LOAD` (Yellow) | None |
| **Weight Transfer — Early Unload** | loadRise $< -0.08\,g$ | `EARLY UNLOAD` (Yellow) | None |
| **Delay Ramp — FWD delayed** | ratio 0.12–0.38 (FWD) | `DELAYED ✓` (Green) | None |
| **Delay Ramp — FWD quick** | ratio $< 0.12$ (FWD) | `QUICK` (Yellow) | None |
| **Delay Ramp — FWD late** | ratio $> 0.38$ (FWD) | `LATE` (Yellow) | ADV only |
| **Delay Ramp — BWD delayed** | ratio 0.18–0.50 (BWD) | `DELAYED ✓` (Green) | None |
| **Delay Ramp — BWD quick** | ratio $< 0.18$ (BWD) | `QUICK` (Yellow) | None |
| **Delay Ramp — BWD late** | ratio $> 0.50$ (BWD) | `LATE` (Yellow) | ADV only |
| **Rigid Lever** | pronation $> 8°/\text{s}$ AND sign reversal in 200 ms | `RIGID LEVER` (Green) | None |
| **Ankle Shock Absorption** | rollIntegral $> 4°$ (no reversal) | `ANKLE FLEX` (Green) | None |
| **Ankle Stiffness** | rollIntegral $< 1°$ | `STIFF ANKLE` (Yellow) | None |
| **Hitch & Go — detected** | $|aZ| < 0.35\,g$ for 50–380 ms within 700 ms of last step | `✓ HITCH (L/R)` (Green) | None |
| **Ball→Heel — optimal** | $\theta_{T-1} < -2°$, lateMean $> 3°$ | `BALL→HEEL ✓` (Green) | None |
| **Ball→Heel — partial** | $\theta_{T-1} < 0°$, lateMean $> 0°$ | `PARTIAL ROLL` (Yellow) | None |
| **Ball→Heel — heel-first** | $\theta_{T-1} \ge 0°$ | `HEEL-FIRST` (Yellow) | None |
| **Ball→Heel — ball only** | $\theta_{T-1} < 0°$, lateMean $\le 0°$ | `BALL ONLY ⚠` (Red) | None |
| **Heel→Ball — optimal** | earlyMean $> 2°$, drop $> 3°$ | `HEEL→BALL ✓` (Green) | None |
| **Heel→Ball — partial** | earlyMean $> 0°$, drop $> 1°$ | `PARTIAL ROLL` (Yellow) | None |
| **Heel→Ball — stuck** | earlyMean $> 2°$, drop $\le 1°$ | `HEEL STUCK ⚠` (Red) | None |
| **Toe→Heel — optimal (ball-lead)** | earlyMean $< -2°$, rise $> 3°$ | `TOE→HEEL ✓` (Green) | None |
| **Toe→Heel — partial (ball-lead)** | earlyMean $< 0°$, rise $> 1°$ | `PARTIAL ROLL` (Yellow) | None |
| **Heel→Ball — flat-foot** | earlyMean $\le 0°$, rise $\le 1°$ | `FLAT-FOOT` (Yellow) | None |
| **Per-Foot Lockout Window**| $180\text{–}320\text{ ms}$ (cadence-adaptive) + Alternation Guard | Suppresses same-foot re-trigger | None |

---

### H. Hitch & Go Detection

A *hitch* is a brief, voluntary foot-lift on the recently-placed foot — a jazz/blues accent used to syncopate the phrasing. The sensor detects it as a transient loss of ground contact on the active foot within 80–700 ms of the last step on that foot.

**Detection logic:**

| State | Condition | Transition |
| :--- | :--- | :--- |
| `ground` → `lifted` | $|aZ| < 0.35\,g$ AND $80\,\text{ms} < t_{\text{since step}} < 700\,\text{ms}$ | Start lift timer |
| `lifted` → `ground` (valid) | $|aZ| \ge 0.55\,g$ AND $50\,\text{ms} \le t_{\text{lift}} \le 380\,\text{ms}$ | `✓ HITCH (L/R)` (Green) |
| `lifted` → `ground` (too long) | $t_{\text{lift}} > 380\,\text{ms}$ | Reset silently — weight shift, not a hitch |
| Sensor offline | — | State reset to `ground` |

The 0.35g lift threshold lies clearly below the normal loaded-foot aZ of ~1.0g while remaining above the electrical noise floor. The 0.55g return threshold provides hysteresis at re-contact (note: the DS ground-contact entry threshold is 0.65g; this 0.55g is specific to hitch re-contact detection).

**Note:** Hitch detection is not level-gated — the badge appears in the Last Step card at all training levels.

---

### I. Ball-to-Heel Anchor Progression

During a well-executed backward anchor, the foot initially contacts on the ball (negative θ — plantarflexion) and then lowers to the heel as bodyweight settles. The sensor quantifies this progression by tracking foot pitch angle θ through a tempo-adaptive window after every backward step.

**Window:** `anchorWindowMs = clamp(stepDurationMs × 1.05, 280 ms, 900 ms)` — tempo-adaptive, independent of the pelvis Anchor Settle metric (which uses a fixed 500 ms gap after the last backward step).

**Calculation:**

At the moment of the backward step trigger, the foot angle from T-1 (pre-reset) is captured as $\theta_{T-1}$. The CF angle is then reset to 0° to prevent impact corruption. The second half of the post-reset window samples gives $\theta_{\text{late}}$, representing where the foot settled:

$$\theta_{T-1} = \text{foot angle at trigger, pre-reset (T-1 snapshot)}$$

$$\theta_{\text{late}} = \overline{\theta}_{[\lfloor n/2 \rfloor,\,n]} \quad \text{(second half of 280–500 ms post-reset window)}$$

| Condition | Badge | Biomechanical Meaning |
| :---: | :---: | :--- |
| $\theta_{T-1} \ge 0°$ | `HEEL-FIRST` (Yellow) | Foot arrived flat or heel-first — no initial ball contact |
| $\theta_{T-1} < -2°$ AND $\theta_{\text{late}} > 3°$ | `BALL→HEEL ✓` (Green) | Classic WCS landing: ball-first then heel lowers — controlled eccentric loading |
| $\theta_{T-1} < 0°$ AND $\theta_{\text{late}} > 0°$ | `PARTIAL ROLL` (Yellow) | Ball contact present, heel lowers partially into the post-reset window |
| Otherwise | `BALL ONLY ⚠` (Red) | θ stayed negative throughout — foot remained on ball with no heel lowering; typical of rushing or incomplete weight transfer |

**Minimum samples:** The evaluation requires at least 6 samples in the window (~120 ms). If a new step fires before the window closes, `anchorThetaActive` is reset and no badge is issued.

---

### J. Heel-to-Ball / Toe-to-Heel Forward Progression

The forward-step roll metric detects the sagittal ankle articulation pattern after every forward step. Two valid patterns exist in WCS — heel-lead (dorsiflexion at contact, θ > 0°) and ball-lead (plantarflexion at contact, θ < 0°) — and the badge identifies which occurred and whether the roll-through completed correctly.

**Window:** `heelBallWindowMs = clamp(stepDurationMs, 280 ms, 500 ms)`.

**Calculation:**

$$\theta_{\text{early}} = \overline{\theta}_{[0,\,\lfloor n/2 \rfloor]} \qquad \theta_{\text{late}} = \overline{\theta}_{[\lfloor n/2 \rfloor,\,n]}$$

$$\text{drop} = \theta_{\text{early}} - \theta_{\text{late}} \quad \text{(positive = θ fell = heel rolling to ball)}$$
$$\text{rise} = \theta_{\text{late}} - \theta_{\text{early}} \quad \text{(positive = θ rose = ball rolling to heel)}$$

| Condition | Badge | Biomechanical Meaning |
| :---: | :---: | :--- |
| **Heel-lead:** $\theta_{\text{early}} > 2°$ AND drop $> 3°$ | `HEEL→BALL ✓` (Green) | Heel contacts first, ankle rolls forward as weight transfers — correct heel-lead WCS walk |
| **Heel-lead:** $\theta_{\text{early}} > 0°$ AND drop $> 1°$ | `PARTIAL ROLL` (Yellow) | Some forward roll but incomplete — weight transferred before ankle fully unrolled |
| **Heel-lead:** $\theta_{\text{early}} > 2°$ AND drop $\le 1°$ | `HEEL STUCK ⚠` (Red) | Heel contacted but foot stayed dorsiflexed — ankle blocked, no roll-through |
| **Ball-lead:** $\theta_{\text{early}} < -2°$ AND rise $> 3°$ | `TOE→HEEL ✓` (Green) | Ball/toe contacts first, heel lowers as weight settles — correct ball-lead (triple step, coaster, tap variation) |
| **Ball-lead:** $\theta_{\text{early}} < 0°$ AND rise $> 1°$ | `PARTIAL ROLL` (Yellow) | Ball contact present but heel lowering incomplete |
| Otherwise | `FLAT-FOOT` (Yellow) | No clear contact pattern or roll — flat/ambiguous landing |

**Minimum samples:** At least 6 samples required (~120 ms). A new step resets `heelBallActive`.

---

## 4. Signal Filtering & Lockout Concept

To prevent false secondary step triggers caused by micro-taps, foot unweighting, or floor vibrations, the DSP pipeline executes a **Triple-Stage Filtering & Lockout Concept**:

1. **Transient Signal Candidate Sensing:**
   Each foot independently qualifies as an impact candidate via OR-logic:
   $$\text{signal}_{\text{foot}} = \bigl(|aZ| > \theta_{\text{thr}} \;\mathbf{AND}\; \text{preJerk} > 2\bigr) \;\mathbf{OR}\; \bigl(|\omega_{\text{pitch}}| > 80\,\text{deg/s} \;\mathbf{AND}\; \text{preJerk} > 8\bigr)$$
   where $\theta_{\text{thr}} = 0.95\,g$ when $t_{\text{step}} > 800\,\text{ms}$ (slow practice, $< 75\,\text{BPM}$), and $0.97\,g$ otherwise. The `preJerk > 2` gate on the aZ path suppresses gradual stance-foot weight drift (typical preJerk 0.5–2) while passing real foot impacts (preJerk typically 5–30+). This tempo-adaptive threshold captures soft deliberate weight transfers in slow footwork training while suppressing brush artefacts at dance tempo.
   The `preJerk` gate (`|aZ_t - aZ_{t-1}| / Δt > 8`) on the gyro path suppresses liftoff rotation artefacts that would otherwise ghost as step triggers. When both feet signal in the same frame, the dominant foot is selected by peak ground reaction force: $\text{detectedFoot} = \arg\max(|aZ_L|, |aZ_R|)$.

   > **Note:** The gyro path correctly resolves flat ball-of-foot contacts (aZ below threshold) via `|gPitch| > 80°/s`. Observed preJerk minimum in real WCS steps: **6.0** — well above the 2.0 gate. Step balance in practice: L/R counts remain equal across single steps and triple steps.

2. **Opposing-Foot Plausibility Check (Phantom Trigger Suppression):**
   After candidate selection, the trigger is discarded if the detected foot shows $|aZ| < 0.90\,g$ while the opposing foot shows $|aZ| > 1.1\,g$ (clearly the loaded stance leg). This eliminates swing-phase artefacts — brush, scuff, or abrupt liftoff events that generate a high jerk signal without real weight transfer. Without this check, a scuff during the swing phase can produce a false `HARD IMPACT ⚠` badge despite zero ground loading (confirmed in analysis: 48 g/s phantom trigger at second 18 with stance foot bearing full load).

3. **Per-Foot Cadence-Adaptive Lockout & Alternation Guard:**
   * **The Lockout Concept:** The system maintains independent last-step timestamps for each leg (`lastStepTimeLeft` and `lastStepTimeRight`). Whenever a candidate step is detected for a leg, the state machine checks if the time elapsed since the previous step *on that specific leg* is less than the dynamic lockout window.
   * **Cadence-Adaptive Window:** The lockout scales with the current step period: $t_{\text{lockout}} = \text{clamp}(t_{\text{step}} \times 0.55,\ 180\text{ ms},\ 320\text{ ms})$. At 120 BPM ($t_{\text{step}} = 500\text{ ms}$) this yields 275 ms; at 160 BPM (375 ms) → 206 ms; at 200 BPM (300 ms) → 180 ms (floor). This prevents both ghost triggers at slow tempos and missed steps at high tempos.
   * **Alternation Guard:** Steps must alternate (`Left -> Right -> Left`). If the same foot fires twice without the opposite foot making contact in between, it is discarded as a liftoff re-detection or vibration ghost.
   * **Global 130 ms Cross-Foot Lockout:** Any step trigger — regardless of which foot — is rejected if it arrives within 130 ms of the last confirmed step. This cross-foot guard catches the case where the non-stepping foot oscillates near 0.95–0.97 g shortly after a real step: the per-foot lockout on the opposite foot is stale (its last-step time is old) and would not block it. `lastStepTimestamp` is updated on every confirmed step and shared across both feet.

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

> 📸 **[Screenshot: Solo Dashboard with camera HUD active showing semi-transparent data cards overlaying the live body view in landscape mode]**

---

## 6. Pelvis Metrics (Solo Dashboard)

When the pelvis sensor (ID 4) is connected, the Solo Dashboard displays six additional badge cards derived from pelvic kinematics. The sensor is worn on a belt at the sacrum and streams 3-axis accelerometer (`pAx`, `pAy`, `pAz`) and yaw gyroscope (`pYaw`) data at 200 Hz.

### gYaw Curve in the Live Roll-off Dynamics Graph

A **dashed yellow line** in the Live Roll-off Dynamics graph shows the measured hip yaw rate (`pYaw` from the pelvis sensor), scaled to the same display range as the foot pitch curves. This curve is visible whenever the pelvis sensor is connected and provides a real-time reference for how hip rotation timing aligns with foot roll-off events.

> 📸 **[Screenshot: Roll-off Dynamics graph showing left (cyan) and right (magenta) foot pitch curves with the dashed yellow pelvis yaw rate curve overlaid]**

### Hip Activation

Measures the peak rotational speed of the pelvis around the vertical axis — a proxy for active hip engagement during each step.

**Signal processing:**
- Sliding buffer: `gYawAbsHistory` — 25 samples (~0.5 s window) of absolute `pYaw` values
- Peak extraction: `gYawPeak = max(gYawAbsHistory)`
- Exponential smoothing: `hipActSmoothed = hipActSmoothed × 0.9 + gYawPeak × 0.1` (τ ≈ 2 s)

**Thresholds (tempo-adaptive):**

$$\text{scaleFactor} = \frac{500}{\max(400,\; \text{stepDurationMs})}$$

| State | Threshold | Colour |
| :--- | :--- | :--- |
| ACTIVE | `hipActSmoothed ≥ round(60 × scaleFactor)` °/s | Green |
| MODERATE | `hipActSmoothed ≥ round(25 × scaleFactor)` °/s | Yellow |
| STIFF HIPS | below MODERATE threshold | Red |

Reference values at 500 ms/step (scaleFactor = 1.0): ACTIVE ≥ 60 °/s, MODERATE ≥ 25 °/s. At slow tempo (700 ms/step, scaleFactor ≈ 0.71): ACTIVE ≥ 43 °/s, MODERATE ≥ 18 °/s. At fast tempo (400 ms/step, scaleFactor = 1.25): ACTIVE ≥ 75 °/s, MODERATE ≥ 31 °/s.

### Lateral Stability

Monitors pelvis sway in the mediolateral (side-to-side) direction.

- Buffer: `aXPHistory` — 50 samples (~1 s) of lateral acceleration `pAx`
- Metric: `aXVar = variance(aXPHistory)`

| State | Condition | Colour |
| :--- | :--- | :--- |
| STABLE | `aXVar < 0.004` | Green |
| SLIGHT SWAY | `aXVar < 0.015` | Yellow |
| LATERAL SWAY | `aXVar ≥ 0.015` | Red |

### Hip-Foot Coupling

Assesses whether the pelvis rotation initiates before foot contact (the WCS ideal) or lags behind.

- Buffer: `gYawTimedBuf` — ring buffer of `{value, timestamp}` pairs for the last 600 ms
- Trigger: at each confirmed step, find the timestamp of peak `pYaw` magnitude in the buffer
- Lead time: `leadMs = stepTimestamp − peakTimestamp`

| State | Condition | Colour |
| :--- | :--- | :--- |
| HIP LEADS | `leadMs > 100 ms` | Green |
| IN SYNC | `leadMs > 40 ms` | Yellow |
| HIP LAGS | `leadMs ≤ 40 ms` | Red |

### Vertical Bounce

Detects excessive vertical oscillation of the pelvis — a sign of bouncy or heel-heavy movement rather than the grounded, level carriage characteristic of good WCS.

- Buffer: `aZPDynHistory` — 50 samples (~1 s) of `(pAz − 1.0)` (gravity-subtracted vertical acceleration)
- Metric: `aZVar = variance(aZPDynHistory)`

| State | Condition | Colour |
| :--- | :--- | :--- |
| GROUNDED | `aZVar < 0.006` | Green |
| SLIGHT BOUNCE | `aZVar < 0.020` | Yellow |
| BOUNCY | `aZVar ≥ 0.020` | Red |

### Pelvic Tilt

Detects anterior pelvic tilt (Hohlkreuz / lordosis) as a continuous postural metric. The sagittal tilt angle is derived from the pelvis accelerometer axes `pAy` (sagittal) and `pA` (vertical) — the same approach used for foot pitch, but for the pelvis:

$$\theta_{\text{pelvis}} = \arctan2(aY_P,\; aZ_P) \cdot \frac{180°}{\pi} - \theta_{\text{offset}}$$

The offset is captured at `📐 ZERO` press (dancer stands in neutral dance position). The raw angle is IIR-smoothed (α = 0.08, τ ≈ 250 ms) to suppress step-impact vibration.

| State | Condition | Colour | Biomechanical Meaning |
| :--- | :--- | :--- | :--- |
| ALIGNED | $|\theta_{\text{pelvis}}| \le 6°$ | Green | Neutral pelvis — lumbar spine protected, core engaged |
| SLIGHT ARCH | $6° < \theta_{\text{pelvis}} \le 12°$ | Yellow | Mild anterior tilt — connection still functional but core tension reduced |
| LORDOSIS ⚠ | $\theta_{\text{pelvis}} > 12°$ | Red | Significant hyperextension — partner connection disrupted, injury risk |
| TUCKED | $\theta_{\text{pelvis}} < -6°$ | Yellow | Excessive posterior tilt — over-correction reduces hip mobility and swing |

**Calibration note:** `📐 ZERO` must be pressed in neutral dance stance (upright, weight balanced). The pelvis offset is zeroed simultaneously with the foot offsets.

### Anchor Settle

Evaluates the quality of deceleration and pelvis settling at the end of each backward step — the defining moment where WCS stretch converts into grounded, controlled weight transfer.

**Trigger:** The first confirmed BACKWARD step in a backward sequence starts fresh sample collection. Each additional backward step (e.g. all three steps of an anchor triple at beats 5–&–6) extends the evaluation deadline without clearing the accumulated samples. Evaluation fires 500 ms after the **last** backward step:

$$t_{\text{eval}} = t_{\text{last BACKWARD step}} + 500\,\text{ms}$$

This ensures the full anchor triple is captured before scoring, while evaluating promptly once backward movement ends.

Samples collected: sagittal pelvis acceleration `aYP` and hip yaw rate `gYawP`.

**Score composition (scaled 0–100):**

$$\text{score} = \text{decelScore} \times 0.35 + \text{yawDampScore} \times 0.35 + \text{stabilScore} \times 0.30$$

| Component | Formula | What it captures |
| :--- | :--- | :--- |
| **decelScore** | `(earlyAYMag − lateAYMag + 0.05) / 0.25`, clipped 0–1 | Sagittal braking: pelvis decelerates forward momentum |
| **yawDampScore** | `(earlyYawMean − lateYawMean) / 25`, clipped 0–1 | Yaw damping: hip rotation ceases after landing |
| **stabilScore** | `max(0, 1 − lateYawVariance / 400)` | Late stability: low yaw variance in second half of window |

| State | Condition | Colour |
| :--- | :--- | :--- |
| ANCHORED | score ≥ 60 | Green |
| SETTLING | score ≥ 30 | Yellow |
| UNSTABLE | score < 30 | Red |

> 📸 **[Screenshot: Pelvis card Anchor Settle badge showing a numeric score (e.g. ANCHORED 74) in green after a backward anchor step]**

### Hip Settle

Evaluated at the end of the Anchor Settle window. Uses lateral pelvis acceleration (`pAx`) samples from the first half of the window to detect the characteristic lateral weight-shift of a well-executed anchor.

- `earlyLatPeak` = max absolute `pAx` in the first half of the window
- `lateLatVar` = variance of `pAx` in the second half

| State | Condition | Colour |
| :--- | :--- | :--- |
| OVERSWING ⚠ | `earlyLatPeak > 0.30 g` | Yellow |
| HIP SETTLE ✓ | `earlyLatPeak > 0.10 g` AND `lateLatVar < 0.015` | Green |
| SLIGHT SETTLE | `earlyLatPeak > 0.05 g` | Yellow |
| NO HIP SETTLE | `earlyLatPeak ≤ 0.05 g` | Red |


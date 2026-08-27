# WCS Connection Dashboard — Technical Reference

This document covers the mathematical and biomechanical foundations of the **Connection Dashboard** (`/`) — connection force measurement, jerk/lead quality, and foot roll-off visualization in the partner graph.

> **Audience:** Developers and technically curious coaches. For the dancer-facing guide see [dancer_guide_partner.md](dancer_guide_partner.md).

> For system architecture, sensor fusion, ESP-NOW protocol, and the full step-detection pipeline see [Solo Dashboard — Technical Reference](solo_explanations.md).

---

## 1. Foot Roll-off Quality (Partner Graph)

The cyan (left) and magenta (right) lines in the combined analysis graph represent a continuous quality score derived from impact acceleration and rotational articulation:

$$\text{FootQuality} = \frac{\lvert \omega_\text{pitch} \rvert}{1.0 + \max(0,\; \lvert a_z \rvert - 1.0) \times 2.0}$$

> The denominator only penalises impacts above 1 g — foot-lifting (values below 1 g) does not degrade the quality score.

A **stomping error** (vertical cyan/magenta marker) fires when both conditions hold simultaneously:

`|accel_z| > ACCEL_MAX`  and  `|gyro_x| < GYRO_MIN`

For the full biomechanical derivation (Rocker Model, plantarflexion phases, empirical analysis) see [solo_explanations.md](solo_explanations.md).

---

## 2. Connection Force & Weight Distribution

*   **Measured value:** `weight` — push/pull force in grams (g) from the strain gauge held between the two dancers.
*   **Biomechanical meaning:** Represents the physical tension and compression transmitted through the connection frame between partners.
    *   **Positive values (> 0 g):** Compression (pushing / moving together).
    *   **Negative values (< 0 g):** Tension (pulling / stretching away).

---

## 3. Hand Jerk & Lead Smoothness

Jerk is the rate of change of acceleration — abrupt spikes correlate directly with harsh, unannounced redirections or yanks in the partner connection.

*   **Calculations:**
    1.  **Weight delta (ΔW):** $\Delta W = \left| \text{weight}_{\text{current}} - \text{weight}_{\text{previous}} \right|$
    2.  **Spatial acceleration jerk vector:**
        *   $\Delta a_x = a_{x,\text{current}} - a_{x,\text{previous}}$
        *   $\Delta a_y = a_{y,\text{current}} - a_{y,\text{previous}}$
        *   $\Delta a_z = a_{z,\text{current}} - a_{z,\text{previous}}$
        *   $\text{accelJerk} = \sqrt{(\Delta a_x)^2 + (\Delta a_y)^2 + (\Delta a_z)^2}$
    3.  **Raw lead hardness index (`fuehrungshaerteRaw`):** $\text{fuehrungshaerteRaw} = \left(\frac{\Delta W}{50.0}\right) + (\text{accelJerk} \times 15.0)$

*   **Biomechanical meaning:** High values indicate abrupt shocks, sudden yanks, or micro-stutters in the lead/follow frame rather than continuous, fluid momentum transfer.

---

## 4. Thresholds

| Parameter | Value | Origin / Rationale |
| :--- | :--- | :--- |
| **`GYRO_MIN`** | $80.0\,\text{deg/s}$ | Derived from empirical testing of clean dancing foot articulation; values below this threshold lack the necessary rolling motion. |
| **`ACCEL_MAX`** | $1.5\,g$ | Typical acceleration boundary separating cushioned steps from harsh impact shocks. |
| **Jerk threshold** | $12.0$ (raw index) | Threshold where auditory triggers activate to flag sharp, uncomfortable structural pulls. |
| **Graph scaling** | $-4000\,\text{g}$ to $+4000\,\text{g}$ | Chosen to capture standard WCS connection force variations without clipping. |

---

## 5. Visualization Architecture

The frontend renders real-time data using HTML5 Canvas elements updated via `requestAnimationFrame` loops at a 20 ms polling interval:

1.  **Connection Force Graph (`graph_kraft`):**
    *   Displays real-time weight curves across a 10-second sliding window.
    *   Dynamically shifts line colour between **Green** (compression / positive load) and **Red** (tension / negative load) based on zero-crossing logic.
2.  **Combined Analysis Graph (`graph_kombi`):**
    *   **Foot quality curves:** Left foot (Cyan), right foot (Magenta) — continuous score from the formula in §1. The curve spans the full canvas height: `Q = 0` renders at the bottom, `Q = 150` at the horizontal centre reference line, `Q = 300` at the top. The midline is therefore a useful threshold: curves in the upper half indicate above-average rolling quality; curves in the lower half indicate passive or impact-dominated movement.
    *   **Jerk tracking curve:** Yellow line — computed lead hardness/jerk profile, scaled to canvas coordinates.
    *   **Error markers:** Vertical full-height lines rendered whenever errors or jerk peaks breach thresholds:
        *   **Cyan / Blue:** Left foot impact/articulation error.
        *   **Magenta / Purple:** Right foot impact/articulation error.
        *   **Yellow / Orange:** Hand jerk / lead hardness spike.

> 📸 **[Screenshot: Combined Analysis graph showing cyan and magenta foot-quality lines with a yellow jerk curve and vertical error markers]**

---

## 6. Pelvis Metrics (Partner Dashboard)

When the pelvis sensor (`foot_id = 4`) is online, six badge metrics appear in the status bar. All six are always active — no level gating.

| Badge | Signal | Thresholds |
| :--- | :--- | :--- |
| **Hip Activation** | Peak `gYaw` over 500 ms, IIR-smoothed | ≥ 60°/s → ACTIVE ✅ \| 25–59°/s → MODERATE ⚠ \| < 25°/s → STIFF HIPS ❌ |
| **Lateral Stability** | Variance of lateral pelvic acceleration over 1 s | < 0.004 → STABLE ✅ \| 0.004–0.015 → SLIGHT SWAY ⚠ \| > 0.015 → LATERAL SWAY ❌ |
| **Hip-Foot Coupling** | Lead time: peak hip rotation → foot contact | > 100 ms → HIP LEADS ✅ \| 40–100 ms → IN SYNC ⚠ \| hip after foot → HIP LAGS ❌ |
| **Vertical Bounce** | Variance of vertical pelvic acceleration (gravity removed) over 1 s | < 0.006 → GROUNDED ✅ \| 0.006–0.020 → SLIGHT BOUNCE ⚠ \| > 0.020 → BOUNCY ❌ |
| **Anchor Settle** | Weighted score (0–100) over a tempo-adaptive window after each backward step | ≥ 60 → ANCHORED ✅ \| 30–59 → SETTLING ⚠ (score shown) \| < 30 → UNSTABLE ❌ |
| **Hip Settle** | Peak lateral pelvic acceleration (`earlyLatPeak`) in first half of Anchor Settle window | > 0.30 g → OVERSWING ⚠ \| 0.10–0.30 g + late variance < 0.015 → HIP SETTLE ✓ ✅ \| 0.05–0.10 g → SLIGHT SETTLE ⚠ \| ≤ 0.05 g → NO HIP SETTLE ❌ |

> 📸 **[Screenshot: Partner Dashboard status bar showing all six pelvis metric badges (Hip Activation through Hip Settle) simultaneously active]**

> **Hip Activation — tempo-adaptive thresholds:** The 60 °/s / 25 °/s values shown above are reference values at 500 ms/step. At runtime the thresholds scale with the current step interval: `scaleFactor = 500 / max(400, stepDurationMs)`. Effective thresholds: ACTIVE ≥ `round(60 × scaleFactor)` °/s, MODERATE ≥ `round(25 × scaleFactor)` °/s. At slow tempo (700 ms/step, scaleFactor ≈ 0.71): ACTIVE ≥ 43 °/s, MODERATE ≥ 18 °/s. At fast tempo (400 ms/step, scaleFactor = 1.25): ACTIVE ≥ 75 °/s, MODERATE ≥ 31 °/s.

### Anchor Settle — detail

Window duration: `anchorWindowMs = min(500, max(280, stepDurationMs))` — scales with current tempo.

Score composition:

$$\text{score} = \text{decelScore} \times 0.35 + \text{yawDampScore} \times 0.35 + \text{stabilScore} \times 0.30$$

* **decelScore** — sagittal deceleration: early mean > late mean (pelvis brakes forward momentum)
* **yawDampScore** — yaw damping: yaw peak in early half > late half (rotation stops after landing)
* **stabilScore** — late-phase stability: low variance of `|gYaw|` in second half of window

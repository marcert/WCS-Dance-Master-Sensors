# WCS Connection Dashboard — Technical Reference

This document covers the mathematical and biomechanical foundations of the **Connection Dashboard** (`/`) — connection force measurement, jerk/lead quality, and foot roll-off visualization in the partner graph.

> **Audience:** Developers and technically curious coaches. For the dancer-facing guide see [dancer_guide_partner.md](dancer_guide_partner.md).

> For system architecture, sensor fusion, ESP-NOW protocol, and the full step-detection pipeline see [Solo Dashboard — Technical Reference](solo_explanations.md).

---

## 1. Foot Roll-off Quality (Partner Graph)

The cyan (left) and magenta (right) lines in the combined analysis graph represent a continuous quality score derived from impact acceleration and rotational articulation:

$$\text{FootQuality} = \frac{\left| \text{gyro\_x} \right|}{1.0 + \max(0,\; \left| \text{accel\_z} \right| - 1.0) \times 2.0}$$

> The denominator only penalises impacts above 1 g — foot-lifting (values below 1 g) does not degrade the quality score.

A **stomping error** (vertical cyan/magenta marker) fires when both conditions hold simultaneously:

`|accel_z| > ACCEL_MAX`  and  `|gyro_x| < GYRO_MIN`

For the full biomechanical derivation (Rocker Model, plantarflexion phases, empirical validation) see [solo_explanations.md](solo_explanations.md).

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
| **Graph scaling** | $-3500\,\text{g}$ to $+3500\,\text{g}$ | Chosen to capture standard WCS connection force variations without clipping. |

---

## 5. Visualization Architecture

The frontend renders real-time data using HTML5 Canvas elements updated via `requestAnimationFrame` loops at a 20 ms polling interval:

1.  **Connection Force Graph (`graph_kraft`):**
    *   Displays real-time weight curves across a 10-second sliding window.
    *   Dynamically shifts line colour between **Green** (compression / positive load) and **Red** (tension / negative load) based on zero-crossing logic.
2.  **Combined Analysis Graph (`graph_kombi`):**
    *   **Foot quality curves:** Left foot (Cyan), right foot (Magenta) — continuous score from the formula in §1.
    *   **Jerk tracking curve:** Yellow line — computed lead hardness/jerk profile, scaled to canvas coordinates.
    *   **Error markers:** Vertical full-height lines rendered whenever errors or jerk peaks breach thresholds:
        *   **Cyan / Blue:** Left foot impact/articulation error.
        *   **Magenta / Purple:** Right foot impact/articulation error.
        *   **Yellow / Orange:** Hand jerk / lead hardness spike.

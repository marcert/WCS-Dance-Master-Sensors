# WCS Dashboard: Technical & Biomechanical Documentation

This document provides a comprehensive breakdown of the measurements, mathematical and biomechanical calculations, explicit threshold configurations, and 
visualization techniques implemented in the **WCS-Dance-Master** System.

---

## 1. System Overview & Data Acquisition

The architecture relies on a multi-node sensor network communicating via ESP-NOW protocols to a central master unit (M5Stack running a web server). 

*   **Foot Nodes (IDs 1 & 2):** Equipped with IMUs (Inertial Measurement Units) tracking angular velocity (`gyro_x`) and vertical impact acceleration (`accel_z`).
*   **Hand / Scale Node (ID 3):** Measures structural load forces (`weight`) combined with 3-axis spatial accelerations (`accel_x`, `accel_y`, `accel_z`).
*   **Controller Node:** Receives the sensor data, connects to the Wi-Fi (or spans its own access point), and delivers the HTML page for visualization.

---

## 2. Mathematical, Biomechanical & Physical Definitions

### A. Foot Mechanics: Roll-off Quality & Stomping Detection
In West Coast Swing, proper foot mechanics require a smooth weight transfer and gradual heel-to-toe or toe-to-heel articulation (**roll-off**), avoiding abrupt, un-articulated impacts (**stomping**).

*   **Measured Values:**
    *   `gyro_x` (Angular velocity around the roll axis, measured in degrees per second `deg/s`).
    *   `accel_z` (Vertical g-force impact vector).
*   **Calculations & Error Conditions:**
    The system checks for high-impact events lacking adequate rotational articulation. An error state is triggered on either foot when:
    
    `|accel_z| > ACCEL_MAX`  and  `|gyro_x| < GYRO_MIN`
    
    *   **Biomechanical Meaning:** The dancer strikes the floor hard (exceeding $1.5\,g$) without adequately rolling through the foot joint (failing to exceed $80\,\text{deg/s}$ rotation). This indicates harsh, un-buffered foot placement.
### B. Hand Connection Force & Weight Distribution
*   **Measured Values:**
    *   `weight` (Push/Pull force in grams, $\text{g}$).
*   **Biomechanical Meaning:** Represents the physical tension and compression transmitted through the connection frame between partners. 
    *   **Positive values ($> 0\,\text{g}$):** Compression (pushing/moving together).
    *   **Negative values ($< 0\,\text{g}$):** Tension (pulling/stretching away).

### C. Hand Jerk & Lead Smoothness
Jerk represents the rate of change of acceleration (or abrupt spikes in force application), which directly correlates with harsh, unannounced redirection or pulling in partner dance connections.

*   **Calculations:**
    1.  **Weight Delta ($\Delta W$):** $\Delta W = \left| \text{weight}\_{\text{current}} - \text{weight}\_{\text{previous}} \right|$
    2.  **Spatial Acceleration Jerk Vector:**
        *   $\Delta a_x = a_{x,current} - a_{x,previous}$
        *   $\Delta a_y = a_{y,current} - a_{y,previous}$
        *   $\Delta a_z = a_{z,current} - a_{z,previous}$
        *   $\text{accelJerk} = \sqrt{(\Delta a_x)^2 + (\Delta a_y)^2 + (\Delta a_z)^2}$
    3.  **Raw Lead Hardness Index (`fuehrungshaerteRaw`):** $\text{fuehrungshaerteRaw} = \left(\frac{\Delta W}{50.0}\right) + (\text{accelJerk} \times 15.0)$

*   **Biomechanical Meaning:** High values indicate abrupt shocks, sudden yanks, or micro-stutters in the lead/follow frame rather than continuous, fluid momentum transfer.
---

## 3. Thresholds and Origins

| Parameter | Value | Origin / Rationale |
| :--- | :--- | :--- |
| **`GYRO_MIN`** | $80.0\,\text{deg/s}$ | Derived from empirical testing of clean dancing foot articulation; values below this threshold lack necessary rolling motion. |
| **`ACCEL_MAX`** | $1.5\,g$ | Represents the typical baseline acceleration boundary separating cushioned steps from harsh impact shocks. |
| **Jerk Threshold** | $12.0$ (Raw Index) | Threshold where tactile feedback or auditory triggers activate to flag sharp, uncomfortable structural pulls. |
| **Graph Scaling Limits** | $-3500\,\text{g}$ to $+3500\,\text{g}$ | Chosen to comfortably capture standard WCS connection force variations without clipping. |

---

## 4. Visualization Architecture

The frontend renders real-time data across hardware boundaries utilizing HTML5 Canvas elements updated via requestAnimationFrame loops running a 20 ms polling interval:

1.  **Connection Force Graph (`graph_kraft`):**
    *   Displays real-time weight curves across a 10-second sliding window.
    *   Dynamically shifts line coloring between **Green** (Compression / positive or balanced load) and **Red** (Tension / negative load extremes) based on zero-crossing logic.
2.  **Combined Analysis Graph (`graph_kombi`):**
    *   **Foot Quality Curves:** Visualizes calculated left foot quality (Cyan line) and right foot quality (Magenta line) derived from impact deviation formulas:
        $$\text{LeftQuality} = \frac{\left| \text{currentLG} \right|}{1.0 + \max(0,\; \left| \text{currentLA} \right| - 1.0) \times 2.0}$$
        
        > The denominator only penalises impacts above 1 g — foot-lifting (values below 1 g) no longer degrades the quality score.
    *   **Jerk Tracking Curve:** Renders the computed lead hardness/jerk profile (Yellow dashed/solid line scaled to canvas coordinates).
    *   **Synchronous Error Lines:** Vertical full-height error markers rendered automatically whenever foot errors (`isError`) or jerk peaks (`isJerkPeak`) breach configured thresholds, offering immediate visual correlation against camera feed playback.
        *   **Cyan / Blue Lines:** Triggered specifically by **Left Foot** impact/articulation errors.
        *   **Magenta / Purple Lines:** Triggered specifically by **Right Foot** impact/articulation errors.
        *   **Yellow / Orange Lines:** Triggered by sudden **Hand Jerk / Lead Hardness** spikes.

# WCS Dance Master Sensors — Dancer's Guide: Partner Dashboard

> This guide covers the Partner Dashboard (`/`) — the view designed for a coach or dance partner watching from the side.  
> For the solo training view with level-gated feedback, see [dancer_guide_solo.md](dancer_guide_solo.md).

---

## 1. Getting Started

1. Open `http://192.168.4.1/` (root URL — no `/solo`) on a second phone or tablet while the dancer uses the Solo Dashboard on their own device.
2. **Tap `START CAM`** to activate the camera overlay. Position the device so the dancer is in frame.
3. **Tap `ZERO`** once while the dancer is standing in a neutral stance. This tares the foot angle offsets and resets the hardware zero on the force scale simultaneously.
4. The dashboard updates in real time — no further setup needed.

> The Partner Dashboard runs from the same M5 controller as the Solo Dashboard. Both views receive the same live sensor data.

---

## 2. Screen Layout

```
┌──────────────────────────────────────────────────────────────┐
│  ← g     FLIP CAM   EXIT   FREEZE   ZERO   REC START        │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│   CONNECTION FORCE  (−4.0 kg to +4.0 kg)                    │
│   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━   │
│                                                              │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│   COMBINED ANALYSIS  (Roll-off quality · Jerk · Errors)      │
│   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━   │
│   ■ Sound/Error Left   ■ Sound/Error Right   ■ Hand Jerk     │
├──────────────────────────────────────────────────────────────┤
│  STEP:  ⬅ BWD R   −7°   OPTIMAL TOE                         │
│  PELVIS: 🌀 ACTIVE  STABLE  HIP LEADS  GROUNDED  ANCHORED   │
└──────────────────────────────────────────────────────────────┘
```

**Top-left number** (`114 g`, `−39 g`, `— g`): live connection force reading from the hand sensor. Green = pull/tension, red = push/compression, grey dash = sensor offline.

> 📷 **Screenshot placeholder — full dashboard overview**  
> *(Replace with: full-screen photo of the partner dashboard with all sensors active, camera overlay visible in background, status bar showing step + pelvis badges)*

---

## 3. Connection Force Graph (Top Graph)

Shows the force measured by the strain gauge held between the two dancers, scaled to ±4.0 kg.

| Line colour | Meaning |
| :--- | :--- |
| **Green** (above centre) | Leader pulling — tension in the connection |
| **Red** (below centre) | Leader pushing — compression in the connection |
| **Flat line at centre** | Neutral — no measurable connection force |

**What to watch for:**

- **Calm, low-amplitude line near zero** → light, responsive connection. Ideal.
- **Sustained green elevation** → leader holding tension throughout — check whether the follow has room to move freely.
- **Sharp spikes** → sudden force changes — jerky leading or abrupt stopping. Compare with the yellow Jerk line in the lower graph to confirm.
- **Alternating green/red** → leader is not holding a directional intention — oscillating between push and pull within the same phrase.

> 📷 **Screenshot placeholder — connection force graph: calm lead**  
> *(Replace with: screenshot showing a smooth, low-amplitude green line near centre — good connection quality)*

> 📷 **Screenshot placeholder — connection force graph: jerky lead**  
> *(Replace with: screenshot showing repeated red/green spikes — compare with yellow jerk peaks visible in the lower graph at the same timestamps)*

---

## 4. Combined Analysis Graph (Bottom Graph)

Three overlaid data streams in a single canvas:

### Cyan line — Left foot roll-off quality

Derived from `|gyroPitch| / (1 + impactDev × 2)`. A higher value means the foot rolled off smoothly with low impact. The line rises when the left foot steps with good technique and drops during heavy, flat impacts.

### Magenta line — Right foot roll-off quality

Same formula as cyan, for the right foot.

**Reading both lines together:**
- Both lines tracking similarly at mid-height → symmetric, consistent technique.
- One line consistently lower → that foot is impacting harder or rolling off less cleanly than the other.
- Both lines near zero → foot sensors offline or dancer standing still.

### Yellow line — Hand Jerk index

A composite of force-rate-of-change and hand acceleration magnitude. Rises on sudden leading impulses, drops during smooth movement.

- **Yellow near zero** → smooth leading.
- **Yellow spikes** → abrupt force or acceleration changes in the hand connection.

### Vertical marker lines

| Colour | Meaning |
| :--- | :--- |
| **Cyan vertical bar** | Left foot error: heavy impact without roll-off |
| **Magenta vertical bar** | Right foot error: heavy impact without roll-off |
| **Red vertical bar** | Both feet error simultaneously |
| **Yellow dashed line** | Jerk peak detected by the firmware |

Error markers fire when impact acceleration exceeds 1.5 g with less than 80°/s roll-off — a stomping pattern. Multiple markers in a row on the same side indicate a recurring technique issue on that foot.

> 📷 **Screenshot placeholder — combined analysis graph: asymmetric foot quality**  
> *(Replace with: screenshot where cyan line is clearly higher than magenta across a full phrase — weaker right foot visible)*

> 📷 **Screenshot placeholder — combined analysis graph: jerk + error correlation**  
> *(Replace with: screenshot showing a yellow spike and a magenta/red vertical error marker appearing together at the same timestamp)*

---

## 5. Step Badges (Status Bar)

Updates on each detected foot contact. Uses the same classification as the Solo Dashboard Beginner level — no level selection required on the partner view.

| Element | Meaning |
| :--- | :--- |
| **➡ FWD L / R** | Forward step, left or right foot |
| **⬅ BWD L / R** | Backward step, left or right foot |
| **↔ FLAT L / R** | Ambiguous landing — too flat to classify direction |
| **θ angle** | Foot pitch at landing (positive = heel up, negative = toe down) |
| **Strike badge** | Classification of the landing — see table below |
| **Delay badge** | Tempo-normalised weight transfer timing — see table below |

### Strike badge reference

| Badge | Direction | Angle | Assessment |
| :--- | :--- | :--- | :--- |
| `OPTIMAL HEEL` ✅ | Forward | +10° to +35° | Correct heel-first contact |
| `HEEL SPIKE` ⚠️ | Forward | > +35° | Excessive heel angle |
| `OPTIMAL TOE` ✅ | Backward | −5° to −20° | Correct toe-ball contact |
| `ANCHOR SETTLE` ✅ | Backward | near flat + low impact | Foot settling into anchor — weight redistributing |
| `HEEL DROP` ⚠️ | Backward | > +5° | Heel contacting before toe on a backward step |
| `HEEL LANDING!` ❌ | Backward | > +10° | Definitive heel-first on a backward step |
| `FLAT-FOOT!` ❌ | Ambiguous | −5° to +5° | Flat landing without classification |
| `BALL-STEP` | Forward | < −5° | Ball-first on a forward step |

> 📷 **Screenshot placeholder — status bar: step badges**  
> *(Replace with: close-up of the status bar row showing e.g. `⬅ BWD R  −7°  OPTIMAL TOE` with the pelvis row hidden)*

### Delay badge reference

The delay badge measures how quickly weight was committed after foot contact, expressed as a fraction of the step interval — so it is **automatically scaled to the music tempo**. A slow song and a fast song will show the same badge for the same quality of movement.

Thresholds differ by direction because a backward (toe-first) landing naturally requires more settling time than a forward (heel-first) one.

| Badge | Forward step | Backward step | Assessment |
| :--- | :--- | :--- | :--- |
| `DELAYED ✓` ✅ | 12–38% of beat | 18–50% of beat | Characteristic WCS "hover" — weight arrives after the foot |
| `QUICK` ⚠️ | < 12% | < 18% | Weight committed immediately at impact — mechanical, not musical |
| `LATE` ⚠️ | > 38% | > 50% | Weight never fully arrived — floating or incomplete transfer |

> **Coaching tip:** `QUICK` on every anchor step is the most common finding at Newcomer/Intermediate level. The dancer steps back but immediately drops their weight, losing the stretch in the connection. Watch for `QUICK` in the status bar and cue: *"Step back and breathe before you land."*

> 📷 **Screenshot placeholder — delay badge: DELAYED ✓ on anchor**
> *(Replace with: status bar showing `⬅ BWD R  −12°  OPTIMAL TOE  DELAYED ✓` — all green, good technique)*

--- (Status Bar — appears when sensor is online)

All five pelvis metrics are shown simultaneously when the pelvis sensor is active — there is no level selector on the partner view.

For full descriptions of each badge see [dancer_guide_solo.md — Section 8](dancer_guide_solo.md#8-the-pelvis-card-optional-sensor).

### Quick reference

| Badge | Green | Yellow | Red |
| :--- | :--- | :--- | :--- |
| **Hip Activation** | `🌀 ACTIVE` (≥60°/s) | `MODERATE` (25–60°/s) | `STIFF HIPS` (<25°/s) |
| **Lateral Stability** | `STABLE` | `SLIGHT SWAY` | `LATERAL SWAY` |
| **Hip-Foot Coupling** | `HIP LEADS` (>100 ms before foot) | `IN SYNC` (40–100 ms) | `HIP LAGS` (<40 ms) |
| **Vertical Bounce** | `GROUNDED` | `SLIGHT BOUNCE` | `BOUNCY` |
| **Anchor Settle** | `ANCHORED (n)` (≥60) | `SETTLING (n)` (30–59) | `UNSTABLE (n)` (<30) |

> 📷 **Screenshot placeholder — status bar: pelvis badges active**  
> *(Replace with: close-up of the full status bar with both rows visible — step row + PELVIS: row showing all 5 badges lit in various colours)*

---

## 7. Buttons

| Button | Function |
| :--- | :--- |
| **START CAM / FLIP CAM** | Activates the camera overlay; tap again to flip between front and rear camera |
| **EXIT** | Exits fullscreen mode |
| **FREEZE** | Pauses both graphs for inspection — useful for discussing a moment after a run |
| **ZERO** | Tares foot angle offsets (resets the direction/angle baseline) AND triggers hardware tare on the force scale. Tap while the dancer stands in neutral stance. |
| **REC START / STOP** | Records the full screen (graphs + camera overlay + audio) to a `.webm` file downloaded automatically on stop |

---

## 8. Coaching Use Cases

### Reading lead quality in real time

Watch the **Connection Force graph** while the pair is dancing. A good lead produces a calm line with brief, intentional peaks — tension rises when the leader initiates, returns to near-zero when the follow has taken over. Persistent elevation or repeated spikes indicate the leader is not releasing the follow between moves.

### Identifying the weaker foot

Compare the **cyan vs magenta lines** in the Combined Analysis graph across a full phrase. If one line tracks consistently lower, that foot is the one to drill. Use `FREEZE` after a run and compare peak heights visually.

> 📷 **Screenshot placeholder — FREEZE: comparing cyan vs magenta**  
> *(Replace with: screenshot after tapping FREEZE, graphs paused, with a visible difference in peak height between the two foot quality lines)*

### Checking hip initiation

The **Hip-Foot Coupling** badge fires on each step. Consistent `HIP LAGS` means the dancer is walking with their legs, not their core — the single most common technical gap at Newcomer through Intermediate level.

### Anchor quality under pressure

After each anchor, the **Anchor Settle** badge shows a 0–100 score. A score below 60 consistently across a full song means the dancer's pelvis is still moving after the anchor step lands. Low scores late in a song (but not early) indicate fatigue-driven anchor collapse.

### Using FREEZE for discussion

Stop the music, tap **FREEZE** immediately after a notable moment. The graphs hold the last 10 seconds. Walk the dancer through what you see before releasing the freeze.

# WCS Dance Master Sensors — Dancer's Guide: Solo Dashboard

> This guide is written for dancers, not engineers. You do not need to understand the math.  
> Open the dashboard on your phone or tablet, follow the setup steps, and use the badge colour as your real-time coach.

→ For the partner/coach view, see [dancer_guide_partner.md](dancer_guide_partner.md).

---

## 1. Getting Started

1. **Mount your phone or tablet on a tripod** at eye level, facing you. Use **landscape orientation** for best results.
2. **Open the Solo Dashboard** in your browser (`http://192.168.4.1/solo` on the M5 hotspot, or the home WiFi IP shown on the M5 display).
3. **Tap `📷 CAM`** to activate the camera overlay. Your live body appears behind the data cards.
4. **Put on your dance shoes** before the next step.
5. **Stand in your natural dance stance** — feet shoulder-width, slight forward weight. Tap `📐 ZERO`. The system now knows what "flat foot on the floor" means for your shoes.
6. **Tap `🔊 Biofeedback: OFF`** to turn audio ON. The beeps fire faster than you can read the screen — they are your primary alert.
7. Start walking or dancing. Give yourself 10–15 steps to warm up before analysing anything.

> **Re-tare whenever you change shoes or surfaces.** The `📐 ZERO` calibration is shoe-specific.

---

## 2. Layout

In landscape mode the screen is divided into two columns:

```
┌─────────────────────────┬──────────────────┬──────────────────┐
│                         │  Pelvis –        │  Double Stance   │
│   📷  CAMERA            │  Hip Mechanics   │  Overlap         │
│   visible here          │  (if sensor on)  │                  │
├─────────────────────────├──────────────────┼──────────────────┤
│   Roll-off Dynamics     │  Roll-off        │  Last Step       │
│   Graph                 │  Symmetry (ASI)  │  (Step Badge)    │
└─────────────────────────┴──────────────────┴──────────────────┘
```

- **Top-left area**: shows the **Pelvis — Hip Mechanics** card when the pelvis sensor is clipped on and powered; otherwise empty so the camera shows through unobstructed.
- **Bottom-left**: live roll-off dynamics graph (pitch angular velocity of both feet over time).
- **Top-right**: Double Stance Overlap card.
- **Bottom-right**: Last Step card (your primary real-time feedback).
- **Bottom-centre**: Roll-off Symmetry & Smoothness card.

<p align="center">
  <kbd><img src="https://raw.githubusercontent.com/marcert/WCS-Dance-Master-Sensors/refs/heads/main/Attachments/scr.jpg" width="600"></kbd>
</p>

---

## 3. Choosing Your Training Level

The **`👤 BEG`** button in the top-right corner of the header cycles through three training levels. Tap it to switch; the selection is remembered between sessions.

| Button | Level | What is visible |
| :--- | :--- | :--- |
| `👤 BEG` (green) | **Beginner** | [Step Badge](#4-the-step-badge-card) card only — direction, angle, strike badge. [Pelvis card](#8-the-pelvis-card-optional-sensor) shows [Hip Activation](#hip-activation) if sensor is online. |
| `🏃 INT` (orange) | **Intermediate** | [Step Badge](#4-the-step-badge-card) (full, with [Jerk](#impact-jerk-bar) + [Push-Off](#push-off-badge-int--adv)) + [Double Stance](#6-the-double-stance-card). [Pelvis card](#8-the-pelvis-card-optional-sensor) adds [Lateral Stability](#lateral-stability-int), [Hip-Foot Coupling](#hip-foot-coupling-int), [Vertical Bounce](#vertical-bounce-int). |
| `⭐ ADV` (purple) | **Advanced** | All cards — [Step Badge](#4-the-step-badge-card), [Double Stance](#6-the-double-stance-card), [Roll-off Symmetry & Smoothness](#7-the-roll-off-symmetry--smoothness-card). [Pelvis card](#8-the-pelvis-card-optional-sensor) adds [Anchor Settle](#anchor-settle-adv). |

Cards that are not relevant for your level are hidden, giving the camera maximum screen space.

> **Note:** These levels describe the **complexity of feedback shown on screen** — they have nothing to do with your WSDC competition division. A Champion-level dancer starting a new drill should use `👤 BEG` to focus on one thing at a time. An absolute newcomer drilling something specific may benefit from switching to `🏃 INT`. Choose the level that matches what you are currently working on, not your competition résumé.

<p align="center">
  <kbd><img src="https://raw.githubusercontent.com/marcert/WCS-Dance-Master-Sensors/refs/heads/main/Attachments/Buttons.jpg" width="600"></kbd>
</p>

### Beginner view

One card, bottom-right. Everything else is camera. Look at the [direction badge](#how-direction-is-determined) and the [strike badge](#4-the-step-badge-card) after each step. Nothing else.

### Intermediate view

Two cards at the bottom. [Step technique](#4-the-step-badge-card) on the right, [timing quality (Double Stance)](#6-the-double-stance-card) on the left. The upper half of the screen remains camera.

### Advanced view

Three metric cards plus the live graph: [Step Badge](#4-the-step-badge-card), [Double Stance](#6-the-double-stance-card), and [Roll-off Symmetry & Smoothness](#7-the-roll-off-symmetry--smoothness-card). Use this level for detailed analysis sessions, not for learning new patterns.

---

## 4. The Step Badge Card

This is the **primary real-time feedback card**. It updates on every detected foot contact.

### What the display elements mean

| Element | What it tells you |
| :--- | :--- |
| **Direction badge** | ➡️ FORWARD (θ ≥ +8°), ⬅️ BACK (θ < −8°), or — when the angle is in the ambiguous zone |
| **θ angle** | Pitch of your foot at landing. Positive = heel higher than toe. |
| **Strike badge** (large coloured label) | Classification of that landing — see tables below |
| **Jerk bar** | Rate of impact force — how hard your foot hit the floor |
| **PUSH-OFF badge** | Push-off power of your trailing foot (Beginner: hidden) |
| **LOADING badge** | How smoothly (gradient) weight loaded onto the landing foot — [see section below](#loading-badge-adv-only) (Advanced only) |
| **DELAY badge** | How quickly weight committed relative to the beat tempo — [see section below](#delay-badge-int--adv) (Intermediate + Advanced) |
| **ANKLE ROLL badge** | Ankle shock absorption at landing (Advanced only) |

### How direction is determined

The system classifies direction from foot pitch angle θ. Direction is **reliable only at the extremes**:

| Direction badge | θ at landing | Meaning |
| :--- | :--- | :--- |
| **➡️ FORWARD** | θ ≥ +8° | Clear dorsiflexion — heel contacted first |
| **—** (grey) | −8° to +7° | Ambiguous zone — foot too flat to classify direction |
| **⬅️ BACK** | θ < −8° | Clear plantarflexion — toe-ball contacted first |

When θ is between −8° and +7°, the system cannot reliably determine direction. The direction badge shows — (grey). Use the camera view to check actual direction.

### HEEL zone badges (➡️ FORWARD, θ ≥ +8°)

| Badge | Jerk | What you did | Target |
| :--- | :--- | :--- | :--- |
| `HEEL STRIKE ✓` | ≤ 22 g/s | Clean heel strike — controlled contact | Target for all forward walks and breaks |
| `HEEL SLAM ⚠` | > 22 g/s | Hard heel impact — excessive landing force | Bend the knee on contact and soften the ankle |

### Ambiguous zone (—, −8° to +7°)

The foot is too flat to classify direction. The quality badge still fires:

| Badge | Jerk | What it means |
| :--- | :--- | :--- |
| `SOFT ✓` | ≤ 20 g/s | Light, controlled landing — good absorption in this zone |
| `MODERATE` | 20–22 g/s | Moderate impact — acceptable, but worth reducing |
| `HARD IMPACT ⚠` | > 22 g/s | Heavy flat-foot landing — stomping pattern |
| `BRUSH+HEEL` | — | Ambiguous landing followed by heel-set within 200 ms — reclassified to ➡️ FORWARD; correct technique confirmed |

Use the camera view to check actual direction when the direction badge shows —.

### TOE zone badges (⬅️ BACK, θ < −8°)

| Badge | Jerk | What you did | Target |
| :--- | :--- | :--- | :--- |
| `TOE-FIRST ✓` | ≤ 22 g/s | Clean toe-ball contact — controlled landing | Target for all backward walks, anchors, extensions |
| `TOE JAM ⚠` | > 22 g/s | Hard toe impact — excessive landing force | Moderate the extension; absorb through the ankle |

> **Note on early heel drops:** A backward step where the heel contacts before the toe will land in the ambiguous zone (—) rather than ⬅️ BACK. If you see consistent SOFT/MODERATE/HARD IMPACT on what you believe are backward steps, your heel is contacting too early. Focus on sending the toe out first and keeping the ankle relaxed until the foot settles.

### Impact Jerk bar

- **Short bar, no click** → soft, controlled landing. Ideal.
- **Bar past yellow + 1200 Hz click** → `HEEL SLAM ⚠`, `TOE JAM ⚠`, or `HARD IMPACT ⚠` badge — hard landing. Bend the knee on contact and absorb with the ankle.
- **Full red bar + 500 Hz click** → extreme impact (> 30 g/s) — separate alert from the badge system.

> 📸 **[Screenshot: Step Badge card showing a HEEL STRIKE ✓ result with low Jerk bar, direction badge, and theta angle visible]**

---

## 5. Push-Off & Loading Badges

These three badges appear below the Jerk bar and update after each step. Visible from **Intermediate** level (Push-Off) or **Advanced** (Loading + Ankle Roll).

### Push-Off badge (INT + ADV)

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `🚀 POWER PUSH` ✅ | Strong push-off from the trailing foot — optimal for both forward propulsion and anchor redistribution | Maintain — the system adjusts its target automatically: higher threshold for forward walks, lower for anchors and backward steps |
| `↗ PUSH` ⚠️ | Push-off detected but below the directional target | Drive more actively through the ball of the trailing foot; think "push the floor away" |
| `— PUSH-OFF` | No significant push-off detected | Trailing leg is passive. Actively extend the ankle at the end of each walk |

### Loading badge (ADV only)

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `SMOOTH LOAD` ✅ | Weight transferred progressively onto the landing foot | Good joint mechanics — keep it |
| `INSTANT LOAD` ⚠️ | Full weight dropped at impact | Slow the COM; think of "receiving" the floor rather than landing on it |
| `EARLY UNLOAD` ⚠️ | Weight shifting away before foot is settled | You are rushing to the next step. Complete the current weight investment before moving |

### Delay badge (INT + ADV)

Measures how quickly you committed your weight after foot contact, expressed as a **fraction of your current step interval** — so it adjusts automatically to the music tempo. The same physical movement reads identically at 90 BPM and 160 BPM.

Thresholds differ by direction: a backward (toe-first) step naturally needs more settling time than a forward (heel-first) one.

| Badge | Forward step | Backward step | What it means |
| :--- | :--- | :--- | :--- |
| `DELAYED ✓` ✅ | 12–38% of beat | 18–50% of beat | WCS-characteristic hover — weight arrives after the foot contacts |
| `QUICK` ⚠️ | < 12% | < 18% | Weight dropped immediately at contact — mechanical, not musical |
| `LATE` ⚠️ | > 38% | > 50% | Weight never fully committed — floating or incomplete transfer (ADV only) |

At **INT** level only `DELAYED ✓` / `QUICK` are shown — every delayed transfer is already progress. `LATE` is added at **ADV** level where over-hovering also becomes a problem.

> The **LOADING badge** (`SMOOTH LOAD` / `INSTANT LOAD`) and the **DELAY badge** answer different questions:
> - LOADING: *Was the ramp gradual?* (quality of how you arrived)
> - DELAY: *Did you arrive in time?* (timing relative to the beat)

### Ankle Roll badge (ADV only)

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `RIGID LEVER` ✅ | Full pronation → supination cycle detected: ankle absorbed the impact AND locked up for push-off | Optimal ankle mechanics — maintain |
| `ANKLE FLEX` ✅ | Ankle rolling through impact (pronation impulse detected) | Good shock absorption — ankle acting as natural spring |
| `MODERATE ROLL` ⚠️ | Some ankle mobility, but could be more | Consciously relax the ankle at landing; avoid bracing the foot rigid |
| `STIFF ANKLE` ⚠️ | Minimal ankle roll — impact transmitted directly up the chain | Land with a soft, unlocked ankle. Over time this reduces knee and hip load |

---

## 6. The Double Stance Card

Visible from **Intermediate** level. This card tells you how long both feet are on the floor simultaneously during each weight transfer.

| Badge | Overlap ratio | What it means | Training implication |
| :--- | :--- | :--- | :--- |
| `OPTIMAL ROLL` ✅ | 15%–52% | Smooth, grounded weight transfer | The characteristic WCS rolling connection |
| `HECTIC` ⚠️ | < 15% | Rushed — one foot leaves before the other is secure | "Peel, don't lift" — roll through the foot before stepping |
| `SLUGGISH` ⚠️ | > 52% | Prolonged double contact — hesitation or heavy stance | Commit to the COM shift earlier |

Watch this card during **triple steps and walks**. `HECTIC` on an anchor step often means you are rushing out of the anchor before building connection.

> 📸 **[Screenshot: Double Stance Overlap card showing OPTIMAL ROLL badge with the overlap percentage displayed]**

---

## 7. The Roll-off Symmetry & Smoothness Card

Visible at **Advanced** level only.

| Display | What it tells you | Green target |
| :--- | :--- | :--- |
| **ASI %** | Difference between left and right foot roll-off | `SYMMETRIC` — below 15% |
| **Smoothness** | Fluidity of ankle articulation across both feet | `SMOOTH` — 65 or above |

- High **ASI** (e.g. `ASYMMETRIC` > 35%) usually means one ankle is stiffer, or one side is compensating for an old injury.
- Low **Smoothness** means your ankle movements are jerky. Slow the tempo and focus on rolling through the full foot rather than stepping flat.

---

## 8. The Pelvis Card (Optional Sensor)

The pelvis card appears in the **top-left slot** of the dashboard whenever the pelvis sensor is powered on. When the sensor is offline, that slot remains empty and the camera shows through.

### Mounting the sensor

Clip the sensor to the **posterior waistband at the small of your back** (L5 / sacrum level), display facing away from your body. Centred on the spine is ideal, but left or right of centre by a few centimetres makes no practical difference. It should sit flat and snug — not dangling.

### Mounting verification

At the top of the pelvis card a small grey data line shows three live raw values:

```
aZ:+0.95  aX:-0.20  gY:   0
```

| Value | What it shows | Expected at rest |
| :--- | :--- | :--- |
| `aZ` | Vertical acceleration | **+0.90 to +1.00** (gravity) |
| `aX` | Lateral acceleration | −0.30 to +0.30 (small tilt offset is normal) |
| `gY` | Hip yaw rate (°/s) | Near **0** |

If `aZ` is far from +0.95 (e.g. near 0 or negative), the sensor is not mounted correctly — it may be rotated or facing the wrong way. Re-clip it flat against the back with the display facing outward.

### Badge overview

| Badge row | Level | What is measured |
| :--- | :--- | :--- |
| **Hip Activation** | BEG+ | How much the pelvis is rotating during movement (yaw) |
| **Lateral Stability** | INT+ | Lateral sway of the pelvis during movement |
| **Hip-Foot Coupling** | INT+ | Whether hips initiate each step or follow the feet |
| **Vertical Bounce** | INT+ | How much vertical movement the pelvis generates |
| **Anchor Settle** | ADV | Quality of the pelvis settle in the 500 ms after each anchor step |
| **Hip Settle** | ADV | Whether the pelvis shifts into the standing hip after each anchor step (lateral tilt) |

> 📸 **[Screenshot: Pelvis card in the top-left slot showing all badge rows (Hip Activation through Anchor Settle) with sensor active]**

---

### Hip Activation

Measures peak transverse hip rotation (yaw rate) over a rolling 500 ms window.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `🌀 ACTIVE` ✅ | Strong hip rotation contributing to the movement | Maintain |
| `MODERATE` ⚠️ | Some rotation but hip contribution is limited | Wind the hip up before each step — rotation should start in the pelvis, not the ankle |
| `STIFF HIPS` ❌ | Pelvis barely rotating — movement driven by legs only | Drill isolated hip rotations first, then add feet. "Lead with the hip, not the heel." |

> In WCS, hip rotation should accompany or precede each step. `STIFF HIPS` is one of the most common beginner patterns and is invisible to foot sensors alone.

---

### Lateral Stability (INT+)

Measures lateral acceleration variance of the pelvis over 1 second.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `STABLE` ✅ | Minimal lateral pelvis movement — good horizontal control | Good |
| `SLIGHT SWAY` ⚠️ | Some lateral oscillation — common on turns or transitions | Check for hip hike on the stepping side; keep the pelvis level |
| `LATERAL SWAY` ❌ | Significant side-to-side movement | Look for compensatory hip push on each step; practise walks with a conscious level pelvis |

---

### Hip-Foot Coupling (INT+)

Compares when peak hip rotation occurred relative to the moment of foot contact.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `HIP LEADS` ✅ | Peak hip rotation occurred more than 100 ms before foot contact | Good initiation — hips are driving the step |
| `IN SYNC` ⚠️ | Hip peak and foot contact within 40–100 ms of each other | Acceptable — try amplifying the pre-step hip "launch" |
| `HIP LAGS` ❌ | Hips rotating at or after foot contact | Legs are moving independently of the core. Slow down and practise initiating each walk from the hip, letting the foot follow |

---

### Vertical Bounce (INT+)

Measures the variance of vertical pelvis acceleration (gravity removed) over 1 second.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `GROUNDED` ✅ | Minimal vertical movement | Good |
| `SLIGHT BOUNCE` ⚠️ | Some vertical oscillation | Maintain a light bend in the knees throughout; avoid extending to a straight leg during travel |
| `BOUNCY` ❌ | Significant up-down movement | Stay in your knees. Think: "stay low, stay connected." |

---

### Anchor Settle (ADV)

After every backward (anchor) step, the system opens a **tempo-adaptive measurement window** (280–500 ms, automatically scaled to the current step tempo) and evaluates three signals:

1. **Deceleration** — did the pelvis slow down in the anterior-posterior direction?
2. **Yaw damping** — did hip rotation slow after the step?
3. **Stability** — how still was the pelvis in the second half of the window?

These three components are combined into a 0–100 score displayed in the badge.

| Badge | Score | What it means | How to improve |
| :--- | :--- | :--- | :--- |
| `ANCHORED (n)` ✅ | ≥ 60 | Strong deceleration + yaw damping + stable hold | Good — work on consistency across every anchor step |
| `SETTLING (n)` ⚠️ | 30–59 | Partial settle — one or two components weak | Identify the weak component using the tips below |
| `UNSTABLE (n)` ❌ | < 30 | Pelvis still moving or wobbling after the anchor | Focus on "sticking" the anchor — reach the end of the slot and hold |

**Diagnosing a low score:**
- **Low score despite clean foot technique** → the issue is pelvis, not foot angle. Work on the settle itself, not the step.
- **`HECTIC` Double Stance + low Anchor Settle** → you are leaving the anchor before the pelvis has stabilised.
- **`🌀 ACTIVE` hip + `UNSTABLE` anchor** → hips rotate well during travel but do not dampen at the anchor. Practise a deliberate "soft stop".

---

### Hip Settle (ADV)

Measures whether you "settle into the hip" after an anchor step — i.e. whether a brief lateral pelvic shift towards the standing leg occurs and then holds. The system evaluates `aLatP` (lateral acceleration of the pelvis sensor) within the same 500 ms window as Anchor Settle.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `HIP SETTLE ✓` | Clear lateral impulse early in the window, stable hold after — pelvis consciously settling onto the standing leg | Maintain |
| `SLIGHT SETTLE` | Small lateral impulse present but not pronounced | Let more weight consciously drop onto the standing leg and hold |
| `NO HIP SETTLE` | No lateral impulse — pelvis stays neutral after the anchor | Actively load the standing leg: after the anchor step, allow the hip to drop slightly towards the standing side |
| `OVERSWING ⚠` | Lateral impulse too strong — pelvis swings too far to the side | Moderate the movement; the lateral shift should be subtle, not a visible swing |

> **Note:** "Settling into the hip" is a stylistic element — some teaching styles emphasise it strongly, others less so. In WCS, the lateral pelvic movement is intentionally subtler than in Latin dance: the goal is a "grounded arrival", not a visible swing. This badge provides information, not a verdict. If your teacher does not want a lateral settle, disregard this badge.
>
> **Thresholds (0.05 / 0.10 / 0.30 g):** These values are based on biomechanical reference data and can be adjusted after a first test session with the pelvis sensor.

---

## 9. Training by Skill Level

### Beginner — use `👤 BEG`

**One focus: heel vs. toe contact.**

1. Walk forward. Does the badge say `HEEL STRIKE ✓`? If not — your heel is not contacting first. Lift the heel slightly more before the foot lands.
2. Walk backward. Does the badge say `TOE-FIRST ✓`? If not — send your toe out first. If you see — (ambiguous) on backward steps, your heel is contacting the floor before the toe.
3. If you hear the **1200 Hz beep**, stop and slow down. That is a `HEEL SLAM ⚠`, `TOE JAM ⚠`, or `HARD IMPACT ⚠` — too much landing force.
4. Practise at a slow tempo until `HEEL STRIKE ✓` and `TOE-FIRST ✓` appear consistently. Only then increase speed.

**With pelvis sensor:** Watch **Hip Activation** only. If `STIFF HIPS` appears consistently, your legs are moving without your core engaging.

### Intermediate — use `🏃 INT`

**Two focuses: technique consistency + weight transfer timing.**

1. Forward walks → aim for consistent `HEEL STRIKE ✓` with the Jerk bar low.
2. Backward walks → aim for `TOE-FIRST ✓`. A — direction badge on a backward step means the foot is landing too flat — the heel is dropping before the toe.
3. Watch the **POWER PUSH badge**: is your trailing leg passive?
4. Introduce the **Double Stance card**: work toward `OPTIMAL ROLL` during triple steps.
5. Watch the new **DELAY badge**: aim for `DELAYED ✓` on anchor steps. Consistent `QUICK` means you are dropping weight immediately — no musical breath in the connection.

**With pelvis sensor:** Add **Lateral Stability**, **Hip-Foot Coupling**, and **Vertical Bounce**. The single most valuable metric at this level is Hip-Foot Coupling — consistent `HIP LAGS` means you are walking with your feet, not your body.

### Advanced — use `⭐ ADV`

**Full biomechanical feedback loop.**

1. Use **SMOOTH LOAD vs INSTANT LOAD** to fine-tune weight reception — especially on syncopated patterns.
2. Cross-read **DELAY badge** with **SMOOTH LOAD**: `DELAYED ✓` + `SMOOTH LOAD` is the ideal combination — both the timing and the ramp quality are right. `DELAYED ✓` + `INSTANT LOAD` means you waited but then dropped; `QUICK` + `SMOOTH LOAD` means the gradient was good but the hover was too short.
3. Compare **ASI** between left and right over a full practice session. A consistently worse side points to a compensation pattern.
4. Use **ANKLE FLEX vs STIFF ANKLE** to monitor fatigue — ankle stiffness increases as muscles tire.
5. Film with `📷 CAM` and replay during pauses.
5. Use the **Roll-off Dynamics graph** to compare peak gyro values between feet.

**With pelvis sensor:** Focus on **Anchor Settle** as your anchor quality KPI. Run a full 8-count basic and check the score after each anchor step.

---

## 10. Common Problems and How to Fix Them

| What you see | Root cause | Fix |
| :--- | :--- | :--- |
| `HEEL SLAM ⚠` on forward walks | Heel contacting hard — insufficient knee or ankle absorption | Slow down. Bend the knee more on contact and soften the ankle. |
| `HARD IMPACT ⚠` on forward walks | Ankle held rigid; no heel articulation | Slow down. Exaggerate heel-first contact consciously. |
| — badge on backward steps | Heel contacting before toe | Send the toe first, keep the ankle relaxed until the foot settles. |
| `TOE JAM ⚠` consistently | Hard toe impact on backward steps | Moderate the extension; absorb the landing through the ankle. |
| `HECTIC` Double Stance | Rushing the transfer; foot lifts too early | "Leave the floor last" — let the whole foot peel up from the toe. |
| `SLUGGISH` Double Stance | Hesitating before committing weight | Trust the floor. Move the body, not just the foot. |
| `STIFF ANKLE` consistently | Braced ankle at landing | Visualise landing on a sponge. Consciously unlock the ankle before contact. |
| `ASYMMETRIC` ASI | One foot stiffer or less articulated | Identify which foot and drill that foot in isolation. |
| `— PUSH-OFF` (no badge) | Trailing leg passive | "Push the floor, don't just lift the foot." |
| `INSTANT LOAD` on anchors | Dropping weight abruptly | "Melt into the anchor" rather than "land on it". |
| `QUICK` on anchor steps | No hover before weight commitment | "Step back, breathe, then settle." Cue a deliberate pause between foot contact and weight arrival. |
| `LATE` on forward walks | Weight never fully arriving | Trust the transfer — commit fully before initiating the next step. |
| `STIFF HIPS` constantly | Legs moving without core engagement | Start each step with a deliberate hip rotation impulse before the foot moves. |
| `LATERAL SWAY` continuously | Hip hike or lateral pelvis push | Keep the pelvis level; check for asymmetric weight distribution. |
| `HIP LAGS` on every step | Legs and core disconnected | Slow to very slow tempo. Initiate hip rotation, then let the foot follow. |
| `BOUNCY` continuously | Knee extension during travel | Stay in a slight knee bend throughout. The height of your head should not change between steps. |
| `UNSTABLE` anchor always | Pelvis still rotating after the anchor | Step back, plant both feet, and consciously stop all hip movement. Hold for two counts. |

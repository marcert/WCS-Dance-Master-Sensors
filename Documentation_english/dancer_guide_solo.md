# WCS Dance Master Sensors — Dancer's Guide: Solo Dashboard

> This guide is written for dancers, not engineers. You do not need to understand the math.  
> Open the dashboard on your phone or tablet, follow the setup steps, and use the badge colour as your real-time coach.

> ⚡ **Just want to start?** Skip this and read the **[Solo Quickstart](quickstart_solo.md)** — dancing in 60 seconds, come back here later.

→ For the partner/coach view, see [dancer_guide_partner.md](dancer_guide_partner.md).

---

## 1. Getting Started

1. **Hold your phone or tablet in your hand** in front of you in **landscape** so you can glance at the badges between steps. *(Prefer the camera overlay? Prop it on a tripod at eye level facing you instead.)*
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
- **Bottom-left**: live roll-off dynamics graph (how fast each foot rotates as it rolls through, over time).
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
| `👤 BEG` (green) | **Beginner** | [Step Badge](#4-the-step-badge-card) card only — direction, strike badge, ROLL badge. [Pelvis card](#9-the-pelvis-card-optional-sensor) shows [Hip Activation](#hip-activation) if sensor is online. |
| `🏃 INT` (orange) | **Intermediate** | [Step Badge](#4-the-step-badge-card) (full, with [Push-Off](#push-off-badge-int--adv)) + [Double Stance](#6-the-double-stance-card). [Pelvis card](#9-the-pelvis-card-optional-sensor) adds [Lateral Stability](#lateral-stability-int), [Hip-Foot Coupling](#hip-foot-coupling-int), [Vertical Bounce](#vertical-bounce-int). |
| `⭐ ADV` (purple) | **Advanced** | All cards — [Step Badge](#4-the-step-badge-card), [Double Stance](#6-the-double-stance-card), [Roll-off Symmetry & Smoothness](#7-the-roll-off-symmetry--smoothness-card), [Grounding Card](#8-the-grounding-card-adv). [Pelvis card](#9-the-pelvis-card-optional-sensor) adds [Anchor Settle](#anchor-settle-adv). |

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

Four metric cards plus the live graph: [Step Badge](#4-the-step-badge-card), [Double Stance](#6-the-double-stance-card), [Roll-off Symmetry & Smoothness](#7-the-roll-off-symmetry--smoothness-card), and [Grounding Card](#8-the-grounding-card-adv). The Grounding Card appears to the left of the (narrower) graph and consolidates SDR, SETTLE, and GND Score in one panel. Use this level for detailed analysis sessions, not for learning new patterns.

---

### Leader / Follower Mode

The **👤 LEADER** button (blue) in the top bar switches to **💃 FOLLOWER** mode (pink) and back. The setting is saved in the browser and persists across sessions.

**When to use Follower mode:** Activate it when you are dancing the follower role. The system adjusts three metrics to account for the structural differences of reactive (follower) movement:

| What is adjusted | What changes in Follower mode |
|---|---|
| **Weight transfer timing** (DELAY) | The expected window shifts earlier — followers react to the lead, so weight transfer is naturally faster |
| **Push-Off** | A more compact push-off is expected than for the leader role |
| **Side symmetry** (ASI, left vs. right) | More tolerance — the connection side makes followers structurally slightly more asymmetric |

**Calibration display:** In Follower mode, the Direction badge shows the measured foot angle (e.g. `⬅ BWD −4°`) and the Push-Off badge shows a strength value (e.g. `↗ PUSH 148`). These values are hidden in Leader mode to keep the UI clean.

---

## 4. The Step Badge Card

This is the **primary real-time feedback card**. It updates on every detected foot contact.

### What the display elements mean

| Element | What it tells you |
| :--- | :--- |
| **Direction badge** | ➡ FWD (foot angle +6° or more), ⬅ BWD (below −6°), or — when the angle is in the ambiguous zone |
| **Strike badge** (large coloured label) | Classification of that landing — see tables below |
| **ROLL badge** | Foot roll quality — how the forefoot lowers after heel contact (all levels) — [see section below](#roll-badge-all-levels) |
| **PUSH-OFF badge** | Push-off power of your trailing foot (Beginner: hidden) |
| **LOADING badge** | How smoothly (gradient) weight loaded onto the landing foot — [see section below](#loading-badge-adv-only) (Advanced only) |
| **DELAY badge** | How quickly weight committed relative to the beat tempo — [see section below](#delay-badge-int--adv) (Intermediate + Advanced) |
| **ANKLE ROLL badge** | Ankle shock absorption at landing (Advanced only) |

### How direction is determined

The system reads the direction from the tilt of your foot as it lands (shown as an angle). Direction is **reliable only at the extremes**:

| Direction badge | Foot angle at landing | Meaning |
| :--- | :--- | :--- |
| **➡ FWD** | +6° or more (toes up) | Heel clearly touched first |
| **—** (grey) | −6° to +5° | Ambiguous zone — foot too flat to tell direction |
| **⬅ BWD** | below −6° (toes down) | Ball of the foot clearly touched first |

When the foot angle is between −6° and +5°, the system cannot reliably determine direction. The direction badge shows — (grey). Use the camera view to check actual direction.

### HEEL zone badges (➡ FWD, +6° or more)

| Badge | Impact | What you did | Target |
| :--- | :--- | :--- | :--- |
| `HEEL STRIKE ✓` | light | Clean heel strike — controlled contact | Target for all forward walks and breaks |
| `HEEL SLAM ⚠` | hard | Hard heel impact — excessive landing force | Bend the knee on contact and soften the ankle |

### Ambiguous zone (—, −6° to +5°)

The foot is too flat to classify direction. The quality badge still fires:

| Badge | Impact | What it means |
| :--- | :--- | :--- |
| `SOFT ✓` | very light | Light, controlled landing — good absorption in this zone |
| `MODERATE` | moderate | Moderate impact — acceptable, but worth reducing |
| `HARD IMPACT ⚠` | hard | Heavy flat-foot landing — stomping pattern |
| `BRUSH+HEEL` | — | Ambiguous landing followed by heel-set shortly after — reclassified to ➡ FWD; correct technique confirmed |

Use the camera view to check actual direction when the direction badge shows —.

### TOE zone badges (⬅ BWD, below −6°)

| Badge | Impact | What you did | Target |
| :--- | :--- | :--- | :--- |
| `TOE-FIRST ✓` | light | Clean toe-ball contact — controlled landing | Target for all backward walks, anchors, extensions |
| `TOE JAM ⚠` | hard | Hard toe impact — excessive landing force | Moderate the extension; absorb through the ankle |

> **Note on early heel drops:** A backward step where the heel contacts before the toe will land in the ambiguous zone (—) rather than ⬅ BWD. If you see consistent SOFT/MODERATE/HARD IMPACT on what you believe are backward steps, your heel is contacting too early. Focus on sending the toe out first and keeping the ankle relaxed until the foot settles.

### ROLL badge (all levels)

The ROLL badge measures how smoothly the front of your foot lowers after your heel touches down — whether you control it down or let it drop.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `CLEAN ROLL ✓` ✅ | Smooth, controlled lowering of the foot — ankle stays engaged throughout | Good — maintain |
| `MODERATE ROLL` ⚠️ | Some unevenness as the foot rolls through | Focus on the heel → outside edge → ball sequence; keep the ankle lightly engaged |
| `SLAPPING` ❌ | The front of the foot drops uncontrolled after the heel lands | Roll through the foot deliberately: heel touches first, weight travels along the outside edge, then crosses to the ball with the ankle engaged throughout |

> **SLAPPING explained:** After your heel touches the floor, the front of your foot needs to be *lowered* actively, not allowed to fall. If you let it drop, it slaps down — you can usually hear it. This creates a hard impact, absorbs less shock, and softens your connection. The fix is a conscious heel → outside edge → ball sequence, keeping your ankle gently engaged the whole time.

> 📸 **[Screenshot: Step Badge card showing CLEAN ROLL ✓ badge in the lower badge row]**

---

## 5. Push-Off & Loading Badges

These badges appear in the lower badge row of the Step Card and update after each step. Push-Off is visible from **Intermediate** level; Loading and Ankle Roll from **Advanced** only.

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

Measures how quickly you committed your weight after foot contact, relative to the music tempo — so the same physical movement reads identically at slow and fast tempos. In WCS, weight ideally "floats" briefly and arrives after the foot (the characteristic hover feeling).

| Badge | What it means |
| :--- | :--- |
| `DELAYED ✓` ✅ | WCS-characteristic hover — weight arrives after the foot contacts |
| `QUICK` ⚠️ | Weight dropped immediately at contact — mechanical, not musical |
| `LATE` ⚠️ | Weight never fully committed — floating or incomplete transfer (ADV only) |

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

| Badge | What it means | Training implication |
| :--- | :--- | :--- |
| `OPTIMAL ROLL` ✅ | Smooth, grounded weight transfer | The characteristic WCS rolling connection |
| `HECTIC` ⚠️ | Rushed — one foot leaves before the other is secure | "Peel, don't lift" — roll through the foot before stepping |
| `SLUGGISH` ⚠️ | Prolonged double contact — hesitation or heavy stance (threshold rises automatically with slower music) | Commit to the weight shift earlier |

Watch this card during **triple steps and walks**. `HECTIC` on an anchor step often means you are rushing out of the anchor before building connection.

> 📸 **[Screenshot: Double Stance Overlap card showing OPTIMAL ROLL badge with the overlap percentage displayed]**

---

## 7. The Roll-off Symmetry & Smoothness Card

Visible at **Advanced** level only.

| Display | What it tells you | Green target |
| :--- | :--- | :--- |
| **Side Symmetry (ASI)** | Difference between left and right foot roll-off | `SYMMETRIC` — both sides even |
| **Smoothness** | Fluidity of ankle articulation across both feet | `SMOOTH` ✓ |

- `ASYMMETRIC` at the Side Symmetry display usually means one ankle is stiffer, or one side is compensating for an old injury.
- `ROUGH` Smoothness means your ankle movements are jerky. Slow the tempo and focus on rolling through the full foot rather than stepping flat.

---

## 8. The Grounding Card (ADV)

Visible only at **Advanced** level. The Grounding Card appears to the **left of the graph** (which is narrower in ADV mode to make room). It consolidates three grounding-quality signals into one panel.

### What the card contains

| Element | Requires | What it measures |
| :--- | :--- | :--- |
| **SDR badge** | Pelvis sensor | Shock attenuation through the leg chain — how well impact energy is absorbed from foot to hip |
| **SETTLE badge** | Pelvis sensor | Pelvis response timing after foot contact — how quickly the pelvis settles |
| **GND Score + bar** | Foot sensors | Combined grounding score (0–100) blending SDR + SETTLE + ROLL; bar turns green / yellow / red |

> When the pelvis sensor is offline, SDR and SETTLE badges are hidden. The GND Score still reflects ROLL quality from the foot sensors alone.

### SDR badge

SDR (Shock-absorbing Dynamic Response) measures how much of the foot-impact force is attenuated by the time it reaches the pelvis — i.e. how effectively the ankle, knee, and hip chain absorbs landing energy.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `ABSORBING ✓` ✅ | Effective shock attenuation — the leg chain is absorbing impact energy | Good — maintain soft, unlocked joints at landing |
| `PARTIAL SDR` ⚠️ | Some absorption, but impact is partially transmitted up the chain | Increase knee bend depth at heel contact; let the ankle give more on landing |
| `STIFF` ❌ | Minimal absorption — impact travels directly to the pelvis | Land with a softer knee; consciously absorb through the ankle and knee before the hip reacts |

### SETTLE badge

Measures whether the pelvis **actively cushions** the landing — whether the knee and hip chain absorbs the impact or lets it transmit upward. The target window **scales automatically with the music tempo**, so the same quality reads identically at slow and fast tempos.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `SETTLING ✓` ✅ | Leg chain actively cushions the impact — compliant knee and hip response | Maintain |
| `QUICK` ⚠️ | Pelvis dips before proper loading — joints too stiff to produce a measurable cushioning phase | Soften the knee on landing; let the leg chain absorb before committing weight |
| `SLOW` ⚠️ | Pelvis response is delayed — sluggish joint activation, impact absorbed passively | Engage the knee and hip actively at the moment of contact, not after |

### GND Score

A combined score from 0–100 blending all three grounding signals:

- **SDR** — shock attenuation quality (requires pelvis sensor)
- **SETTLE** — pelvis response timing (requires pelvis sensor)
- **ROLL** — forefoot roll quality from the Step Card

The bar below the score turns **green**, **yellow**, or **red** depending on your score.

> Use the GND Score as a single at-a-glance indicator during intensive drilling sessions. When it drops, check which component badge changed colour first.

---

## 9. The Pelvis & Torso Card (Optional Sensors)

The card in the **top-left slot** of the dashboard appears whenever the pelvis sensor **and/or** the torso sensor is powered on. When neither is online, that slot stays empty and the camera shows through. The pelvis rows and the torso rows appear independently, depending on which sensor is worn.

### Mounting the sensor

Clip the sensor to the **posterior waistband at the small of your back** (L5 / sacrum level), display facing away from your body. Centred on the spine is ideal, but left or right of centre by a few centimetres makes no practical difference. It should sit flat and snug — not dangling.

### Mounting verification

Once the pelvis sensor is online, the pelvis card appears at the top-left. If the **Hip Activation** badge shows `🌀 ACTIVE` continuously while you are standing still (no movement), the sensor is probably mounted incorrectly or is rotating. Re-clip it flat against the back with the display facing outward.

### Badge overview

| Badge row | Level | What is measured |
| :--- | :--- | :--- |
| **Hip Activation** | BEG+ | How much the pelvis is rotating during movement (yaw) |
| **Lateral Stability** | INT+ | Lateral sway of the pelvis during movement |
| **Pelvic Tilt** | INT+ | Forward/backward pelvis pitch — posture check (`ALIGNED` / `SLIGHT ARCH` / `LORDOSIS ⚠` / `TUCKED`) |
| **Hip-Foot Coupling** | INT+ | Whether hips initiate each step or follow the feet |
| **Vertical Bounce** | INT+ | How much vertical movement the pelvis generates |
| **Anchor Settle** | ADV | Quality of the pelvis settle shortly after each anchor step |
| **Hip Settle** | ADV | Whether the pelvis shifts into the standing hip after each anchor step (lateral tilt) |
| **Poise (upright)** | INT+ | Upper-body carriage — chest upright vs. leaning/slouching (needs the torso sensor) |
| **Level (side)** | ADV | Sideways tilt of the torso (needs the torso sensor) |
| **Top-Line Quiet** | ADV | How still the upper body stays during footwork (needs the torso sensor) |
| **Torso–Pelvis Stack** | ADV | Whether you lean as one piece or fold/break at the waist (needs torso **and** pelvis) |

> 📸 **[Screenshot: Pelvis card in the top-left slot showing all badge rows (Hip Activation through Anchor Settle) with sensor active]**

---

### Hip Activation

Measures how fast your hips rotate, taking the strongest moment over the last half-second.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `🌀 ACTIVE` ✅ | Strong hip rotation contributing to the movement | Maintain |
| `MODERATE` ⚠️ | Some rotation but hip contribution is limited | Wind the hip up before each step — rotation should start in the pelvis, not the ankle |
| `STIFF HIPS` ❌ | Pelvis barely rotating — movement driven by legs only | Drill isolated hip rotations first, then add feet. "Lead with the hip, not the heel." |

> In WCS, hip rotation should accompany or precede each step. `STIFF HIPS` is one of the most common beginner patterns and is invisible to foot sensors alone.

---

### Lateral Stability (INT+)

Measures how much your hips sway side to side over one second.

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
| `HIP LEADS` ✅ | Peak hip rotation occurred clearly before foot contact | Good initiation — hips are driving the step |
| `IN SYNC` ⚠️ | Hip peak and foot contact almost at the same time | Acceptable — try amplifying the pre-step hip "launch" |
| `HIP LAGS` ❌ | Hips rotating at or after foot contact | Legs are moving independently of the core. Slow down and practise initiating each walk from the hip, letting the foot follow |

---

### Vertical Bounce (INT+)

Measures how much your hips bounce up and down over one second.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `GROUNDED` ✅ | Minimal vertical movement | Good |
| `SLIGHT BOUNCE` ⚠️ | Some vertical oscillation | Maintain a light bend in the knees throughout; avoid extending to a straight leg during travel |
| `BOUNCY` ❌ | Significant up-down movement | Stay in your knees. Think: "stay low, stay connected." |

---

### Anchor Settle (ADV)

After every backward (anchor) step, the system opens a **tempo-adaptive measurement window** and evaluates three signals:

1. **Slowing down** — did your hips brake their forward/backward motion?
2. **Rotation settling** — did your hip rotation slow down after the step?
3. **Stillness** — how still were your hips in the second half of the window?

These three components are combined into a 0–100 score displayed in the badge.

| Badge | Score | What it means | How to improve |
| :--- | :--- | :--- | :--- |
| `ANCHORED (n)` ✅ | ≥ 50 | Strong braking + settling rotation + stable hold | Good — work on consistency across every anchor step |
| `SETTLING (n)` ⚠️ | 30–49 | Partial settle — one or two components weak | Identify the weak component using the tips below |
| `UNSTABLE (n)` ❌ | < 30 | Pelvis still moving or wobbling after the anchor | Focus on "sticking" the anchor — reach the end of the slot and hold |

**Diagnosing a low score:**
- **Low score despite clean foot technique** → the issue is pelvis, not foot angle. Work on the settle itself, not the step.
- **`HECTIC` Double Stance + low Anchor Settle** → you are leaving the anchor before the pelvis has stabilised.
- **`🌀 ACTIVE` hip + `UNSTABLE` anchor** → hips rotate well during travel but do not dampen at the anchor. Practise a deliberate "soft stop".

---

### Hip Settle (ADV)

Measures whether you "settle into the hip" after an anchor step — i.e. whether a brief sideways shift of the hips towards the standing leg happens and then holds. The system looks at your sideways hip movement within the same measurement window as Anchor Settle.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `HIP SETTLE ✓` | Clear lateral impulse early in the window, stable hold after — pelvis consciously settling onto the standing leg | Maintain |
| `SLIGHT SETTLE` | Small lateral impulse present but not pronounced | Let more weight consciously drop onto the standing leg and hold |
| `NO HIP SETTLE` | No lateral impulse — pelvis stays neutral after the anchor | Actively load the standing leg: after the anchor step, allow the hip to drop slightly towards the standing side |
| `OVERSWING ⚠` | Lateral impulse too strong — pelvis swings too far to the side | Moderate the movement; the lateral shift should be subtle, not a visible swing |

> **Note:** "Settling into the hip" is a stylistic element — some teaching styles emphasise it strongly, others less so. In WCS, the lateral pelvic movement is intentionally subtler than in Latin dance: the goal is a "grounded arrival", not a visible swing. This badge provides information, not a verdict. If your teacher does not want a lateral settle, disregard this badge.
>
> **Thresholds note:** These values are based on biomechanical reference data and can be adjusted after a first test session with the pelvis sensor.

---

### Torso — Poise (optional torso sensor)

A second optional sensor worn **high on the upper back** (base of the neck) reads how your upper body carries itself — something the foot and pelvis sensors cannot see. Clip it flat against a tight layer, display facing outward, top edge up. When you press `📐 ZERO` **in your natural dance-ready stance** (the same one that calibrates your feet and pelvis — one press zeroes all sensors), the system learns your working posture as neutral, so afterwards it only shows how far you drift from it — forward *or* back.

These rows appear in the same top-left card as the pelvis rows.

| Badge | What it means | How to improve |
| :--- | :--- | :--- |
| `UPRIGHT ✓` | Chest stacked over the hips — good carriage | Maintain |
| `SLIGHT LEAN` | Upper body starting to drop forward | Grow tall through the crown of the head; lift the sternum |
| `SLOUCHING ⚠` | Chest collapsing forward — rounding the upper back | Reset your posture: ribs over hips, shoulders back and down |
| `LEANING BACK` | Upper body tipped behind your base | Bring the chest back over your centre |

- **`LEVEL` / `SLIGHT TILT` / `TILTED`** — how level your shoulders stay side to side. Persistent tilt usually means you drop one shoulder on turns or weight shifts.
- **`QUIET` / `SOME MOTION` / `RESTLESS`** — how still the upper body stays while your feet work. `RESTLESS` means fast footwork is leaking up into the shoulders instead of being absorbed in the core. Aim for a quiet top line over busy feet.
- **`STACKED` / `OPENING` / `PIKING`** (needs the pelvis sensor too) — separates leaning as one piece from breaking at the waist. `PIKING` means your chest folds forward while your hips stay back — a common way to *look* connected while actually collapsing. Keep the torso and pelvis moving as one column.

> **Poise is a refinement, not a beginner cue.** `Poise` appears from Intermediate; the other three torso rows are Advanced. If you only wear the torso sensor (no pelvis), the poise, level and top-line rows still work — only the Torso–Pelvis Stack needs both.
>
> **Thresholds note:** All torso thresholds are provisional starting points, to be calibrated against your own recordings.

---

## 10. Training by Skill Level

### Beginner — use `👤 BEG`

**One focus: heel vs. toe contact.**

1. Walk forward. Does the badge say `HEEL STRIKE ✓`? If not — your heel is not contacting first. Lift the heel slightly more before the foot lands.
2. Walk backward. Does the badge say `TOE-FIRST ✓`? If not — send your toe out first. If you see — (ambiguous) on backward steps, your heel is contacting the floor before the toe.
3. If you hear the **1200 Hz beep**, stop and slow down. That is a `HEEL SLAM ⚠`, `TOE JAM ⚠`, or `HARD IMPACT ⚠` — too much landing force.
4. Practise at a slow tempo until `HEEL STRIKE ✓` and `TOE-FIRST ✓` appear consistently. Only then increase speed.

**With pelvis sensor:** Watch **Hip Activation** only. If `STIFF HIPS` appears consistently, your legs are moving without your core engaging.

### Intermediate — use `🏃 INT`

**Two focuses: technique consistency + weight transfer timing.**

1. Forward walks → aim for consistent `HEEL STRIKE ✓`. Check the ROLL badge: `CLEAN ROLL ✓` means you are controlling the front of your foot all the way down.
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
6. Use the **Roll-off Dynamics graph** to compare how fast each foot rotates as it rolls.

**With pelvis sensor:** Focus on **Anchor Settle** as your measure of anchor quality. Run a full 8-count basic and check the score after each anchor step. Use the **GND Score** on the Grounding Card as a single at-a-glance grounding indicator — when it drops, check which component badge (ROLL, SDR, or SETTLE) changed colour first.

---

## 11. Common Problems and How to Fix Them

| What you see | Root cause | Fix |
| :--- | :--- | :--- |
| `HEEL SLAM ⚠` on forward walks | Heel contacting hard — insufficient knee or ankle absorption | Slow down. Bend the knee more on contact and soften the ankle. |
| `SLAPPING` on forward steps | The front of the foot is dropping uncontrolled after the heel lands | Roll deliberately: heel → outside edge → ball with the ankle engaged throughout. The front of the foot must be *lowered*, not allowed to fall. |
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

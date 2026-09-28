# WCS-Verbindungs-Dashboard — Technische Referenz

Dieses Dokument beschreibt die mathematischen und biomechanischen Grundlagen des **Verbindungs-Dashboards** (`/`) — Verbindungskraftmessung, Jerk/Führungsqualität und Fußabroll-Visualisierung im Partner-Graphen.

> **Zielgruppe:** Entwickler und technisch interessierte Trainer. Für den tänzergerichteten Leitfaden siehe [dancer_guide_partner.md](dancer_guide_partner.md).

> Systemarchitektur, Sensorfusion, ESP-NOW-Protokoll und die vollständige Schritterkennungs-Pipeline sind unter [Solo-Dashboard — Technische Referenz](solo_explanations.md) beschrieben.

---

## 1. Fußabrollqualität (Partner-Graph)

Die Cyan- (links) und Magenta- (rechts) Linien im kombinierten Analyse-Graphen zeigen einen kontinuierlichen Qualitätswert aus Aufprallbeschleunigung und Rotationsartikulation:

$$\text{FootQuality} = \frac{\lvert \omega_\text{pitch} \rvert}{1{,}0 + \max(0,\; \lvert a_z \rvert - 1{,}0) \times 2{,}0}$$

> Der Nenner bestraft nur Aufpralle oberhalb von 1 g — das Anheben des Fußes (Werte unter 1 g) verschlechtert den Qualitätswert nicht.

Ein **Stampf-Fehler** (vertikaler Cyan/Magenta-Marker) wird ausgelöst, wenn beide Bedingungen gleichzeitig gelten:

`|accel_z| > ACCEL_MAX`  und  `|gyro_x| < GYRO_MIN`

Die vollständige biomechanische Herleitung dieser Schwellenwerte (Rocker-Modell, Plantarflexionsphasen, empirische Analyse) ist unter [solo_explanations.md](solo_explanations.md) beschrieben.

---

## 2. Verbindungskraft & Gewichtsverteilung

*   **Gemessener Wert:** `weight` — Druck-/Zugkraft in Gramm (g) vom Dehnungsmessstreifen zwischen den beiden Tänzern.
*   **Biomechanische Bedeutung:** Repräsentiert die physische Spannung und Kompression, die durch den Verbindungsrahmen zwischen den Tanzpartnern übertragen wird.
    *   **Positive Werte (> 0 g):** Kompression (drücken / aufeinander zugehen).
    *   **Negative Werte (< 0 g):** Zugspannung (ziehen / voneinander wegstrecken).

---

## 3. Hand-Jerk & Führungsgeschmeidigkeit

Jerk (Ruck) bezeichnet die Änderungsrate der Beschleunigung — abrupte Spitzen korrelieren direkt mit ruckartiger, unangekündigter Umlenkung oder Ziehen in der Partnerverbindung.

*   **Berechnungen:**
    1.  **Gewichts-Änderungsrate (Kraftrate):** $\text{forceRate} = \dfrac{\lvert \text{weight}_\text{current} - \text{weight}_\text{previous} \rvert}{\Delta t}$ — in g/s.
    2.  **Räumliche Beschleunigungsrate (Bewegungsrate):**
        *   $\Delta a_x = a_{x,\text{current}} - a_{x,\text{previous}}$, ebenso $\Delta a_y$, $\Delta a_z$
        *   $\text{motionRate} = \dfrac{\sqrt{(\Delta a_x)^2 + (\Delta a_y)^2 + (\Delta a_z)^2}}{\Delta t}$ — in g/s.
    3.  **Normierter Hand-Jerk-Index:** $\text{jerkIndex} = \min\!\left(1{,}0;\; \dfrac{\text{forceRate}}{2000} \times 0{,}5 + \dfrac{\text{motionRate}}{10} \times 0{,}5\right)$

    > Beide Terme werden *vor* der Kombination in Raten pro Sekunde umgerechnet, sodass die Einheiten kohärent sind (die frühere Form `ΔW/50 + accelJerk×15` mischte Gramm mit g und wird nicht mehr verwendet). Als Vollausschlag gelten 2000 g/s Kraftrate und 10 g/s Bewegungsrate, jeweils gleich gewichtet.

*   **Biomechanische Bedeutung:** Hohe Werte weisen auf abrupte Stöße, plötzliche Rucke oder Mikrostottern im Führungs-/Folge-Rahmen hin — kein kontinuierlicher, flüssiger Impulstransfer.

---

## 4. Schwellenwerte

| Parameter | Wert | Herleitung / Begründung |
| :--- | :--- | :--- |
| **`GYRO_MIN`** | $80{,}0\,\text{deg/s}$ | Empirisch aus Tests sauberer Fußartikulierung abgeleitet; Werte darunter fehlt die notwendige Rollbewegung. |
| **`ACCEL_MAX`** | $1{,}5\,g$ | Typische Grenze zwischen gedämpften Schritten und harten Aufprallstößen. |
| **Jerk-Peak-Erkennung** | firmwareseitig bei nativer Abtastrate | Die Firmware markiert Jerk-Peaks (`serverJerk`), statt sie im Browser neu abzuleiten; die gelbe Graph-Linie nutzt den normierten `jerkIndex` aus §3. |
| **Schritt-Jerk (SLAM/JAM/HARD)** | $136$ intern (= $34\,\text{g/s}$ angezeigt) Basis; kraft-gegated ×1,8, gedeckelt bei $200$ (= $50\,\text{g/s}$) | Angehoben vom ursprünglichen $88$ (= $22\,\text{g/s}$) nach Partner-Validierung — in den Fußsensor übertragene Verbindungskraft erzeugte falsche Hard-Impact-Alarme. Siehe §7. |
| **Diagramm-Skalierung** | $-10000\,\text{g}$ bis $+10000\,\text{g}$ | Von ±5000 g erweitert, nachdem Kraftspitzen über 6 kg auf All-Star-Niveau am alten Bereich clippten. |

---

## 5. Visualisierungsarchitektur

Das Frontend stellt Echtzeit-Daten über HTML5-Canvas-Elemente dar, die über `requestAnimationFrame`-Schleifen mit 20 ms Abfrageintervall aktualisiert werden:

1.  **Verbindungskraft-Diagramm (`graph_kraft`):**
    *   Echtzeit-Gewichtskurven über ein gleitendes 10-Sekunden-Fenster.
    *   Dynamische Linienfärbung: **Grün** (Kompression / positive Last) und **Rot** (Zugspannung / negative Last) basierend auf Nulldurchgangslogik.
2.  **Kombiniertes Analyse-Diagramm (`graph_kombi`):**
    *   **Fußqualitätskurven:** Linker Fuß (Cyan), rechter Fuß (Magenta) — kontinuierlicher Qualitätswert aus der Formel in §1. Die Kurve nutzt die volle Canvas-Höhe: `Q = 0` liegt am unteren Rand, `Q = 150` an der horizontalen Mittellinie, `Q = 300` am oberen Rand. Die Mittellinie ist damit eine aussagekräftige Schwelle: Kurven in der oberen Hälfte = überdurchschnittliche Abrollqualität; untere Hälfte = passiv oder impact-dominiert.
    *   **Jerk-Verfolgungskurve:** Gelbe Linie — Führungshärte-/Jerk-Profil, auf Canvas-Koordinaten skaliert.
    *   **Fehlermarkierungen:** Vertikale Vollhöhen-Linien bei Schwellenwertüberschreitung:
        *   **Cyan / Blau:** Linker Fuß — Aufprall-/Artikulationsfehler.
        *   **Magenta / Lila:** Rechter Fuß — Aufprall-/Artikulationsfehler.
        *   **Yellow / Orange:** Hand-Jerk / Führungshärte-Spitze.

> 📸 **[Screenshot: Kombiniertes Analyse-Diagramm mit Cyan- und Magenta-Abrollqualitätslinien, gelber Jerk-Kurve und vertikalen Fehler-Markierungen]**

---

## 6. Pelvis-Metriken (Partner-Dashboard)

Wenn der Pelvis-Sensor (`foot_id = 4`) online ist, erscheinen sechs Badge-Metriken in der Status-Leiste. Alle sechs sind immer aktiv — kein Level-Gate.

| Badge | Signal | Schwellenwerte |
| :--- | :--- | :--- |
| **Hip Activation** | `gYaw`-Peak über 500 ms, IIR-geglättet | ≥ 45°/s → ACTIVE ✅ \| 25–45°/s → MODERATE ⚠ \| < 25°/s → STIFF HIPS ❌ |
| **Lateral Stability** | Varianz der lateralen Beckenbeschleunigung über 1 s | < 0,004 → STABLE ✅ \| 0,004–0,015 → SLIGHT SWAY ⚠ \| ≥ 0,015 → LATERAL SWAY ❌ (kraft-gegated → SLIGHT SWAY bei \|Kraft\| > 1500 g) |
| **Hip-Foot Coupling** | Vorlaufzeit: Peak-Hüftrotation → Fußkontakt | > 100 ms → HIP LEADS ✅ \| 40–100 ms → IN SYNC ⚠ \| Hüfte nach Fuß → HIP LAGS ❌ |
| **Vertical Bounce** | Varianz der vertikalen Beckenbeschleunigung (Schwerkraft entfernt) über 1 s | < 0,006 → GROUNDED ✅ \| 0,006–0,038 → SLIGHT BOUNCE ⚠ \| ≥ 0,038 → BOUNCY ❌ |
| **Anchor Settle** | Gewichteter Score (0–100) über tempo-adaptives Fenster nach jedem Rückwärtsschritt | ≥ 42 → ANCHORED ✅ \| 30–41 → SETTLING ⚠ (Score eingeblendet) \| < 30 → UNSTABLE ❌ |
| **Hip Settle** | Peak der lateralen Beckenbeschleunigung (`earlyLatPeak`) in der ersten Fensterhälfte | `earlyLatPeak` > 0,30 g → OVERSWING ⚠ \| 0,10–0,30 g + späte Varianz < 0,015 → HIP SETTLE ✓ ✅ \| 0,05–0,10 g → SLIGHT SETTLE ⚠ \| ≤ 0,05 g → NO HIP SETTLE ❌ |

> 📸 **[Screenshot: Statusleiste im Partner-Dashboard mit allen sechs Becken-Metriken (Hüftaktivierung bis Hip Settle) gleichzeitig aktiv]**

> **Hip Activation — tempoabhängige Schwellenwerte:** Die oben angezeigten Werte (45 °/s / 25 °/s) sind Referenzwerte bei 500 ms/Schritt. Zur Laufzeit skalieren die Schwellenwerte mit dem aktuellen Schrittintervall: `scaleFactor = 500 / max(400, stepDurationMs)`. Effektive Schwellenwerte: ACTIVE ≥ `round(45 × scaleFactor)` °/s, MODERATE ≥ `round(25 × scaleFactor)` °/s. Bei langsamem Tempo (700 ms/Schritt, scaleFactor ≈ 0,71): ACTIVE ≥ 32 °/s, MODERATE ≥ 18 °/s. Bei schnellem Tempo (400 ms/Schritt, scaleFactor = 1,25): ACTIVE ≥ 56 °/s, MODERATE ≥ 31 °/s. Die ACTIVE-Basis wurde für den Partner-Kontext von 60 auf 45 °/s gesenkt — der Slot dämpft die Rotationsgeschwindigkeit auch bei echter aktiver Hüftnutzung.

### Anchor Settle — Details

**Sammelfenster** (wie lange Becken-Samples erfasst werden): `anchorWindowMs = min(500, max(280, stepDurationMs))`.

**Auswerte-Deadline** (wann der Score berechnet wird und das Badge feuert): `anchorSettleWindowMs = min(400, max(280, round(stepDurationMs × 0,55)))` nach dem letzten Rückwärtsschritt. Diese tempo-adaptive Deadline wurde vom ursprünglichen festen 700-ms-Fenster verkürzt: Bei 90 BPM (≈667 ms/Beat) schloss das alte Fenster einen ganzen Beat später und bewertete die Stabilität, während der Tänzer bereits die nächste Figur begonnen hatte — was falsche `UNSTABLE`-Anzeigen erzeugte. Der Faktor 0,55 schließt das Fenster ≈300 ms vor Count 1.

Score-Zusammensetzung:

$$\text{score} = \text{decelScore} \times 0{,}35 + \text{yawDampScore} \times 0{,}35 + \text{stabilScore} \times 0{,}30$$

* **decelScore** — sagittale Dezeleration: früher Mittelwert > später Mittelwert (Becken bremst Vorwärtsmomentum ab)
* **yawDampScore** — Yaw-Dämpfung: Yaw-Peak frühe Hälfte > späte Hälfte (Rotation stoppt nach der Landung)
* **stabilScore** — Spät-Phasen-Stabilität: niedrige Varianz von `|gYaw|` in der zweiten Fensterhälfte

---

## 7. Verbindungskraft-Gates (nur Partner-Dashboard)

Das Partner-Dashboard liest die Live-Verbindungskraft (`currentW`, Gramm) im Moment der Badge-Auswertung und nutzt sie, um kraft-induzierte Falschalarme zu unterdrücken. Diese Gates existieren **nicht** im Solo-Dashboard, wo es keine Verbindungskraft gibt. Alle wurden nach der Validierung eines All-Star-Partnervideos hinzugefügt, in dem >90 % der Warn-Badges mit Kraftspitzen statt echten Technikfehlern korrelierten.

| Badge | Gate-Bedingung | Verhalten bei aktivem Gate |
| :--- | :--- | :--- |
| **Schritt-Jerk** (SLAM/JAM/HARD) | `\|currentW\| > 2000 g` | Schwelle ×1,8 (136 → 245 intern), **hart gedeckelt bei 200** (= 50 g/s angezeigt). Echtes hartes Aufstampfen über 50 g/s löst weiterhin aus. |
| **Lateral Stability** | `\|currentW\| > 1500 g` | `LATERAL SWAY` (rot) herabgestuft auf `SLIGHT SWAY` (gelb) |
| **Delay** (QUICK/LATE) | `\|currentW\| > 1500 g` | Unabhängig vom gemessenen Verhältnis auf `DELAYED ✓` gesetzt |

**Warum das Gate auf den Betrag statt die Richtung reagiert:** In der Validierung korrelierten sowohl `QUICK`- als auch `LATE`-Falschalarme mit betragsmäßig hoher Verbindungskraft (überwiegend großen Werten auf einer Seite der Null), unabhängig von der Druck-/Zug-Richtung. Hohe Kraft belastet den Fußsensor und verzerrt die Gewichtsverlagerungs-Rampe in beide Richtungen unvorhersehbar — deshalb reagiert das Delay-Gate auf `|currentW|` (Betrag) statt auf das Kraftvorzeichen.

**Nicht gegated:** `OVERSWING` braucht kein Kraft-Gate — in der Validierung erzeugte es null Falschalarme selbst bei Kraftspitzen über 6 kg; die Schwelle `earlyLatPeak > 0,30 g` ist bereits robust. `BOUNCY` ist nicht kraft-gegated, aber seine Schwelle wurde angehoben (0,020 → 0,038), um den Anstieg der vertikalen Varianz durch Rumpfanspannung gegen Partnerkraft aufzufangen.

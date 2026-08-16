# WCS Dance Master Sensors — Tänzer-Leitfaden: Partner-Dashboard

> Dieser Leitfaden behandelt das Partner-Dashboard (`/`) — die Ansicht für einen Coach oder Tanzpartner, der von der Seite zuschaut.  
> Die Solo-Trainingsansicht mit stufengesteuertem Feedback ist unter [dancer_guide_solo.md](dancer_guide_solo.md) beschrieben.

---

## 1. Erste Schritte

1. Öffne `http://192.168.4.1/` (Root-URL — kein `/solo`) auf einem zweiten Handy oder Tablet, während der Tänzer das Solo-Dashboard auf seinem eigenen Gerät verwendet.
2. **Tippe auf `START CAM`**, um die Kamera-Einblendung zu aktivieren. Richte das Gerät so aus, dass der Tänzer im Bild ist.
3. **Tippe einmal auf `ZERO`**, während der Tänzer in einer neutralen Haltung steht. Damit werden die Fußwinkel-Offsets tariert und gleichzeitig der Hardware-Nullpunkt der Kraftwaage zurückgesetzt.
4. Das Dashboard aktualisiert sich in Echtzeit — keine weitere Einrichtung erforderlich.

> Das Partner-Dashboard läuft auf demselben M5-Controller wie das Solo-Dashboard. Beide Ansichten empfangen dieselben Live-Sensordaten.

---

## 2. Bildschirmaufbau

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

**Zahl oben links** (`114 g`, `−39 g`, `— g`): aktueller Verbindungskraft-Messwert vom Hand-Sensor. Grün = Zug/Spannung, Rot = Druck/Kompression, grauer Strich = Sensor offline.

> 📷 **Screenshot-Platzhalter — vollständige Dashboard-Übersicht**  
> *(Ersetzen durch: Vollbild-Foto des Partner-Dashboards mit allen aktiven Sensoren, sichtbarer Kamera-Einblendung im Hintergrund, Statusleiste mit Schritt- und Becken-Badges)*

---

## 3. Verbindungskraft-Diagramm (Oberes Diagramm)

Zeigt die vom Dehnungsmessstreifen zwischen den beiden Tänzern gemessene Kraft, skaliert auf ±4,0 kg.

| Linienfarbe | Bedeutung |
| :--- | :--- |
| **Grün** (oberhalb der Mitte) | Leader zieht — Spannung in der Verbindung |
| **Rot** (unterhalb der Mitte) | Leader drückt — Druck in der Verbindung |
| **Flache Linie in der Mitte** | Neutral — keine messbare Verbindungskraft |

**Worauf zu achten ist:**

- **Ruhige Linie mit geringer Amplitude nahe null** → leichte, reaktionsfähige Verbindung. Ideal.
- **Anhaltende grüne Erhöhung** → Leader hält durchgehend Spannung — prüfen, ob die Follow genug Bewegungsfreiheit hat.
- **Scharfe Spitzen** → plötzliche Kraftänderungen — ruckartiges Führen oder abruptes Stoppen. Zum Bestätigen mit der gelben Jerk-Linie im unteren Diagramm vergleichen.
- **Abwechselnd grün/rot** → Leader hält keine gerichtete Absicht — Wechsel zwischen Druck und Zug innerhalb derselben Phrase.

> 📷 **Screenshot-Platzhalter — Verbindungskraft-Diagramm: ruhiges Führen**  
> *(Ersetzen durch: Screenshot einer glatten, grünen Linie mit geringer Amplitude nahe der Mitte — gute Verbindungsqualität)*

> 📷 **Screenshot-Platzhalter — Verbindungskraft-Diagramm: ruckartiges Führen**  
> *(Ersetzen durch: Screenshot mit wiederholten roten/grünen Spitzen — zum Vergleich mit gelben Jerk-Spitzen im unteren Diagramm zum selben Zeitpunkt)*

---

## 4. Kombiniertes Analyse-Diagramm (Unteres Diagramm)

Drei überlagerte Datenströme in einer einzigen Zeichenfläche:

### Cyan-Linie — Abrollqualität linker Fuß

Abgeleitet aus `|gyroPitch| / (1 + impactDev × 2)`. Ein höherer Wert bedeutet, dass der Fuß mit geringem Aufprall sauber abgerollt ist. Die Linie steigt, wenn der linke Fuß mit guter Technik aufgesetzt wird, und fällt bei schweren, flachen Aufprallen.

### Magenta-Linie — Abrollqualität rechter Fuß

Gleiche Formel wie Cyan, für den rechten Fuß.

**Beide Linien zusammen lesen:**
- Beide Linien verlaufen ähnlich auf mittlerer Höhe → symmetrische, konsistente Technik.
- Eine Linie konstant niedriger → dieser Fuß setzt mit mehr Wucht auf oder rollt weniger sauber ab als der andere.
- Beide Linien nahe null → Fußsensoren offline oder Tänzer steht still.

### Gelbe Linie — Hand-Jerk-Index

Eine Kombination aus Kraftänderungsrate und Beschleunigungsbetrag der Hand. Steigt bei plötzlichen Führungsimpulsen, fällt bei gleichmäßiger Bewegung.

- **Gelb nahe null** → gleichmäßiges Führen.
- **Gelbe Spitzen** → abrupte Kraft- oder Beschleunigungsänderungen in der Handverbindung.

### Vertikale Markierungslinien

| Farbe | Bedeutung |
| :--- | :--- |
| **Cyan vertikaler Balken** | Fehler linker Fuß: schwerer Aufprall ohne Abrollbewegung |
| **Magenta vertikaler Balken** | Fehler rechter Fuß: schwerer Aufprall ohne Abrollbewegung |
| **Roter vertikaler Balken** | Fehler beider Füße gleichzeitig |
| **Gelbe gestrichelte Linie** | Jerk-Spitze von der Firmware erkannt |

Fehler-Markierungen werden ausgelöst, wenn die Aufprallbeschleunigung 1,5 g übersteigt und die Abrollbewegung weniger als 80°/s beträgt — ein Stampfmuster. Mehrere Markierungen hintereinander auf derselben Seite weisen auf ein wiederkehrendes Technkproblem an diesem Fuß hin.

> 📷 **Screenshot-Platzhalter — kombiniertes Analyse-Diagramm: asymmetrische Fußqualität**  
> *(Ersetzen durch: Screenshot, auf dem die Cyan-Linie über eine vollständige Phrase deutlich höher liegt als die Magenta-Linie — schwächerer rechter Fuß sichtbar)*

> 📷 **Screenshot-Platzhalter — kombiniertes Analyse-Diagramm: Jerk + Fehlerkorrelation**  
> *(Ersetzen durch: Screenshot mit einer gelben Spitze und einem magenta/roten vertikalen Fehler-Marker zum selben Zeitpunkt)*

---

## 5. Schritt-Badges (Statusleiste)

Wird bei jedem erkannten Fußkontakt aktualisiert. Verwendet dieselbe Klassifizierung wie das Solo-Dashboard auf Anfänger-Niveau — keine Stufenauswahl in der Partner-Ansicht erforderlich.

| Element | Bedeutung |
| :--- | :--- |
| **➡ FWD L / R** | Vorwärtsschritt, linker oder rechter Fuß |
| **⬅ BWD L / R** | Rückwärtsschritt, linker oder rechter Fuß |
| **↔ FLAT L / R** | Unklare Landung — zu flach für eine Richtungsklassifizierung |
| **θ Winkel** | Fußneigung beim Aufsetzen (positiv = Ferse hoch, negativ = Zehe runter) |
| **Schritt-Badge** | Klassifizierung der Landung — siehe Tabelle unten |
| **Verzögerungs-Badge** | Temponormierte Zeitgebung der Gewichtsverlagerung — siehe Tabelle unten |

### Schritt-Badge Referenz

| Badge | Richtung | Winkel | Bewertung |
| :--- | :--- | :--- | :--- |
| `OPTIMAL HEEL` ✅ | Vorwärts | +10° bis +35° | Korrekter Fersen-zuerst-Kontakt |
| `HEEL SPIKE` ⚠️ | Vorwärts | > +35° | Übermäßiger Fersenwinkel |
| `OPTIMAL TOE` ✅ | Rückwärts | −5° bis −20° | Korrekter Zehenball-Kontakt |
| `ANCHOR SETTLE` ✅ | Rückwärts | nahe flach + geringer Aufprall | Fuß setzt im Anchor ein — Gewicht wird neu verteilt |
| `HEEL DROP` ⚠️ | Rückwärts | > +5° | Ferse berührt vor dem Zeh bei einem Rückwärtsschritt |
| `HEEL LANDING!` ❌ | Rückwärts | > +10° | Eindeutiger Fersen-zuerst-Kontakt bei einem Rückwärtsschritt |
| `FLAT-FOOT!` ❌ | Unklar | −5° bis +5° | Flache Landung ohne Klassifizierung |
| `BALL-STEP` | Vorwärts | < −5° | Zehenball-zuerst bei einem Vorwärtsschritt |

> 📷 **Screenshot-Platzhalter — Statusleiste: Schritt-Badges**  
> *(Ersetzen durch: Nahaufnahme der Statusleistenzeile, z. B. `⬅ BWD R  −7°  OPTIMAL TOE` mit ausgeblendeter Becken-Zeile)*

### Verzögerungs-Badge Referenz

Der Verzögerungs-Badge misst, wie schnell das Gewicht nach dem Fußkontakt verlagert wurde, ausgedrückt als Anteil des Schrittintervalls — er ist daher **automatisch an das Musiktempo angepasst**. Bei einem langsamen und einem schnellen Lied wird für dieselbe Bewegungsqualität dasselbe Badge angezeigt.

Die Schwellenwerte unterscheiden sich je nach Richtung, da eine Rückwärtslandung (Zehe zuerst) von Natur aus mehr Einschwingzeit benötigt als eine Vorwärtslandung (Ferse zuerst).

| Badge | Vorwärtsschritt | Rückwärtsschritt | Bewertung |
| :--- | :--- | :--- | :--- |
| `DELAYED ✓` ✅ | 12–38 % des Beats | 18–50 % des Beats | Charakteristisches WCS-„Hover" — Gewicht kommt nach dem Fuß |
| `QUICK` ⚠️ | < 12 % | < 18 % | Gewicht sofort beim Aufprall verlagert — mechanisch, nicht musikalisch |
| `LATE` ⚠️ | > 38 % | > 50 % | Gewicht nie vollständig angekommen — schwebendes oder unvollständiges Transfer |

> **Coaching-Tipp:** `QUICK` bei jedem Anchor-Schritt ist der häufigste Befund auf Newcomer-/Intermediate-Niveau. Der Tänzer tritt zurück, verlagert aber sofort das Gewicht und verliert damit die Dehnung in der Verbindung. Auf `QUICK` in der Statusleiste achten und als Cue geben: *„Tritt zurück und atme, bevor du landest."*

> 📷 **Screenshot-Platzhalter — Verzögerungs-Badge: DELAYED ✓ im Anchor**
> *(Ersetzen durch: Statusleiste mit `⬅ BWD R  −12°  OPTIMAL TOE  DELAYED ✓` — alles grün, gute Technik)*

--- (Statusleiste — erscheint wenn der Sensor online ist)

Alle fünf Becken-Metriken werden gleichzeitig angezeigt, wenn der Becken-Sensor aktiv ist — es gibt keine Stufenauswahl in der Partner-Ansicht.

Vollständige Beschreibungen der einzelnen Badges sind unter [dancer_guide_solo.md — Abschnitt 8](dancer_guide_solo.md#8-the-pelvis-card-optional-sensor) zu finden.

### Kurzübersicht

| Badge | Grün | Gelb | Rot |
| :--- | :--- | :--- | :--- |
| **Hüftaktivierung** | `🌀 ACTIVE` (≥60°/s) | `MODERATE` (25–60°/s) | `STIFF HIPS` (<25°/s) |
| **Seitliche Stabilität** | `STABLE` | `SLIGHT SWAY` | `LATERAL SWAY` |
| **Hüft-Fuß-Kopplung** | `HIP LEADS` (>100 ms vor dem Fuß) | `IN SYNC` (40–100 ms) | `HIP LAGS` (<40 ms) |
| **Vertikales Wippen** | `GROUNDED` | `SLIGHT BOUNCE` | `BOUNCY` |
| **Anchor Settle** | `ANCHORED (n)` (≥60) | `SETTLING (n)` (30–59) | `UNSTABLE (n)` (<30) |

> 📷 **Screenshot-Platzhalter — Statusleiste: Becken-Badges aktiv**  
> *(Ersetzen durch: Nahaufnahme der vollständigen Statusleiste mit beiden Zeilen sichtbar — Schritt-Zeile + PELVIS:-Zeile mit allen 5 Badges in verschiedenen Farben)*

---

## 7. Schaltflächen

| Schaltfläche | Funktion |
| :--- | :--- |
| **START CAM / FLIP CAM** | Aktiviert die Kamera-Einblendung; erneutes Tippen wechselt zwischen Front- und Rückkamera |
| **EXIT** | Beendet den Vollbildmodus |
| **FREEZE** | Pausiert beide Diagramme zur Inspektion — nützlich, um einen Moment nach einem Durchlauf zu besprechen |
| **ZERO** | Tariert Fußwinkel-Offsets (setzt die Richtungs-/Winkel-Basislinie zurück) UND löst Hardware-Tarierung der Kraftwaage aus. Tippen, während der Tänzer in neutraler Haltung steht. |
| **REC START / STOP** | Nimmt den vollständigen Bildschirm auf (Diagramme + Kamera-Einblendung + Audio) und speichert ihn als `.webm`-Datei, die beim Stoppen automatisch heruntergeladen wird |

---

## 8. Coaching-Anwendungsfälle

### Lead-Qualität in Echtzeit ablesen

Beobachte das **Verbindungskraft-Diagramm**, während das Paar tanzt. Ein gutes Führen erzeugt eine ruhige Linie mit kurzen, gezielten Spitzen — die Spannung steigt, wenn der Leader initiiert, kehrt nahe null zurück, wenn die Follow übernommen hat. Anhaltende Erhöhung oder wiederholte Spitzen deuten darauf hin, dass der Leader die Follow zwischen den Moves nicht loslässt.

### Den schwächeren Fuß identifizieren

Vergleiche die **Cyan- und Magenta-Linien** im kombinierten Analyse-Diagramm über eine vollständige Phrase. Wenn eine Linie konstant niedriger verläuft, ist das der Fuß, den es zu trainieren gilt. `FREEZE` nach einem Durchlauf verwenden und die Spitzenhöhen visuell vergleichen.

> 📷 **Screenshot-Platzhalter — FREEZE: Cyan vs. Magenta vergleichen**  
> *(Ersetzen durch: Screenshot nach dem Tippen auf FREEZE, Diagramme pausiert, mit sichtbarem Unterschied in der Spitzenhöhe zwischen den beiden Fußqualitätslinien)*

### Hüftinitiierung prüfen

Der **Hüft-Fuß-Kopplungs**-Badge wird bei jedem Schritt ausgelöst. Konstantes `HIP LAGS` bedeutet, dass der Tänzer mit seinen Beinen läuft und nicht aus dem Kern — die häufigste technische Schwäche auf Newcomer- bis Intermediate-Niveau.

### Anchor-Qualität unter Belastung

Nach jedem Anchor zeigt der **Anchor Settle**-Badge einen Wert von 0–100. Ein Wert konstant unter 60 über einen ganzen Song bedeutet, dass das Becken des Tänzers sich noch bewegt, nachdem der Anchor-Schritt gelandet ist. Niedrige Werte gegen Ende eines Songs (aber nicht am Anfang) weisen auf einen erschöpfungsbedingten Anchor-Zusammenbruch hin.

### FREEZE für die Besprechung nutzen

Musik stoppen, unmittelbar nach einem bemerkenswerten Moment auf **FREEZE** tippen. Die Diagramme halten die letzten 10 Sekunden fest. Den Tänzer durch das Bild führen, bevor das Einfrieren aufgehoben wird.

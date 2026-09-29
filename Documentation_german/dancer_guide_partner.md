# WCS Dance Master Sensors — Tänzer-Leitfaden: Partner-Dashboard

> Dieser Leitfaden behandelt das Partner-Dashboard (`/`) — die Ansicht für einen Coach oder Tanzpartner, der von der Seite zuschaut.  
> Die Solo-Trainingsansicht mit stufengesteuertem Feedback ist unter [dancer_guide_solo.md](dancer_guide_solo.md) beschrieben.  
> **Grounding-Metriken (SDR, SETTLE, ROLL, GND-Score) sind ausschließlich im Solo-Dashboard** (`/solo`, ADV-Level) verfügbar. Das Partner-Dashboard enthält keine Grounding-Kachel.

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
│  ← g  START CAM  FLIP CAM  FULL  FREEZE  ZERO  AUDIO  REC   │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│   CONNECTION FORCE  (−10,0 kg bis +10,0 kg)                 │
│   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━   │
│                                                              │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│   COMBINED ANALYSIS  (Roll-off quality · Jerk · Errors)      │
│   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━   │
│   ■ Sound/Error Left   ■ Sound/Error Right   ■ Hand Jerk     │
├──────────────────────────────────────────────────────────────┤
│  STEP:  ⬅ BWD R   −7°   TOE-FIRST ✓                         │
│  PELVIS: 🌀 ACTIVE  STABLE  HIP LEADS  GROUNDED  ANCHORED  HIP SETTLE ✓ │
└──────────────────────────────────────────────────────────────┘
```

**Zahl oben links** (`114 g`, `−39 g`, `— g`): aktueller Verbindungskraft-Messwert vom Hand-Sensor. Grün (positiv) = Druck/Kompression, Rot (negativ) = Zug/Spannung, grauer Strich = Sensor offline.

> 📷 **Screenshot-Platzhalter — vollständige Dashboard-Übersicht**  
> *(Ersetzen durch: Vollbild-Foto des Partner-Dashboards mit allen aktiven Sensoren, sichtbarer Kamera-Einblendung im Hintergrund, Statusleiste mit Schritt- und Becken-Badges)*

---

## 3. Verbindungskraft-Diagramm (Oberes Diagramm)

Zeigt die vom Dehnungsmessstreifen zwischen den beiden Tänzern gemessene Kraft, skaliert auf ±10,0 kg. Da die Verbindungskraft auf All-Star-Niveau bei Kompression (Whip-Catches, Redirects) über 6 kg erreicht, wurde der Diagrammbereich von ursprünglich ±5 kg erweitert, um Clipping zu vermeiden.

| Linienfarbe | Bedeutung |
| :--- | :--- |
| **Grün** (oberhalb der Mitte) | Leader drückt — Kompression in der Verbindung |
| **Rot** (unterhalb der Mitte) | Leader zieht — Spannung in der Verbindung |
| **Flache Linie in der Mitte** | Neutral — keine messbare Verbindungskraft |

**Worauf zu achten ist:**

- **Ruhige Linie mit geringer Amplitude nahe null** → leichte, reaktionsfähige Verbindung. Ideal.
- **Anhaltende rote Erhöhung** → Leader hält durchgehend Spannung — prüfen, ob die Follow genug Bewegungsfreiheit hat.
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

Ein höherer Wert bedeutet, dass der Fuß mit geringem Aufprall sauber abgerollt ist. Die Linie steigt, wenn der linke Fuß mit guter Technik aufgesetzt wird, und fällt bei schweren, flachen Aufprallen.

### Magenta-Linie — Abrollqualität rechter Fuß

Gleiches Maß wie Cyan, für den rechten Fuß.

**Beide Linien zusammen lesen:**
- Beide Linien verlaufen ähnlich auf mittlerer Höhe → symmetrische, konsistente Technik.
- Eine Linie konstant niedriger → dieser Fuß setzt mit mehr Wucht auf oder rollt weniger sauber ab als der andere.
- Beide Linien nahe null → Fußsensoren offline oder Tänzer steht still.

### Gelbe Linie — Hand-Jerk-Index

Kombiniert, wie schnell sich die Verbindungskraft ändert, mit der Ruckartigkeit der Handbewegung. Steigt bei plötzlichen Führungsimpulsen, fällt bei gleichmäßiger Bewegung.

- **Gelb nahe null** → gleichmäßiges Führen.
- **Gelbe Spitzen** → abrupte Kraft- oder Beschleunigungsänderungen in der Handverbindung.

### Vertikale Markierungslinien

| Farbe | Bedeutung |
| :--- | :--- |
| **Cyan vertikaler Balken** | Fehler linker Fuß: schwerer Aufprall ohne Abrollbewegung |
| **Magenta vertikaler Balken** | Fehler rechter Fuß: schwerer Aufprall ohne Abrollbewegung |
| **Roter vertikaler Balken** | Fehler beider Füße gleichzeitig |
| **Gelbe gestrichelte Linie** | Jerk-Spitze von der Firmware erkannt |

Fehler-Markierungen werden ausgelöst, wenn ein Fuß hart aufsetzt (scharfer Aufprall), ohne abzurollen — ein Stampfmuster. Mehrere Markierungen hintereinander auf derselben Seite weisen auf ein wiederkehrendes Technikproblem an diesem Fuß hin.

> 📷 **Screenshot-Platzhalter — kombiniertes Analyse-Diagramm: asymmetrische Fußqualität**  
> *(Ersetzen durch: Screenshot, auf dem die Cyan-Linie über eine vollständige Phrase deutlich höher liegt als die Magenta-Linie — schwächerer rechter Fuß sichtbar)*

> 📷 **Screenshot-Platzhalter — kombiniertes Analyse-Diagramm: Jerk + Fehlerkorrelation**  
> *(Ersetzen durch: Screenshot mit einer gelben Spitze und einem magenta/roten vertikalen Fehler-Marker zum selben Zeitpunkt)*

---

## 5. Schritt-Badges (Statusleiste)

Wird bei jedem erkannten Fußkontakt aktualisiert. Verwendet dieselbe Klassifizierung wie das Solo-Dashboard auf Anfänger-Niveau — keine Stufenauswahl in der Partner-Ansicht erforderlich.

| Element | Bedeutung |
| :--- | :--- |
| **➡ FWD L / R** | Vorwärtsschritt (Fußwinkel +8° oder mehr), linker oder rechter Fuß |
| **⬅ BWD L / R** | Rückwärtsschritt (Fußwinkel unter −8°), linker oder rechter Fuß |
| **— L / R** | Unklare Zone (−8° bis +7°) — Richtung aus dem Winkel allein nicht erkennbar |
| **Fußwinkel** | Fußneigung beim Aufsetzen (positiv = Zehen hoch, negativ = Zehen runter) |
| **Schritt-Badge** | Klassifizierung der Landung — siehe Tabelle unten |
| **Verzögerungs-Badge** | Temponormierte Zeitgebung der Gewichtsverlagerung — siehe Tabelle unten |
| **Frame-Badge** (`LINK`) | Ob ein Führungsimpuls den Körper tatsächlich bewegt — siehe unten |

### Schritt-Badge Referenz

| Badge | Zone | Jerk | Bewertung |
| :--- | :--- | :--- | :--- |
| `HEEL STRIKE ✓` | HEEL (+8° oder mehr) | ≤ 34 g/s | Korrekter Fersen-zuerst-Kontakt |
| `HEEL SLAM ⚠` | HEEL (+8° oder mehr) | > 34 g/s | Harter Fersenaufprall — zu viel Landekraft |
| `TOE-FIRST ✓` | TOE (unter −8°) | ≤ 34 g/s | Korrekter Zehenball-Kontakt |
| `TOE JAM ⚠` | TOE (unter −8°) | > 34 g/s | Harter Zehenaufprall — überstreckter oder erzwungener Kontakt |
| `SOFT ✓` | Ambiguous (−8° bis +7°) | ≤ 25 g/s | Leichte, kontrollierte Landung — Kamera für Richtungsprüfung nutzen |
| `MODERATE` | Ambiguous (−8° bis +7°) | 25–34 g/s | Mittlerer Aufprall in der Flachzone |
| `HARD IMPACT ⚠` | Ambiguous (−8° bis +7°) | > 34 g/s | Harte Flachfuß-Landung — Stampfmuster |

> **Verbindungskraft-Gate für die Jerk-Schwelle:** Überschreitet die Verbindungskraft im Schritt-Moment ±2 kg, wird die SLAM/JAM/HARD-Schwelle angehoben (×1,8, gedeckelt bei 50 g/s). Die über die Hände übertragene Partnerkraft läuft bis in den Fußsensor und erzeugt eine Schock-Spitze, die **kein** echter Landefehler ist — das Gate unterdrückt diese Falschalarme. Echtes hartes Aufstampfen (>50 g/s) löst auch unter hoher Kraft weiterhin aus.

> 📷 **Screenshot-Platzhalter — Statusleiste: Schritt-Badges**  
> *(Ersetzen durch: Nahaufnahme der Statusleistenzeile, z. B. `⬅ BWD R  −7°  TOE-FIRST ✓` mit ausgeblendeter Becken-Zeile)*

### Verzögerungs-Badge Referenz

Der Verzögerungs-Badge misst, wie schnell das Gewicht nach dem Fußkontakt verlagert wurde, ausgedrückt als Anteil des Schrittintervalls — er ist daher **automatisch an das Musiktempo angepasst**. Bei einem langsamen und einem schnellen Lied wird für dieselbe Bewegungsqualität dasselbe Badge angezeigt.

Die Schwellenwerte unterscheiden sich je nach Richtung, da eine Rückwärtslandung (Zehe zuerst) von Natur aus mehr Einschwingzeit benötigt als eine Vorwärtslandung (Ferse zuerst).

| Badge | Vorwärtsschritt | Rückwärtsschritt | Bewertung |
| :--- | :--- | :--- | :--- |
| `DELAYED ✓` ✅ | 12–38 % des Beats | 18–50 % des Beats | Charakteristisches WCS-„Hover" — Gewicht kommt nach dem Fuß |
| `QUICK` ⚠️ | < 12 % | < 18 % | Gewicht sofort beim Aufprall verlagert — mechanisch, nicht musikalisch |
| `LATE` ⚠️ | > 38 % | > 50 % | Gewicht nie vollständig angekommen — schwebendes oder unvollständiges Transfer |

> **Verbindungskraft-Gate:** Überschreitet die Verbindungskraft im Schritt-Moment ±1,5 kg (in beide Richtungen), wird das Verzögerungs-Badge unabhängig vom gemessenen Verhältnis auf `DELAYED ✓` gesetzt. Hohe Verbindungskraft belastet den Fußsensor und verzerrt das Gewichtsverlagerungs-Signal in beide Richtungen und erzeugt falsche `QUICK`- und `LATE`-Anzeigen, die nicht das tatsächliche Timing des Tänzers widerspiegeln. Das Gate reagiert auf den Kraftbetrag und ist unter 1,5 kg deaktiviert, dort ist das Verhältnis verlässlich.

> **Coaching-Tipp:** `QUICK` bei jedem Anchor-Schritt ist der häufigste Befund auf Newcomer-/Intermediate-Niveau. Der Tänzer tritt zurück, verlagert aber sofort das Gewicht und verliert damit die Dehnung in der Verbindung. Auf `QUICK` in der Statusleiste achten und als Cue geben: *„Tritt zurück und atme, bevor du landest."*

> 📷 **Screenshot-Platzhalter — Verzögerungs-Badge: DELAYED ✓ im Anchor**
> *(Ersetzen durch: Statusleiste mit `⬅ BWD R  −12°  TOE-FIRST ✓  DELAYED ✓` — alles grün, gute Technik)*

### Frame-Badge (`LINK`) — Kraft-zu-Bewegung-Kopplung

Dieser Badge braucht **keinen Zusatzsensor** — er vergleicht den Führungs-Kraftimpuls (wie schnell sich die Verbindungskraft ändert) mit der tatsächlichen **Becken-Beschleunigung** als Antwort. Er beantwortet eine Frage: *Wenn Kraft über die Verbindung eingeleitet wird — bewegt sich der Körper, oder wird die Kraft irgendwo absorbiert?*

| Badge | Bedeutung |
| :--- | :--- |
| `— LINK` | Gerade kein aktiver Führungsimpuls (Verbindung ruhig, oder ein Sensor offline) |
| `TRANSMITTED ✓` | Ein Kraftimpuls wurde mit Körperbewegung beantwortet — die Verbindung trug bis ins Zentrum durch |
| `SOFT LINK ⚠` | Ein klarer Kraftimpuls erzeugte kaum Körperbewegung — die Kraft wurde elastisch absorbiert, statt den Tänzer zu bewegen |

> **Wichtige Einschränkung — vor dem Coaching lesen.** Ein `SOFT LINK` bedeutet **nicht** immer einen Fehler. Im WCS zeigt sich das Halten gegen die Verbindung (eine gute Counterbalance am Anchor) *ebenfalls* als „Kraft eingeleitet, Körper bewegt sich nicht" — was genau richtig ist, kein Kollaps. Dieser Badge kann eine korrekte Counterbalance nicht von einem echten Frame-Kollaps unterscheiden; nur ein Oberkörper-/Torso-Sensor (Brust-Haltung unter Last) kann das. Behandle `SOFT LINK` als **Hinweis, das Paar anzusehen**, nicht als Urteil. Benötigt Hand- und Beckensensor gleichzeitig online.
>
> **Hinweis zu den Schwellenwerten:** Vorläufige Startwerte — anhand eigener Aufnahmen kalibrieren.

## 5b. Becken-Badges (Statusleiste — erscheint wenn der Sensor online ist)

Alle sechs Becken-Metriken werden gleichzeitig angezeigt, wenn der Becken-Sensor aktiv ist — es gibt keine Stufenauswahl in der Partner-Ansicht.

Vollständige Beschreibungen der einzelnen Badges sind unter [dancer_guide_solo.md — Abschnitt 8](dancer_guide_solo.md#8-die-beckenkarte-optionaler-sensor) zu finden.

### Kurzübersicht

| Badge | Grün | Gelb | Rot |
| :--- | :--- | :--- | :--- |
| **Hüftaktivierung** | `🌀 ACTIVE` (≥45°/s) | `MODERATE` (25–45°/s) | `STIFF HIPS` (<25°/s) |
| **Seitliche Stabilität** | `STABLE` | `SLIGHT SWAY` | `LATERAL SWAY` |
| **Hüft-Fuß-Kopplung** | `HIP LEADS` (>100 ms vor dem Fuß) | `IN SYNC` (40–100 ms) | `HIP LAGS` (<40 ms) |
| **Vertikales Wippen** | `GROUNDED` | `SLIGHT BOUNCE` | `BOUNCY` (≥0,038) |
| **Anchor Settle** | `ANCHORED (n)` (≥42) | `SETTLING (n)` (30–41) | `UNSTABLE (n)` (<30) |
| **Hip Settle** | `HIP SETTLE ✓` | `SLIGHT SETTLE` | `OVERSWING ⚠` / `NO HIP SETTLE` |

> **Verbindungskraft-Gates in der Partner-Ansicht.** Drei Becken-/Schritt-Badges verhalten sich hier anders als in der Solo-Ansicht, weil die Verbindungskraft die Rohsignale verfälscht:
> - **Seitliche Stabilität:** `LATERAL SWAY` (rot) wird auf `SLIGHT SWAY` (gelb) herabgestuft, sobald die Verbindungskraft 1,5 kg überschreitet. Das Umlenken der Followerin erzeugt laterale Beckenbeschleunigung, die strukturell ist, kein Gleichgewichtsfehler.
> - **Vertikales Wippen:** Die `BOUNCY`-Schwelle wurde von 0,020 auf 0,038 angehoben. Das Anspannen des Rumpfes gegen Partner-Kraftspitzen lässt die Hüften eine Auf-und-Ab-Bewegung registrieren, auch ohne sichtbares Wippen.
> - **Hüftaktivierung:** Die `ACTIVE`-Schwelle wurde von 60°/s auf 45°/s gesenkt. Der Partner-Slot dämpft die Rotationsgeschwindigkeit, sodass echte aktive Hüften langsamer messen als im Solo-Tanz.

> 📷 **Screenshot-Platzhalter — Statusleiste: Becken-Badges aktiv**  
> *(Ersetzen durch: Nahaufnahme der vollständigen Statusleiste mit beiden Zeilen sichtbar — Schritt-Zeile + PELVIS:-Zeile mit allen 5 Badges in verschiedenen Farben)*

---

## 6. Akustische Alarme

Das Partner-Dashboard gibt synthetische Töne aus, wenn kritische Technikfehler erkannt werden — so muss der Coach nicht auf den Bildschirm schauen, während er den Tänzer direkt beobachtet.

**`🔇 Audio: OFF`** in der Kopfleiste antippen, um Alarme zu aktivieren. Erneutes Tippen stummt die Ausgabe.

| Ereignis | Ton | Bedingung |
| :--- | :--- | :--- |
| **`HEEL SLAM ⚠`** | 1200-Hz-Klick (80 ms) | Harter Fersenaufprall in der HEEL-Zone (Jerk > 34 g/s, kraft-gegated) |
| **`TOE JAM ⚠`** | 1200-Hz-Klick (80 ms) | Harter Zehenaufprall in der TOE-Zone (Jerk > 34 g/s, kraft-gegated) |
| **`HARD IMPACT ⚠`** | 1200-Hz-Klick (80 ms) | Harte Flachfuß-Landung in der Ambiguous-Zone (Jerk > 34 g/s, kraft-gegated) |
| **`LATERAL SWAY`** | 400-Hz-Ton, gehalten (250 ms) | Hüften schwingen seitlich über die Schwelle **und** Verbindungskraft ≤ 1,5 kg — feuert einmalig beim Eintritt in den Fehlerzustand |
| **`BOUNCY`** | 600-Hz-Doppelklick | Auf-und-Ab-Bewegung der Hüften über die Schwelle — feuert einmalig beim Eintritt in den Fehlerzustand |
| **`UNSTABLE`-Anker** | 800 → 350-Hz-Absteigsweep (300 ms) | Anchor-Settle-Score < 30 nach jedem Rückwärtsschritt |

> **Zustandsübergangsbasierte Alarme:** `LATERAL SWAY` und `BOUNCY` feuern nur einmal, wenn das Badge erstmals rot wird — nicht bei jedem Frame. Der Alarm wird zurückgesetzt, sobald das Badge wieder gelb oder grün wird.

---

## 7. Schaltflächen

| Schaltfläche | Funktion |
| :--- | :--- |
| **START CAM / FLIP CAM** | Aktiviert die Kamera-Einblendung; erneutes Tippen wechselt zwischen Front- und Rückkamera |
| **EXIT** | Beendet den Vollbildmodus |
| **FREEZE** | Pausiert beide Diagramme zur Inspektion — nützlich, um einen Moment nach einem Durchlauf zu besprechen |
| **ZERO** | Tariert Fußwinkel-Offsets (setzt die Richtungs-/Winkel-Basislinie zurück) UND löst Hardware-Tarierung der Kraftwaage aus. Tippen, während der Tänzer in neutraler Haltung steht. |
| **🔇 Audio: OFF / 🔊 Audio: ON** | Schaltet synthetische Akustik-Alarme für Technikfehler EIN/AUS — siehe [Abschnitt 6](#6-akustische-alarme) |
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

Nach jedem Anchor zeigt der **Anchor Settle**-Badge einen Wert von 0–100. Ein Wert konstant unter 42 über einen ganzen Song bedeutet, dass das Becken des Tänzers sich noch bewegt, nachdem der Anchor-Schritt gelandet ist. Niedrige Werte gegen Ende eines Songs (aber nicht am Anfang) weisen auf einen erschöpfungsbedingten Anchor-Zusammenbruch hin.

> **Partner-Kontext:** Die ANCHORED-Schwelle in der Partner-Ansicht (≥42) ist niedriger als in der Solo-Ansicht (≥50). Anhaltende Verbindungsspannung hält das Becken unter leichter Restlast, sodass ein echter „Null-Bewegungs"-Anchor bei aktiver Verbindung physikalisch unmöglich ist — die niedrigere Schwelle bildet das ab.

### FREEZE für die Besprechung nutzen

Musik stoppen, unmittelbar nach einem bemerkenswerten Moment auf **FREEZE** tippen. Die Diagramme halten die letzten 10 Sekunden fest. Den Tänzer durch das Bild führen, bevor das Einfrieren aufgehoben wird.

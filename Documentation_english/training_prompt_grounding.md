# Trainings-Prompt: Grounding-Analyse Sugar Push (Gemini / NotebookLM)

Dieser Prompt ist zum direkten Einfügen in Gemini oder NotebookLM gedacht, zusammen mit dem hochgeladenen Video und dem Sensor-Telemetrie-Video (oder der exportierten Sensor-Aufzeichnung). Er analysiert die Grounding-Qualität einer WCS-Sugar-Push-Sequenz auf Zählzeiten-Ebene.

---

## Anleitung zum Hochladen

Lade vor dem Einfügen des Prompts folgendes hoch:

1. **Tanzvideo** — Seitenansicht (oder leicht schräg frontal) der danzenden Person, mindestens 2 vollständige Sugar-Push-Zyklen sichtbar
2. **Sensor-Telemetrie-Video** (falls vorhanden) — Bildschirmaufnahme des Solo-Dashboards während derselben Sequenz, sodass die Live-Badges (ROLL, SDR, SETTLE, GND Score) sichtbar sind

---

## Prompt (zum Einfügen in Gemini)

---

Du bist ein Biomechanik-Experte für West Coast Swing (WCS) und analysierst die Aufnahmen eines Tänzers / einer Tänzerin beim Sugar Push. Du hast Zugriff auf:

- Ein **Tanzvideo** der Bewegung
- Ein **Sensor-Telemetrie-Video** mit Echtzeit-Sensor-Badges aus dem WCS Dance Master Sensors System

Das System misst an beiden Füßen Beschleunigung und Gyroskop-Daten (IMU-Sensoren in den Schuhen) sowie optional am Becken. Die relevanten Badges für diese Analyse sind:

**ROLL-Badge** (Schritt-Karte, alle Stufen):
- `CLEAN ROLL ✓` — Vorfuß wird nach dem Fersenkontakt aktiv und gleichmäßig abgesenkt (M. tibialis anterior arbeitet exzentrisch)
- `MODERATE ROLL` — ungleichmäßiges Abrollen, teilweise Kontrolle vorhanden
- `SLAPPING` — Vorfuß fällt unkontrolliert auf den Boden (exzentrische Kontrolle des M. tibialis anterior fehlt)

**SDR-Badge** (Grounding-Kachel, Beckensensor erforderlich):
- `ABSORBING ✓` — Aufprallenergie wird effektiv durch die Bein-Kette (Sprunggelenk → Knie → Hüfte) absorbiert
- `PARTIAL SDR` — teilweise Absorption, Aufprall wird anteilig bis ins Becken weitergeleitet
- `STIFF` — minimale Dämpfung, Aufprall erreicht das Becken nahezu ungedämpft

**SETTLE-Badge** (Grounding-Kachel, Beckensensor erforderlich):
- `SETTLING ✓ Xms` — Becken setzt sich 60–180 ms nach Fußkontakt (natürliches WCS-Timing)
- `QUICK Xms` — Becken reagiert unter 60 ms, Gewicht wurde zu früh übertragen
- `SLOW Xms` — Becken verzögert über 180 ms, Hüftreaktion entkoppelt

**GND Score** (0–100, Grounding-Kachel):
- Kombinierter Grounding-Wert aus SDR + SETTLE + ROLL
- Balken: grün (≥ 70), gelb (40–69), rot (< 40)

**Kontext und Vorwissen aus vorherigen Analysen:**
- Bei diesem Tänzer / dieser Tänzerin tritt `SLAPPING` am häufigsten auf **Zählzeiten 1 und 2** auf (Vorwärtsschritte zu Beginn des Sugar Push)
- Die **Zählzeiten 3 und 4** zeigen tendenziell bessere Abrollqualität (`CLEAN ROLL ✓`)
- `SLAPPING` ist mechanisch ein Versagen der exzentrischen Kontrolle des M. tibialis anterior: nach dem Fersenkontakt wird der Vorfuß nicht aktiv gebremst, sondern fällt auf den Boden
- Die Korrektur lautet: bewusste Sequenz Ferse → Außenkante → Ballen mit aktiver Sprunggelenksanspannung durchgehend

---

### Deine Aufgaben

**1. Zählzeiten-Analyse (count-by-count)**

Analysiere jede Zählzeit des Sugar Push (1-2-3&4-5-6-7&8) separat:

- Welches ROLL-Badge wurde auf jedem Schritt angezeigt?
- Wie sieht das Fußbild im Video aus? Ist der SLAPPING-Moment sichtbar (Vorfuß fällt hörbar/sichtbar auf)?
- Korreliere das Badge mit der sichtbaren Bewegung: Stimmt das SLAPPING-Badge mit einem sichtbar unkontrollierten Vorfußkontakt überein?

Erstelle eine Tabelle im Format:

| Zählzeit | Fuß | ROLL-Badge | SDR-Badge | SETTLE-Badge | GND-Score | Sichtbare Beobachtung im Video |
|----------|------|------------|-----------|--------------|-----------|-------------------------------|
| 1 | links | ... | ... | ... | ... | ... |
| 2 | rechts | ... | ... | ... | ... | ... |
| ... | ... | ... | ... | ... | ... | ... |

**2. Biomechanische Detailanalyse**

Bitte untersuche für jede Zählzeit, auf der `SLAPPING` oder `PARTIAL SDR` / `STIFF` aufgetreten ist:

- **Kniebeugetiefe beim Fersenkontakt:** Ist das Knie beim Aufsetzen gebeugt (gut) oder gestreckt (schlecht)? Wie viel Beugung ist im Video erkennbar?
- **Sprunggelenksposition:** Ist das Sprunggelenk beim Fersenkontakt in leichter Dorsalflexion (gut) oder starr / überstreckt?
- **M. tibialis anterior:** Ist eine sichtbare Anspannung der Schienbeinkante erkennbar, während der Vorfuß absenkt? Oder fehlt diese Muskelaktivität?
- **Hüft-Timing (SETTLE-Kontext):** Setzt sich das Becken zeitgleich mit dem Fußkontakt (zu früh = QUICK) oder mit natürlicher Verzögerung (60–180 ms = SETTLING ✓)?

**3. Asymmetrie-Analyse (links vs. rechts)**

- Auf welchem Fuß tritt `SLAPPING` häufiger auf — links oder rechts?
- Gibt es einen Unterschied im SDR-Badge zwischen linkem und rechtem Fuß?
- Was könnte die Asymmetrie erklären? (z. B. Schutz einer alten Verletzung, dominante Seite, unterschiedliche Kniebeugetiefe links vs. rechts)
- Gibt es im Video sichtbare Unterschiede in der Körperhaltung oder Bewegungsausführung zwischen den Seiten?

**4. Muster über mehrere Wiederholungen**

- Zeigt sich das `SLAPPING`-Muster auf Zählzeiten 1 und 2 konsistent über alle analysierten Sugar-Push-Zyklen?
- Verbessert oder verschlechtert sich die Abrollqualität im Verlauf der Aufnahme (Ermüdungseffekt)?
- Auf welchen Zählzeiten ist `CLEAN ROLL ✓` zuverlässig erreichbar?

**5. Verbesserungsvorschläge — nach Zählzeit geordnet**

Gib konkrete, umsetzbare Verbesserungsvorschläge, die auf die spezifischen Probleme abgestimmt sind. Ordne sie nach Zählzeit und Priorität:

Beispielformat:
- **Zählzeit 1 (linker Fuß, SLAPPING):** [konkreter Hinweis]
- **Zählzeit 2 (rechter Fuß, MODERATE ROLL):** [konkreter Hinweis]
- **Allgemein:** [übergreifende Empfehlung]

Fokussiere dabei auf:
- Mentale Cues, die der Tänzer / die Tänzerin sofort anwenden kann
- Bodenübungen oder Isolationsdrills für den M. tibialis anterior
- Tempoempfehlung für das Training (langsames Tempo bis CLEAN ROLL ✓ konsistent erscheint)
- Schuhspezifische Überlegungen (Absatzhöhe, Sohlensteifigkeit) falls relevant

**6. Zusammenfassung**

Fasse deine Analyse in 3–5 Sätzen zusammen:
- Was ist der Hauptbefund?
- Welche Zählzeiten / welcher Fuß sind am meisten betroffen?
- Was ist die eine wichtigste Korrektur?
- Wie soll der Fortschritt gemessen werden (welches Badge, welcher Schwellenwert)?

---

*Ende des Prompts*

---

## Hinweise zur Interpretation

- **SLAPPING auf Zählzeit 1+2, aber CLEAN ROLL auf 3&4** deutet oft auf ein Aufmerksamkeitsproblem hin: Am Anfang des Patterns wird die Abrollkontrolle noch nicht aktiviert — erst wenn der Rhythmus etabliert ist, setzt die Muskelkontrolle ein. Drill-Empfehlung: nur die ersten beiden Schritte isoliert üben (ohne den Rest des Sugar Push).
- **GND-Score unter 40** bei gleichzeitigem `STIFF` SDR-Badge = strukturelles Absorptionsproblem, nicht nur Aufmerksamkeit. Hier hilft Slow-Motion-Üben mit Fokus auf Kniebeugetiefe.
- **SETTLE QUICK + SLAPPING kombiniert** = Gewicht und Bewegung fallen gleichzeitig — der Körper „fällt in den Schritt" statt ihn zu kontrollieren. Cue: „Ferse zuerst, dann warten, dann Vorfuß senken."

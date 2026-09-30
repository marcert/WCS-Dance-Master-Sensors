# WCS Solo-Training Dashboard: Biomechanische & Technische Dokumentation

Dieses Dokument bietet eine umfassende Übersicht über die biomechanischen Metriken, Sensorfusions-Algorithmen, Schwellenwert-Konfigurationen, Zustandsmaschinen-Sperrlogik (State Machine Lockout) und akustischen Biofeedback-Mechanismen des **WCS Solo-Training-Dashboards** (`/solo`).

<p align="center">
<img src="https://raw.githubusercontent.com/marcert/WCS-Dance-Master-Sensors/refs/heads/main/Attachments/Solo-Dashboard.jpg" width="300">
</p>

---

## 1. Systemarchitektur & Hochfrequenz-Datenerfassung

Das Solo-Training-System arbeitet als hochfrequenter biomechanischer Feedback-Kreislauf für die West Coast Swing-Schritttechnikanalyse:

```text
 +--------------------------+           +--------------------------+
 |  Linker Fußsensor (ID 1) |           | Rechter Fußsensor (ID 2) |
 |  M5Stick S3 @ 200 Hz     |           | M5Stick S3 @ 200 Hz      |
 |  Invertierte aY-Montage  |           | Standard-Montage         |
 +------------+-------------+           +------------+-------------+
              |                                      |
              +-----------------+  +-----------------+
                                |  |  ESP-NOW (5 ms Intervall)
                                v  v
 +------------------------+   +-+-----------------------------+
 | Beckensensor (opt.)    |-->|  Zentrale Master-Einheit      |
 | ID: 4 | M5Stick @ 200Hz|   |  M5Stick S3 Web-Server        |
 +------------------------+   +---------------+---------------+
                                              |
                                              | Web HTTP / JSON-Stream (`/data`) @ 50 Hz
                                              v
                             +--------------------------------+
                             | Web-Browser-Dashboard (`/solo`)|
                             | Transparentes WebRTC-Overlay   |
                             | Web Audio API Biofeedback      |
                             +--------------------------------+
```

* **Fußknoten (IDs 1 & 2):** M5Stick-S3-Einheiten mit 6-Achsen-IMUs (Inertial Measurement Units, BMI270 / MPU6886). Die Firmware arbeitet mit **200 Hz (5 ms Abtastintervall)**, um ultraschnelle transiente Impulsspitzen bei Fersenaufsatz und Zehenlandungen zu erfassen.
* **Beckenknoten (ID 4, optional):** Gleiche Hardware, am Kreuzbein auf einem Gürtel getragen. Überträgt 3-Achsen-Beschleunigung (`pA`, `pAy`, `pAx`) und Gier-Gyro (`pYaw`, `pG`) bei 200 Hz. Ermöglicht Hüftaktivierung, seitliche Stabilität, Hüft-Fuß-Kopplung, vertikales Hüpfen, Beckenneigung, Anchor Settle und Hip Settle. Fehlt der Sensor, arbeitet das Dashboard normal ohne Beckenkarten.
* **Thoraxknoten (ID 5, optional):** Gleiche Hardware, hoch am oberen Rücken (C7–T1) getragen, identisch montiert und achsen-gemappt wie der Beckenknoten. Überträgt `tA`/`tAy`/`tAx`-Beschleunigung und `tYaw`/`tG`-Gyro bei 200 Hz. Ermöglicht die Oberkörper-Poise-Metriken (§7). **Provisorisch — noch nicht WCS-validiert.** Fehlt der Sensor, sind die Torso-Zeilen ausgeblendet.
* **Zentrale Master-Einheit:** Aggregiert ESP-NOW-Streams und liefert JSON-Datenpakete (`lG`, `lA`, `lAy`, `lGr`, `rG`, `rA`, `rAy`, `rGr`; optional `pA`, `pAy`, `pAx`, `pYaw`, `pG`, `pOk`) über den `/data`-Endpunkt an den Browser.
* **Web-Dashboard (`/solo`):** Clientseitiges JavaScript führt Zustandsmaschinen-Filterung, Richtungszuordnung, Neigungsintegration, Standphasen-Berechnungen und Web-Audio-API-Feedback aus.

> 📸 **[Screenshot: Solo-Dashboard mit allen vier verbundenen Sensorknoten und laufendem Live-Telemetrie-Stream im Browser]**

---

## 2. Mathematische & Biomechanische Definitionen

### A. Kinematische Abrollphasen (Rocker-Modell)

Die Fußmechanik im West Coast Swing folgt einem biomechanischen 4-Phasen-Abrollmodell (*Rocker-Modell*) bei Vorwärts- und Rückwärts-Gewichtstransfers:

```text
Vorwärtsschritt (Fersenaufsatz / Ferse-Ballen-Zehe):
  [Erstkontakt]       -->  [Belastungsreaktion] -->  [Mittlere Standphase] -->  [Abstoß (Windlass)]
    (Fersenaufsatz)         (Exzentrische Kont.)     (Ganzer Fuß)                (Zehenantrieb)
      θ > 10°                  Plantarflexion         θ ≈ 0°                    -ω_pitch > 120°/s

Rückwärtsschritt (Zehenlandung / Zehe-Ballen-Ferse):
  [Zehenkontakt]      -->  [Absenkphase]       -->  [Volle Gewichtsübernahme (Anchor Settle)]
    (Ballen / Spitze)        (Exz. Sprunggelenk)    (Ferse berührt Boden & COM-Verlagerung)
      -20° ≤ θ ≤ 5°           θ → 0°                  θ ≈ -2° to 5°, aZ > 0.85g
```

**Die drei Rocker (Modell nach Perry & Burnfield):**

| Rocker | Standphase | Drehpunkt | Muskuläre Kontrolle |
| :--- | :--- | :--- | :--- |
| **Fersenrocker** (1.) | Erstkontakt → Belastungsreaktion | Ferse | Exzentr. M. tibialis anterior — senkt Fuß zum Boden |
| **Sprunggelenkrocker** (2.) | Belastungsreaktion → Mittlere Standphase | Sprunggelenk | Exzentr. Gastrocnemius/Soleus — kontrolliert Tibiavorlauf |
| **Vorfußrocker** (3.) | Terminale Standphase → Abstoß | Metatarsalköpfe | Windlass-Mechanismus + konzentrische Plantarflexoren |

Im WCS sind bei **Vorwärtsschritten** alle drei Rocker vorhanden: Fersenaufsatz → Tibiavorlauf → Vorfußabrollbewegung. Bei **Rückwärtsschritten** wirken nur Vorfuß- und Sprunggelenkrocker in umgekehrter Reihenfolge (Zehenerstkontakt → Fersenabsenken). Der θ-Winkel zum T-1-Zeitpunkt erfasst die Fußposition an den Übergängen zwischen den Rocker-Phasen; Jerk quantifiziert, wie abrupt jeder Übergang ausgeführt wird.

> **Weiterführende Literatur:** Perry, J. & Burnfield, J.M. (2010). *Gait Analysis: Normal and Pathological Function* (2. Aufl.). SLACK Inc. — die klinische Standardreferenz für alle hier verwendeten Gangphasen-Begriffe. Frei zugänglicher Überblick: [Wikipedia — Ganganalyse](https://de.wikipedia.org/wiki/Ganganalyse).

---

### B. Fußneigungswinkel ($\theta$) & Landeartikulation

> **Messtechnischer Hinweis:** θ misst die sagittale Neigung des *Fußsegments* relativ zur Schwerkraft (Fußneigungswinkel), nicht den anatomischen Sprunggelenk- (Talokruralgelenk-)Winkel. Der anatomische Sprunggelenkwinkel würde einen zweiten Sensor am Schienbein erfordern. Aussagen wie „Dorsalflexion" beziehen sich im Folgenden auf die Fußsegment-Interpretation.

1. **Komplementärfilter-Neigungswinkel ($\theta_{\text{raw}}$) (Complementary Filter):**
   Gyro und Beschleunigungsmesser werden mit α = 0,94 (τ ≈ 78 ms) fusioniert. Der T-1-Schnappschuss (einen Frame *vor* dem Auslöseimpuls aufgenommen) schützt die θ-Schätzung vor Aufprallverzerrungen — höhere α-Werte wurden getestet, verursachten jedoch dθ-Kompression und Gyro-Spike-Artefakte. Der Beschleunigungsreferenzwinkel unterscheidet sich je nach Sensor aufgrund der physischen Montage:

   | Sensor | Beschleunigungsreferenz | Grund |
   | :--- | :--- | :--- |
   | Rechts (ID 2) | $\theta_{\text{accel}} = \text{atan2}(aY_R,\; aZ_R)$ | Standardausrichtung |
   | Links (ID 1) | $\theta_{\text{accel}} = \text{atan2}(-aY_L,\; aZ_L)$ | aY-Achse durch Montage physisch invertiert |

   $$\theta_{\text{raw}}(t) = \alpha \cdot \bigl(\theta_{\text{raw}}(t-\Delta t) + \omega_{\text{pitch}} \cdot \Delta t\bigr) + (1-\alpha) \cdot \theta_{\text{accel}}, \quad \alpha = 0.94$$

   **T-1-Schnappschuss für Schrittklassifikation:** Zum Zeitpunkt des Aufpralls wird der Winkel vom *vorherigen Frame* (T-1) verwendet — nicht der Momentanwert. Der aZ > 0,92–0,95 g-Auslöser (tempo-adaptiv) feuert nach teilweiser Gewichtsbelastung, wenn das Abrollen bereits begonnen hat; der T-1-Frame erfasst die Fußausrichtung vor dem Kontakt, bevor Verzerrungen auftreten.

2. **Nullpunkt-Tarierung ($\theta_{\text{calibrated}}$):**
   Zur Anpassung individueller Schuhabsatz-Neigungen erfasst der `📐 ZERO`-Button statische Montageversätze ($\text{leftMountOffset}$, $\text{rightMountOffset}$):
   $$\theta = \theta_{\text{raw}} - \text{mountOffset}$$

3. **Richtungsanzeige & Landungsqualitäts-Badge:**

   **Messtechnische Einschränkung:** Messungen (n=15 Rückwärtsschritte im Flat-Walk, inklusive Beckensensor) zeigen, dass WCS-Rückwärtsschritte im Flat-Walk konsistent bei θ = +2° bis +9° landen — identisch mit der mehrdeutigen Zone. Weder der dθ-Neigungstrend noch die sagittale Beckenbeschleunigung (durchschnittlicher Richtungsunterschied < 0,03 g über alle Achsen) können in diesem Bereich zuverlässig zwischen Rückwärts und Vorwärts unterscheiden. Das Richtungs-Badge wird daher nur angezeigt, wenn θ eindeutige physikalische Evidenz liefert:

   | θ bei T-1 | Richtungs-Badge |
   | :---: | :--- |
   | θ ≥ +6° | ➡ FWD (blau) — zuverlässiger Fersenerstkontakt |
   | θ < −6° | ⬅ BWD (lila) — zuverlässiger Zehenerstkontakt |
   | −6° ≤ θ < +6° | — (grau) — mehrdeutig; Richtung nicht angezeigt |

   **Interne Richtungsklassifikation (Anchor-Settle-Auslöser):** Unabhängig von der Badge-Anzeige verwendet die interne `activeDir`-Logik eine engere mehrdeutige Zone: Nur `0° < θ < +10°` erfordert eine Neigungstrend-Prüfung. Für `θ ≤ 0°` (jede Plantarflexion / Zehenerstkontakt) wird `activeDir` direkt auf BACKWARD gesetzt, ohne Trendanalyse. Dies stellt sicher, dass Rückwärtsschritte, die bei θ = −2° bis −5° landen, das Anchor-Settle-Auswertungsfenster korrekt auslösen, obwohl das Richtungs-Badge weiterhin „—" anzeigt (da |θ| < 8°).

   **Landungsqualitäts-Badge — richtungsunabhängig, basierend auf θ-Zone + Jerk:**

   Das Strike-Badge bewertet *wie* der Fuß gelandet ist, unabhängig von der Richtung. Dies ist für Vorwärts- und Rückwärtsschritte gleichermaßen nützlich: `SOFT ✓` bei θ ≈ 0° zeigt einen kontrollierten Rückwärtsschritt an; `HARD IMPACT ⚠` bei θ ≈ 0° bedeutet, dass der Tänzer auf den Fuß gefallen ist.

   Jerk-Schwellenwerte:
   - **HART:** J > 130 g/s (intern > 520)
   - **MODERAT:** 55 g/s < J ≤ 130 g/s (intern 220–520)
   - **WEICH:** J ≤ 55 g/s (intern ≤ 220)

   | θ-Zone | Jerk | Badge | Bedeutung |
   | :---: | :---: | :--- | :--- |
   | ≥ +6° (Ferse) | ≤ 130 g/s | `HEEL STRIKE ✓` (Grün) | Saubere Fersenlandung — korrekte Vorwärtstechnik |
   | ≥ +6° (Ferse) | > 130 g/s | `HEEL SLAM ⚠` (Rot) | Fersenkontakt, aber zu abrupt — mit Knie/Knöchel abfedern |
   | < −6° (Zehe) | ≤ 130 g/s | `TOE-FIRST ✓` (Grün) | Kontrollierter Zehenerstkontakt — korrekt für tiefe Rückwärtsschritte oder Ball-Steps |
   | < −6° (Zehe) | > 130 g/s | `TOE JAM ⚠` (Rot) | Zehenkontakt zu hart |
   | −6° bis +5° (mehrdeutig) | ≤ 55 g/s | `SOFT ✓` (Grün) | Kontrollierte Landung — gute Qualität unabhängig von der Richtung |
   | −6° bis +5° (mehrdeutig) | 55–130 g/s | `MODERATE` (Gelb) | Akzeptabel; Aufprall reduzieren |
   | −6° bis +5° (mehrdeutig) | > 130 g/s | `HARD IMPACT ⚠` (Rot) | Auf den Fuß gefallen — löst 1200-Hz-Klick aus |

   * **BRUSH+HEEL-Neuklassifikation (200-ms-Fenster):** Wenn eine Landung in der mehrdeutigen Zone innerhalb von 200 ms von einem zweiten aZ > 1,05 g-Peak mit accelAngle > 8° am gleichen Fuß gefolgt wird, wird das Badge zu `BRUSH+HEEL` (grün) aufgewertet und das Richtungs-Badge zeigt ➡ FWD.

---

### C. Terminale Standphase & Power Push-Antrieb

WCS-Antrieb erfordert ein aktives Zehenabdrücken (*Windlass-Mechanismus*) vom Standbein am Ende der Standphase. Die optimale Plantarflexions-Winkelgeschwindigkeit hängt von der Bewegungsrichtung ab — Vorwärtsantrieb erfordert mehr Kraft als die subtilere Umverteilung bei einem Anker oder Rückwärtslauf.

> **Der Windlass-Mechanismus** (Hicks, 1954): Beim Zehenabdrücken dorsalflexieren die Zehen, wodurch die Plantarfaszie — die unter den Metatarsalköpfen verläuft — sich wie ein Seil auf einer Winde (engl. *windlass*) strafft. Das hebt das mediale Längsgewölbe an und verwandelt den Fuß von einer flexiblen Stoßdämpferstruktur in einen steifen Hebel für den Vorwärtsantrieb. Im WCS bestätigt ein `🚀 POWER PUSH`-Badge, dass der Mechanismus ausgelöst wurde: die ausreichende $-\omega_\text{pitch}$-Winkelgeschwindigkeit zeigt, dass der Vorfußrocker vollständig abgeschlossen und der Zehenabdruck propulsiv war.
>
> **Referenz:** Hicks, J.H. (1954). The mechanics of the foot. *Journal of Anatomy*, 87(4), 345–357. Freier Überblick (englisch): [Wikipedia — Windlass mechanism of the foot](https://en.wikipedia.org/wiki/Windlass_mechanism_of_the_foot).

* **Erkennung — zwei komplementäre Signale:**
  * **Momentaner Spitzenwert:** $-\omega_{\text{pitch}} \ge 120^\circ/\text{s}$ UND $aY > 0.15g$ — erfasst kurze explosive Abstoßbewegungen.
  * **Energieintegral:** $\Phi_{\text{push}} = \int -\omega_{\text{pitch}}\,dt$ während $aY > 0.15g$, akkumuliert seit der letzten Landung, zurückgesetzt bei jedem Schrittauslöser — erfasst anhaltende Antriebe mit geringerer Amplitude, die der Spitzendetektor allein übersehen würde.

  Das $aY > 0.15g$-Gate bestätigt die Bodenscherkraft (Translationskomponente des dritten Newtonschen Gesetzes) und unterdrückt unbelastete Schwungbein-Artefakte. Der endgültige Push-Level ist das Maximum beider Signale — entweder ein hoher Spitzenwert *oder* ein ausreichendes Integral qualifiziert als POWER PUSH.

* **Abgestufte Rückmeldung (hält 400 ms) — richtungsabhängige optimale Schwellenwerte:**

| Letzter Schritt Standbein | Spitzenwert $-\omega_{\text{pitch}}$ | Integral $\Phi_{\text{push}}$ | Badge | Bedeutung |
| :---: | :---: | :---: | :---: | :--- |
| BACKWARD (→ Vorwärtslauf) | $\ge 200^\circ/\text{s}$ | $\ge 20°$ | `🚀 POWER PUSH` (Grün) | Starker Vorwärtsantrieb — Ziel für Läufe und Passes |
| FORWARD (→ Anker / Rückwärtslauf) | $\ge 160^\circ/\text{s}$ | $\ge 16°$ | `🚀 POWER PUSH` (Grün) | Ausreichende Umverteilung — geringerer Antrieb am Anker erwartet |
| Beide Richtungen | Spitze $120\text{–}199^\circ/\text{s}$ ODER Integral $\ge 12°$ | | `↗ PUSH` (Gelb) | Abstoß erkannt, aber unter dem Richtungsoptimum |
| Beide Richtungen | Spitze $< 120^\circ/\text{s}$ UND Integral $< 12°$ | | `— PUSH-OFF` (Grau) | Kein signifikanter Abstoß erkannt |

> **Tempoabhängige Skalierung:** Alle Peak-Schwellenwerte (120/160/200 °/s bei Referenztempo) skalieren mit dem Schrittintervall: `scaleFactor = 500 / max(400, stepDurationMs)`. Bei langsamem Tempo (700 ms/Schritt, scaleFactor ≈ 0,71) fallen die Schwellenwerte auf ≈86/114/142 °/s; bei schnellem Tempo (400 ms/Schritt, scaleFactor = 1,25) steigen sie auf ≈150/200/250 °/s. Die Integralschwellenwerte (12°/16°/20°) messen die gesamte Winkelverschiebung und sind temponeutral.

> **Hinweis:** Die $-\omega_{\text{pitch}}$-Werte sind Fußsegment-Winkelgeschwindigkeiten, keine anatomischen Sprunggelenk-Winkelgeschwindigkeiten. Literaturwerte (~250°/s) gelten für Barfuß-/Sportgang; 200°/s und 160°/s sind Leistungsschwellenwerte, kalibriert für Tanzschuhe auf Studioparkettböden. Die Integralschwellenwerte (12°/16°/20°) approximieren 100 ms anhaltenden Abstoß bei den entsprechenden Spitzengeschwindigkeiten.

---

### D. Aufprall-Jerk ($J_{\text{impact}}$) & Stoßdämpfung

Der Aufprall-Jerk quantifiziert die Änderungsrate der vertikalen Beschleunigung ($aZ$ in $g$) beim Schrittaufsetzen — ein Proxy dafür, wie abrupt die kinematische Kette belastet wird:

$$J_{\text{impact}} = \left| \frac{aZ_{\text{current}} - aZ_{\text{previous}}}{\Delta t} \right| \quad [\text{g/s}]$$

> **Einheitenhinweis:** Dieses $J$ ist in $g/\text{s}$, nicht in $N/\text{s}$ oder $\text{BW/s}$ wie in der Bodenreaktionskraft-Literatur. Die folgenden Schwellenwerte sind geräte- und algorithmusspezifische Heuristiken, keine direkten Äquivalente zu GRF-Belastungsratenstudien.

* **Weiche Dämpfung ($\le 55\text{ g/s}$):** Gute Gelenkabsorption (`SOFT ✓`).
* **Moderater Aufprall ($55\text{ bis }130\text{ g/s}$):** Erhöhter, aber normaler Schrittaufprall (`MODERATE`).
* **Hartes Stampfen ($> 130\text{ g/s}$):** Übermäßiger Schock auf die Gelenke (`HARD IMPACT ⚠`); löst einen niederfrequenten 500-Hz-Aufprallklick aus. (Interne Skala: Badge-Grenzen bei 220 und 520 Roheinheiten = 55 und 130 g/s angezeigt nach ÷4-Skalierung.)

---

### E. Doppelstandphasen-Überlappung ($\Delta t_{\text{double-stance}}$) & Bodenkontakt-Verhältnis

West Coast Swing betont einen kontinuierlichen, gegrundeten „Abroll"-Gewichtstransfer anstelle von abruptem Hüpfen oder verfrühtem Abheben vom Boden. Bodenkontakt wird über einen **Hysterese-Algorithmus** erkannt: Ein Fuß wechselt auf „am Boden" (●) wenn $|aZ| > 0{,}65\,g$, und auf „abgehoben" (○) wenn $|aZ| <$ exitAZ **ODER** $|\omega_{\text{pitch}}| > 80\,°/\text{s}$ **ODER** $|\omega_{\text{roll}}| > 80\,°/\text{s}$, nach Ablauf einer Mindest-Kontaktzeit.

> **Signalhinweis:** Der $|aZ|$-Schwellenwert ist eine Sensor-Heuristik für die Bodenreaktionskraft — keine direkte Kraftmessung. Dynamische Fußrotationen können $aZ$ unabhängig vom tatsächlichen Bodenkontakt verschieben. Der Gyro-Exit-Guard (80 °/s) verhindert vorzeitiges ○ beim normalen Push-off-Abrollen, das typischerweise 60–75 °/s erreicht. Die Schwellenwerte sind empirisch kalibriert.

> **Implementierungsdetail — Hysterese-Parameter:**
> * **Eintritt:** $|aZ| > 0{,}65\,g$ → Fuß wird ● (gelandet); `landedAt`-Timer startet
> * **Austrittsschwelle (exitAZ):** $0{,}48\,g$ wenn Gegenfuß $|aZ| > 0{,}75\,g$ (trägt Last), sonst $0{,}45\,g$
> * **Mindest-Kontaktzeit (minGnd):** $\text{clamp}(t_{\text{step}} \times 0{,}40,\;150\,\text{ms},\;300\,\text{ms})$ — Austritts-Bedingung wird bis zum Ablauf dieser Zeit nach Landung gesperrt
> * **Maximale Kontaktzeit (Timed-Exit):** $\text{clamp}(t_{\text{step}} \times 0{,}75,\;350\,\text{ms},\;600\,\text{ms})$ — Fuß wird nach dieser Dauer unabhängig von $aZ$ zwangsweise ○
> * **Schrittintervall-Glättung (EMA):** $t_{\text{step}} = 0{,}45 \times t_{\text{step,prev}} + 0{,}55 \times t_{\text{step,aktuell}}$ — schnell konvergierende EMA (α = 0,55) verhindert, dass ein einzelnes Ausreißer-Intervall den DS%-Nenner verfälscht

$$\text{Standphasenverhältnis} = \left( \frac{\Delta t_{\text{double-stance}}}{t_{\text{step}}} \right) \times 100\%$$

#### Warum Überlappung in der WCS-Mechanik wichtig ist:
* **Geerdetes Abrollen:** Im West Coast Swing ist der Gewichtstransfer graduell. Während ein Fuß den Boden verlässt, nimmt der andere das Gewicht auf und erzeugt eine natürliche bilaterale Überlappungsphase (Bodenkontakt-Eintritt bei $|aZ| > 0{,}65\,g$).
* **Elastische Ausdehnung & Timing:** Ein gesundes Überlappungsverhältnis ($15\%\text{ bis }60\%$) erzeugt die charakteristische „elastische" Dehnung und den reibungslosen Impulsübergang im WCS. Zu wenig Überlappung zeigt Hetzen oder Springen an, zu viel führt zu schwerfälligen Übergängen.
* **Hinweis zur Fachliteratur:** Die klassische Ganganalyse (Perry & Burnfield, 2010 — zitiert in §2A; Winter, D.A., 1990: *Biomechanics and Motor Control of Human Gait*, University of Waterloo Press) gibt die Standphase mit ~60 % und die Schwungphase mit ~40 % des Gangzyklus bei komfortabler Gehgeschwindigkeit an. Dies ist eine andere Messung — sie beschreibt, wie lange *ein* Fuß während eines Gangzyklus auf dem Boden bleibt. Die hiesige Metrik misst das *gleichzeitige bilaterale Kontaktverhältnis* innerhalb eines Schrittintervalls, was ein Subset der Einzel-Fuß-Standphase ist. Freier Überblick über Gangphasendefinitionen: [Wikipedia — Ganganalyse](https://de.wikipedia.org/wiki/Ganganalyse).

| Verhältnisbereich (%) | Badge-Bewertung | Biomechanische Bedeutung |
| :---: | :---: | :--- |
| **15% bis 60%** | `OPTIMAL ROLL` | Ideale geerdete Abrollphase für Läufe und Ausdehnung. |
| **< 15%** | `HECTIC` | Gehetzter Gewichtstransfer; fehlende Abrollartikulierung. |
| **> sluggishThr (tempoadaptiv)** | `SLUGGISH` | Übermäßiger Bodenkontakt; schwerfälliger Tempoübergang. Schwellenwert: 60 % bei ≥ 120 BPM, 67 % bei 90 BPM, 70 % bei 80 BPM, 72 % bei 75 BPM. Formel: `min(80, 60 + max(0, stepDurationMs − 500) × 0,04)` |

---

### F. Abroll-Symmetrie-Index (ASI)

**Asymmetrie-Index (ASI):**
   Vergleicht die integrierte Winkelarbeit über linke und rechte Fuß-Abrollzyklen während die Füße aktiv in Bewegung sind ($|\omega_{\text{pitch}}| > 15^\circ/\text{s}$):
   $$\text{ASI} = \frac{2 \cdot \left|\int|\omega_{\text{left}}|\,dt - \int|\omega_{\text{right}}|\,dt\right|}{\int|\omega_{\text{left}}|\,dt + \int|\omega_{\text{right}}|\,dt} \times 100\%$$
   * **Ziel:** $< 15\%$ (`SYMMETRIC`), $16\text{--}35\%$ (`MINOR ASYM`), $>35\%$ (`ASYMMETRIC`).

---

### Rollen-Modus: Leader / Follower

Der **👤 LEADER / 💃 FOLLOWER**-Schalter (in localStorage gespeichert) passt Schwellenwert-Gruppen für drei Metriken an, um die biomechanischen Unterschiede zwischen Leader- und Follower-Rolle im WCS widerzuspiegeln.

**Warum unterschiedliche Schwellenwerte:**
- **Timing:** Follower reagieren auf die Führung — ihr Gewichtstransfer ist von Natur aus schneller. Das gleiche schnelle Timing, das beim Leader „zu früh" signalisiert, ist für den Follower korrekt und beabsichtigt.
- **Push-Off:** Follower-Abstoß ist strukturell kompakter (kürzere Hebellänge, weniger vorbereitende Standphasenverlängerung).
- **Asymmetrie:** Follower haben eine strukturelle Verbindungsseiten-Asymmetrie, die unabhängig vom Können besteht.

**Schwellenwert-Vergleich:**

| Metrik | Leader | Follower |
|---|---|---|
| DELAY RAMP vorwärts — DELAYED ✓ | 12–38 % | 6–30 % |
| DELAY RAMP rückwärts — DELAYED ✓ | 18–50 % | 10–40 % |
| Push-Off vorwärts (POWER PUSH) | ≥ 200 °/s Peak ODER ≥ 20° Integral | ≥ 160 °/s Peak ODER ≥ 16° Integral |
| Push-Off rückwärts (POWER PUSH) | ≥ 160 °/s Peak ODER ≥ 16° Integral | ≥ 130 °/s Peak ODER ≥ 13° Integral |
| ASI — SYMMETRISCH | ≤ 15 % | ≤ 25 % |
| ASI — GERINGE ASYM. | ≤ 35 % | ≤ 40 % |

**Kalibrierungsanzeige (nur Follower-Modus):**
Im Follower-Modus werden zwei zusätzliche Rohwerte zur Algorithmus-Validierung angezeigt:
- **Richtungs-Badge:** Zeigt den gemessenen Fußwinkel θ beim Aufprall (z. B. `⬅ BWD −4°`, `— +2°`). Ermöglicht die Validierung der ±6°-Zonengrenze für Follower-Rückwärtsschritte.
- **Push-Off-Badge:** Zeigt die Spitzen-Winkelgeschwindigkeit des abstoßenden Fußes (z. B. `↗ PUSH 148 °/s`). Ermöglicht die Validierung der 160 °/s / 130 °/s Follower-Push-Schwellen.
Beide Werte sind im Leader-Modus ausgeblendet.

**Status:** DELAY RAMP- und ASI-Schwellen validiert (Gemini-Videoanalyse, Sep 2026). Push-Off-°/s-Schwellen und θ-Zonengrenzen für Follower-Rückwärtsschritte warten auf Validierung mit dediziertem Follower-Videomaterial.

---

### G. Gewichtstransfer-Gradient & Sprunggelenk-Stoßdämpfung

Beide Metriken werden aus einem Post-Aufprall-Überwachungsfenster berechnet, das unmittelbar nach jedem Schrittauslöser öffnet.

**Gewichtstransfer-Gradient** (240-ms-Fenster, 12 Samples):

$$\text{loadRise} = \overline{aZ}_{[160-240\text{ ms}]} - \overline{aZ}_{[0-80\text{ ms}]}$$

| loadRise | Badge | Biomechanische Bedeutung |
| :---: | :---: | :--- |
| $> 0.12\,g$ | `SMOOTH LOAD` (Grün) | Progressiver Gewichtstransfer — Körperschwerpunkt bewegt sich schrittweise über den Fuß |
| $-0.10\text{ bis }+0.12\,g$ | `INSTANT LOAD` (Gelb) | Gewicht sofort beim Aufprall übertragen — weniger Gelenkschutz |
| $< -0.10\,g$ | `EARLY UNLOAD` (Gelb) | Gewicht verlagert sich bereits vor Stabilisierung zum nächsten Fuß |

**Sprunggelenk-Stoßdämpfung + Abrollumkehr** (200-ms-Fenster, 10 Samples von `gRoll`):

> **Messtechnischer Hinweis:** `gRoll` misst die Rotation des *Schuhsegments* um die Roll-Achse des Sensors, nicht direkt den Subtalargelenk-Eversionswinkel. `rollIntegral` ist ein Fußrotations-Proxy für Pronations-Stoßdämpfung; die Umkehrprüfung ist ein Proxy für den Pronation→Supination-Zyklus, der den Windlass-Mechanismus vorspannt. Beide dienen als Trainingsindikatoren, keine anatomischen Gelenkmessungen.

Fußroll-Integral über die ersten 100 ms (Samples 0–4):

$$\text{rollIntegral} = \left\lvert\sum_{i=0}^{4} \omega_{\text{roll},i} \times 0.02\,\text{s}\right\rvert \quad [\text{Grad}]$$

Abrollumkehr-Prüfung — Vorzeichenwechsel zwischen früher (Samples 0–3) und später (Samples 6–9) Phase:

$$\text{rollReversal} = \lvert\overline{\omega}_{0-3}\rvert > 8°/\text{s} \quad\text{UND}\quad \overline{\omega}_{0-3} \cdot \overline{\omega}_{6-9} < 0$$

| Bedingung | Badge | Biomechanische Interpretation |
| :---: | :---: | :--- |
| rollReversal = wahr | `RIGID LEVER` (Grün) | Abrollumkehr erkannt — Proxy für Pronation→Supination-Vorspannung (Windlass-Mechanismus) |
| rollIntegral $> 4°$ | `ANKLE FLEX` (Grün) | Fußrollimpuls erkannt — Proxy für stoßabsorbierende Pronation |
| rollIntegral $1°\text{–}4°$ | `MODERATE ROLL` (Gelb) | Etwas Fußmobilität, könnte erhöht werden |
| rollIntegral $< 1°$ | `STIFF ANKLE` (Gelb) | Minimales Abrollen — Aufprall wahrscheinlich in die kinetische Kette weitergeleitet |

---

### H. Tempo-normiertes Gewichtstransfer-Timing (Delay Ramp)

Die Verzögerungsrampe misst, wie schnell das Gewicht nach dem Fußkontakt übertragen wurde — ausgedrückt als Bruchteil des aktuellen Schrittintervalls. Die Metrik passt sich dadurch automatisch an das Musiktempo an.

**Berechnung:** Nach jedem Schrittauslöser wird ein 500-ms-Überwachungsfenster gestartet. Das Fenster gilt als abgeschlossen, wenn $|aZ - 1{,}0| < 0{,}15\,g$ UND $|\omega_{\text{pitch}}| < 40°/\text{s}$ für 2 aufeinanderfolgende Samples erfüllt sind.

$$\text{ratio} = \frac{t_{\text{ramp}}}{t_{\text{step}}}$$

| Bedingung | Richtung | Badge | Bedeutung |
| :---: | :---: | :---: | :--- |
| ratio 0,12–0,38 | FORWARD | `DELAYED ✓` (Grün) | WCS-typisches Schweben — Gewicht kommt nach dem Fuß an |
| ratio < 0,12 | FORWARD | `QUICK` (Gelb) | Sofortige Gewichtsübernahme — mechanisch, kein musikalischer Atem |
| ratio > 0,38 | FORWARD | `LATE` (Gelb) | Gewicht kam nie vollständig an — übermäßiges Schweben (nur ADV) |
| ratio 0,18–0,50 | BACKWARD | `DELAYED ✓` (Grün) | WCS-typisches kontrolliertes Einsinken |
| ratio < 0,18 | BACKWARD | `QUICK` (Gelb) | Zu schnelle Gewichtsübernahme beim Rückwärtsschritt |
| ratio > 0,50 | BACKWARD | `LATE` (Gelb) | Übermäßiges Schweben beim Rückwärtsschritt (nur ADV) |

---

## 3. Vollständige Schwellenwert- & Badge-Referenz

| Metrik / Parameter | Wert / Bereich | Visuelles Badge / Zustand | Audio-Biofeedback |
| :--- | :--- | :--- | :--- |
| **Fersenzone — kontrolliert** | $\theta \ge +6°$, Jerk $\le 130$ g/s | `HEEL STRIKE ✓` (Grün) | Kein |
| **Fersenzone — abrupt** | $\theta \ge +6°$, Jerk $> 130$ g/s | `HEEL SLAM ⚠` (Rot) | 1200-Hz-Klick |
| **Zehenzone — kontrolliert** | $\theta < -6°$, Jerk $\le 130$ g/s | `TOE-FIRST ✓` (Grün) | Kein |
| **Zehenzone — abrupt** | $\theta < -6°$, Jerk $> 130$ g/s | `TOE JAM ⚠` (Rot) | 1200-Hz-Klick |
| **Mehrdeutig — weich** | $-6° \le \theta < +6°$, Jerk $\le 55$ g/s | `SOFT ✓` (Grün) | Kein |
| **Mehrdeutig — moderat** | $-6° \le \theta < +6°$, Jerk $55\text{–}130$ g/s | `MODERATE` (Gelb) | Kein |
| **Mehrdeutig — hart** | $-6° \le \theta < +6°$, Jerk $> 130$ g/s | `HARD IMPACT ⚠` (Rot) | 1200-Hz-Klick |
| **BRUSH+HEEL-Neuklassifikation** | mehrdeutig → zweites aZ $> 1{,}05\,g$ + accelAngle $> 8°$ innerhalb 200 ms | `BRUSH+HEEL` (Grün) → ➡ FWD | Kein |
| **Standbein-Abstoß (vorwärts, optimal)** | BACKWARD letzter Schritt + $-\omega_{\text{pitch}} \ge 200^\circ/\text{s}$ UND $aY > 0.15g$ | `🚀 POWER PUSH` (Grün) — hält 400 ms | Kein |
| **Standbein-Abstoß (rückwärts/Anker, optimal)** | FORWARD letzter Schritt + $-\omega_{\text{pitch}} \ge 160^\circ/\text{s}$ UND $aY > 0.15g$ | `🚀 POWER PUSH` (Grün) — hält 400 ms | Kein |
| **Standbein-Abstoß (schwach)** | Beide Richtungen, $120\text{–}159/199^\circ/\text{s}$ UND $aY > 0.15g$ | `↗ PUSH` (Gelb) — hält 400 ms | Kein |
| **Aufprall-Jerk ($J_{\text{impact}}$)** | $> 130\text{ g/s}$ | Karten-Rand blinkt | 500-Hz-Aufprallklick (80 ms) |
| **Doppelstandphase — Optimal** | 15% bis 60% | `OPTIMAL ROLL` (Grün) | Kein |
| **Doppelstandphase — Hetzen** | $< 15\%$ | `HECTIC` (Gelb) | Kein |
| **Doppelstandphase — Träge** | $> \text{sluggishThr}$ (60–72 %, tempoadaptiv) | `SLUGGISH` (Gelb) | Kein |
| **Gewichtstransfer — Progressiv** | loadRise $> 0.12\,g$ | `SMOOTH LOAD` (Grün) | Kein |
| **Gewichtstransfer — Sofort** | $-0.10 \le$ loadRise $\le 0.12$ | `INSTANT LOAD` (Gelb) | Kein |
| **Gewichtstransfer — Frühzeitig** | loadRise $< -0.10\,g$ | `EARLY UNLOAD` (Gelb) | Kein |
| **Verzögerungsrampe — VW verzögert** | ratio 0,12–0,38 (VW) | `DELAYED ✓` (Grün) | Kein |
| **Verzögerungsrampe — VW schnell** | ratio $< 0.12$ (VW) | `QUICK` (Gelb) | Kein |
| **Verzögerungsrampe — VW spät** | ratio $> 0.38$ (VW) | `LATE` (Gelb) | Nur ADV |
| **Verzögerungsrampe — RW verzögert** | ratio 0,18–0,50 (RW) | `DELAYED ✓` (Grün) | Kein |
| **Verzögerungsrampe — RW schnell** | ratio $< 0.18$ (RW) | `QUICK` (Gelb) | Kein |
| **Verzögerungsrampe — RW spät** | ratio $> 0.50$ (RW) | `LATE` (Gelb) | Nur ADV |
| **Rigid Lever** | Pronation $> 8°/\text{s}$ UND Vorzeichenumkehr in 200 ms | `RIGID LEVER` (Grün) | Kein |
| **Sprunggelenk-Stoßdämpfung** | rollIntegral $> 4°$ (keine Umkehr) | `ANKLE FLEX` (Grün) | Kein |
| **Sprunggelenk-Steifigkeit** | rollIntegral $< 1°$ | `STIFF ANKLE` (Gelb) | Kein |
| **Ball→Ferse — optimal** | $\theta_{T-1} < -2°$, lateMean $> 3°$ | `BALL→HEEL ✓` (Grün) | Kein |
| **Ball→Ferse — partiell** | $\theta_{T-1} < 0°$, lateMean $> 0°$ | `PARTIAL ROLL` (Gelb) | Kein |
| **Ball→Ferse — Ferse zuerst** | $\theta_{T-1} \ge 0°$ | `HEEL-FIRST` (Gelb) | Kein |
| **Ball→Ferse — nur Ballen** | $\theta_{T-1} < 0°$, lateMean $\le 0°$ | `BALL ONLY ⚠` (Rot) | Kein |
| **Ferse→Ball — optimal** | earlyMean $> 2°$, drop $> 3°$ | `HEEL→BALL ✓` (Grün) | Kein |
| **Ferse→Ball — partiell** | earlyMean $> 0°$, drop $> 1°$ | `PARTIAL ROLL` (Gelb) | Kein |
| **Ferse→Ball — blockiert** | earlyMean $> 2°$, drop $\le 1°$ | `HEEL STUCK ⚠` (Rot) | Kein |
| **Zehe→Ferse — optimal (Ballensch.)** | earlyMean $< -2°$, rise $> 3°$ | `TOE→HEEL ✓` (Grün) | Kein |
| **Zehe→Ferse — partiell (Ballensch.)** | earlyMean $< 0°$, rise $> 1°$ | `PARTIAL ROLL` (Gelb) | Kein |
| **Ferse→Ball — Flachfuß** | earlyMean $\le 0°$, rise $\le 1°$ | `FLAT-FOOT` (Gelb) | Kein |
| **Roll-Glattheit — sauber** | rollVar < 130 | `CLEAN ROLL ✓` (Grün) | Kein |
| **Roll-Glattheit — moderat** | rollVar 130–300 | `MODERATE ROLL` (Gelb) | Kein |
| **Roll-Glattheit — Slapping** | rollVar > 300 | `SLAPPING` (Rot) | Kein |
| **SDR — gut gedämpft** | SDR > 0,65 | `ABSORBING ✓` (Grün) — nur ADV + Pelvis | Kein |
| **SDR — partiell** | SDR 0,35–0,65 | `PARTIAL SDR` (Gelb) — nur ADV + Pelvis | Kein |
| **SDR — steif** | SDR < 0,35 | `STIFF` (Rot) — nur ADV + Pelvis | Kein |
| **SETTLE — gesund** | 10–32 % des Schrittintervalls (max. 220 ms) | `SETTLING ✓ Xms` (Grün) — nur ADV + Pelvis | Kein |
| **SETTLE — starr** | < 10 % des Schrittintervalls | `QUICK Xms` (Gelb) — nur ADV + Pelvis | Kein |
| **SETTLE — verzögert** | > 32 % des Schrittintervalls (max. 220 ms) | `SLOW Xms` (Gelb) — nur ADV + Pelvis | Kein |
| **GND-Score — gut** | ≥ 65 | Grounding-Kachel grün — nur ADV | Kein |
| **GND-Score — mittel** | ≥ 35 | Grounding-Kachel gelb — nur ADV | Kein |
| **GND-Score — schwach** | < 35 | Grounding-Kachel rot — nur ADV | Kein |
| **Pro-Fuß-Sperrzeitfenster** | $180\text{–}320\text{ ms}$ (kadenzadaptiv) + Alternierungswächter | Unterdrückt Doppelauslösung | Kein |

---

### I. Ball-to-Heel Anker-Progression

Bei einem gut ausgeführten Rückwärts-Anker setzt der Fuß zunächst auf dem Ballen auf (θ negativ — Plantarflexion) und senkt sich dann zur Ferse, während das Körpergewicht einsinkt. Der Sensor quantifiziert diese Progression durch Tracking des Fußneigungswinkels θ während eines tempoadaptiven Fensters nach jedem Rückwärtsschritt.

**Fenster:** `anchorWindowMs = clamp(stepDurationMs × 1,05; 280 ms; 900 ms)` — tempoadaptiv, unabhängig vom Becken-Anchor-Settle (der ein tempoadaptives Fenster von 280–400 ms nach dem letzten Rückwärtsschritt verwendet).

**Berechnung:**

Zum Zeitpunkt des Rückwärtsschritt-Auslösers wird der Fußwinkel aus T-1 (vor dem Reset) als $\theta_{T-1}$ gespeichert. Der CF-Winkel wird dann auf 0° zurückgesetzt, um Aufprallverzerrungen zu verhindern. Der Mittelwert der zweiten Fensterhälfte der Post-Reset-Samples ergibt $\theta_{\text{spät}}$ und repräsentiert, wo der Fuß zur Ruhe kommt:

$$\theta_{T-1} = \text{Fußwinkel zum Auslösezeitpunkt, vor Reset (T-1-Snapshot)}$$

$$\theta_{\text{spät}} = \overline{\theta}_{[\lfloor n/2 \rfloor,\,n]} \quad \text{(zweite Hälfte des 280–900 ms Post-Reset-Fensters)}$$

| Bedingung | Badge | Biomechanische Bedeutung |
| :---: | :---: | :--- |
| $\theta_{T-1} \ge 0°$ | `HEEL-FIRST` (Gelb) | Fuß kam flach oder Ferse-zuerst an — kein initialer Ballenkontakt |
| $\theta_{T-1} < -2°$ UND $\theta_{\text{spät}} > 3°$ | `BALL→HEEL ✓` (Grün) | Klassische WCS-Landung: Ballen-zuerst, Ferse senkt sich — kontrollierte exzentrische Belastung |
| $\theta_{T-1} < 0°$ UND $\theta_{\text{spät}} > 0°$ | `PARTIAL ROLL` (Gelb) | Ballenkontakt vorhanden, Ferse senkt sich teilweise im Post-Reset-Fenster |
| Sonst | `BALL ONLY ⚠` (Rot) | θ blieb durchgehend negativ — Fuß blieb auf dem Ballen; typisch bei Hetzen oder unvollständigem Gewichtstransfer |

**Mindestanzahl Samples:** Die Auswertung erfordert mindestens 6 Samples im Fenster (~120 ms). Wenn ein neuer Schritt auslöst, bevor das Fenster schließt, wird `anchorThetaActive` zurückgesetzt und kein Badge ausgegeben.

---

### J. Ferse-zu-Ballen / Zehe-zu-Ferse Vorwärts-Progression

Die Vorwärtsschritt-Rollmetrik erkennt das sagittale Sprunggelenk-Artikulationsmuster nach jedem Vorwärtsschritt. Im WCS gibt es zwei gültige Muster — Fersenschritt (Dorsalflexion beim Kontakt, θ > 0°) und Ballenschritt (Plantarflexion beim Kontakt, θ < 0°) — und das Badge identifiziert, welches vorlag und ob das Abrollen vollständig war.

**Fenster:** `heelBallWindowMs = clamp(stepDurationMs, 280 ms, 500 ms)`.

**Berechnung:**

$$\theta_{\text{früh}} = \overline{\theta}_{[0,\,\lfloor n/2 \rfloor]} \qquad \theta_{\text{spät}} = \overline{\theta}_{[\lfloor n/2 \rfloor,\,n]}$$

$$\text{drop} = \theta_{\text{früh}} - \theta_{\text{spät}} \quad \text{(positiv = θ sank = Ferse rollt zum Ballen)}$$
$$\text{rise} = \theta_{\text{spät}} - \theta_{\text{früh}} \quad \text{(positiv = θ stieg = Ballen rollt zur Ferse)}$$

| Bedingung | Badge | Biomechanische Bedeutung |
| :---: | :---: | :--- |
| **Fersenschritt:** $\theta_{\text{früh}} > 2°$ UND drop $> 3°$ | `HEEL→BALL ✓` (Grün) | Ferse kontaktiert zuerst, Sprunggelenk rollt nach vorne ab — korrekter Fersenschritt-WCS-Gang |
| **Fersenschritt:** $\theta_{\text{früh}} > 0°$ UND drop $> 1°$ | `PARTIAL ROLL` (Gelb) | Etwas Vorwärtsrollen vorhanden, aber unvollständig |
| **Fersenschritt:** $\theta_{\text{früh}} > 2°$ UND drop $\le 1°$ | `HEEL STUCK ⚠` (Rot) | Ferse kontaktiert, aber Fuß blieb dorsalflektiert — blockiertes Sprunggelenk |
| **Ballenschritt:** $\theta_{\text{früh}} < -2°$ UND rise $> 3°$ | `TOE→HEEL ✓` (Grün) | Ballen/Zehe kontaktiert zuerst, Ferse senkt sich ab — korrekter Ballenschritt (Triple Step, Coaster, Tap) |
| **Ballenschritt:** $\theta_{\text{früh}} < 0°$ UND rise $> 1°$ | `PARTIAL ROLL` (Gelb) | Ballenkontakt vorhanden, aber Fersenabsenken unvollständig |
| Sonst | `FLAT-FOOT` (Gelb) | Kein eindeutiges Kontaktmuster — flache oder mehrdeutige Landung |

**Mindestanzahl Samples:** Mindestens 6 Samples erforderlich (~120 ms). Ein neuer Schritt setzt `heelBallActive` zurück.

---

### K. Roll-Glattheit pro Schritt (rollSmoothBadge)

Das `ROLL`-Badge erscheint in der Letzter-Schritt-Kachel bei **allen Trainingslevels** und bewertet die Gleichmäßigkeit der Fußabrollbewegung während der Standphase — ein direktes Maß für die Qualität der exzentrischen Vorfußkontrolle.

**Berechnung:** Die Metrik wird über ein 350-ms-Fenster der Standphase nach dem Fußkontakt ausgewertet:

$$\text{rollVar} = \frac{\sum_{i} (\Delta g_{\text{Pitch},i})^2}{n}$$

Dabei ist $\Delta g_{\text{Pitch},i}$ die Änderung des sagittalen Gyroskop-Winkels zwischen aufeinanderfolgenden Samples — hohe quadratische Varianz zeigt ruckartige, unkontrollierte Vorfuß-Absenkbewegungen an.

| Zustand | Bedingung | Farbe | Biomechanische Bedeutung |
| :---: | :---: | :---: | :--- |
| `CLEAN ROLL ✓` | rollVar < 130 | Grün | Gleichmäßige, kontrollierte Abrollbewegung — M. tibialis anterior aktiv |
| `MODERATE ROLL` | rollVar 130–300 | Gelb | Teilweise Kontrolle; Abrollbewegung könnte gleichmäßiger sein |
| `SLAPPING` | rollVar > 300 | Rot | Unkontrolliertes Abkippen des Vorfußes (siehe unten) |
| `— ROLL` | Noch kein Schritt | Grau | Initialer Zustand |

#### Mechanismus des SLAPPING-Phänomens

SLAPPING entsteht **nicht** durch die Stärke des Fersenaufschlags selbst. Die Ursache liegt im fehlenden exzentrischen Bremsen durch den **M. tibialis anterior** nach dem Fersenkontakt:

1. Der Fersenkontakt erfolgt (θ > +8°) — dieser Moment ist normal
2. Der M. tibialis anterior bremst den Vorfuß nach dem Fersenkontakt nicht exzentrisch ab
3. Der Vorfuß kippt unkontrolliert auf den Boden → steile Spitze in der $g_{\text{Pitch}}$-Winkelgeschwindigkeit → hohe rollVar

**Richtungsabhängiges Muster:**

- **Count 1+2 (Fersenerstkontakt bei Vorwärtsschritten):** Typischerweise höhere rollVar → `SLAPPING`-Risiko erhöht, da der volle Fersenrocker durchlaufen wird
- **Count 3&4 (Ballenerstkontakt bei Rückwärtsschritten):** Typischerweise niedrigere rollVar → `CLEAN ROLL`, da der Fuß bereits in Plantarflexion landet

**Korrektur:** Bewusstes exzentrisches Abbremsen des Vorfußes nach dem Fersenkontakt — Ferse → Fußaußenkante → Ballen mit aktiver Sprunggelenksspannung. Statt den Vorfuß fallen zu lassen: aktive Dorsalflexionskontrolle des Sprunggelenks im Fersenrocker.

---

### L. Grounding-Kachel (nur ADV-Level)

Die Grounding-Kachel erscheint im Querformat neben dem Abroll-Dynamik-Graphen und ist ausschließlich im **ADV-Traininglevel** sichtbar. Sie aggregiert drei Metriken der Stoßdämpfungs- und Impulsübertragungs-Qualität der kinematischen Kette: SDR, SETTLE und ROLL (letzteres wird aus der Step-Kachel eingelesen). SDR und SETTLE erfordern den Beckensensor (ID 4).

---

#### SDR-Badge (Shock Damping Ratio — Stoßdämpfungsverhältnis)

Misst, wie viel des Fußaufprall-Jerks durch die Beinkette absorbiert wird, bevor er das Becken erreicht.

**Berechnung:** In einem 100-ms-Fenster nach dem Fußkontakt werden Spitzen-Jerk-Werte am Fuß ($J_{\text{Fuß,peak}}$) und am Becken (Spitzenwert der sagittalen Beckenbeschleunigung, $J_{\text{Becken,peak}}$) verglichen:

$$\text{SDR} = 1 - \frac{J_{\text{Becken,peak}}}{J_{\text{Fuß,peak}}}$$

Ein SDR von 1,0 bedeutet vollständige Dämpfung (kein Jerk erreicht das Becken); SDR = 0 bedeutet ungefilterte Weiterleitung.

| Badge | Bedingung | Farbe | Biomechanische Bedeutung |
| :---: | :---: | :---: | :--- |
| `ABSORBING ✓` | SDR > 0,65 | Grün | Beinkette dämpft Aufprall gut — Knie/Sprunggelenk absorbieren effektiv |
| `PARTIAL SDR` | SDR 0,35–0,65 | Gelb | Partielle Dämpfung; Aufprall wird teilweise weitergeleitet |
| `STIFF` | SDR < 0,35 | Rot | Stoß wird ungefiltert weitergeleitet — steife Beinkette |
| `— SDR` | Kein Pelvis-Sensor | Grau | Beckensensor nicht verbunden |

---

#### SETTLE-Badge (Impact-Absorptions-Latenz / Beinketten-Compliance)

Misst die Zeit vom Fußkontakt bis zum **ersten** Minimum der vertikalen Beckenbeschleunigung (`pAz`) — die frühe Stoßabsorptions-Reaktion der Beinkette (Loading-Response-Phase). Dies ist NICHT das vollständige WCS-Settle (das erst bei 60–90 % des Schrittintervalls nach vollständiger Gewichtsübertragung eintritt — erfasst durch den Anchor-Settle-Badge).

$$\text{SETTLE-Zeit} = t\!\left(\min(pA_z)\right) - t_{\text{Fußkontakt}} \quad [\text{ms}]$$

Ein gesundes Impact-Absorptions-Fenster von 10–32 % des aktuellen Schrittintervalls (Untergrenze 40 ms) zeigt an, dass Knie- und Hüftgelenk den Aufprall in der Loading-Response-Phase aktiv abfedern — nicht starr in die kinetische Kette weiterleiten. Das Suchfenster ist auf max. 220 ms begrenzt (`min(220 ms, t_Schritt × 0,35)`), um sicherzustellen, dass nur Dip 1 (Impact-Absorption) erfasst wird, nicht Dip 2 (vollständiges WCS-Settle).

| Badge | Bedingung | Farbe | Biomechanische Bedeutung |
| :---: | :---: | :---: | :--- |
| `SETTLING ✓ Xms` | `pdLo`–`pdHi` (10–32 % des Schrittintervalls) | Grün | Gesunde Beinketten-Compliance — kontrolliertes Einsinken |
| `QUICK Xms` | < `pdLo` (< 10 % des Schrittintervalls, min. 40 ms) | Gelb | Starre Absorption, kein messbares Verzögerungsplateau; zeigt auch `QUICK 0ms` bei völlig starrer Hüftabsorption (kein messbarer Dip in Becken-aZ) |
| `SLOW Xms` | > `pdHi` (> 32 % des Schrittintervalls, max. 220 ms) | Gelb | Sehr verzögerte Reaktion — träge Gelenkaktivierung |
| `— SETTLE` | Kein Pelvis-Sensor | Grau | Beckensensor nicht verbunden |

---

#### GND-Score (Grounding-Gesamtscore)

Ein gewichteter Composite-Score (0–100) aus allen drei Grounding-Metriken:

$$\text{GND} = \text{SDR-Score} \times 0{,}40 + \text{SETTLE-Score} \times 0{,}30 + \text{ROLL-Score} \times 0{,}30$$

Das ROLL-Badge (aus der Step-Kachel, §K) fließt mit 30 % in den GND-Score ein; Step-Kachel und Grounding-Kachel teilen sich diese Metrik.

| GND-Score | Farbe | Bedeutung |
| :---: | :---: | :--- |
| ≥ 65 | Grün | Gute Gesamtdämpfung und Bodenkontakt-Qualität |
| ≥ 35 | Gelb | Verbesserungspotenzial in mindestens einer Teilmetrik |
| < 35 | Rot | Deutliche Schwächen in der Stoßdämpfungskette |

Ein **Fortschrittsbalken** in der Grounding-Kachel visualisiert den GND-Score. Der Score wird nach jedem Schritt aktualisiert, für den SDR- und SETTLE-Daten vorliegen.

---

## 4. Signalfilterung & Sperrzeitkonzept (Lockout)

Um falsche sekundäre Schrittauslöser durch Mikrotipps, Fußentlastungen oder Bodenschwingungen zu verhindern, führt die DSP-Pipeline (Digital Signal Processing) ein **Dreistufiges Filterungs- & Sperrzeitkonzept** aus:

1. **Transientes Signal-Kandidaten-Sensing:**
   Jeder Fuß qualifiziert sich unabhängig als Aufprallkandidat über ODER-Logik:

   $$\text{signal}_{\text{foot}} = \bigl(\lvert aZ\rvert > \theta_{\text{thr}} \quad\mathbf{UND}\quad \text{preJerk} > 2\bigr) \quad\mathbf{ODER}\quad \bigl(\lvert\omega_{\text{pitch}}\rvert > 80\,\text{deg/s} \quad\mathbf{UND}\quad \text{preJerk} > 8\bigr)$$

   Dabei gilt $\theta_{\text{thr}} = 0{,}92\,g$ wenn $t_{\text{Schritt}} > 800\,\text{ms}$ (Trainingstempo $< 75\,\text{BPM}$), sonst $0{,}95\,g$. Das `preJerk > 2`-Gate auf dem aZ-Pfad unterdrückt langsame Standfuß-Gewichtsdrift (typischer preJerk 0,5–2), lässt aber echte Aufprallimpulse (preJerk typisch 5–30+) durch.

   Das `preJerk`-Gate (`|aZ_t - aZ_{t-1}| / Δt > 8`) auf dem Gyro-Pfad unterdrückt Abhebebewegungs-Artefakte. Wenn beide Füße im gleichen Frame signalisieren, wird der dominante Fuß nach maximaler Bodenreaktionskraft ausgewählt: $\text{detectedFoot} = \arg\max(|aZ_L|, |aZ_R|)$.

   > **Hinweis:** Der Gyro-Pfad erkennt korrekt flache Ballenauftrittte (aZ unter Schwellenwert) über `|gPitch| > 80°/s`. Beobachteter Minimalwert preJerk in echten WCS-Schritten: **6,0** — deutlich über dem Gate von 2,0. Schrittbalance in der Praxis: L/R-Zähler bleiben bei Einzel- und Triple-Steps ausgeglichen.

2. **Gegenfuß-Plausibilitätsprüfung (Phantom-Trigger-Unterdrückung):**
   Nach der Kandidatenauswahl wird der Auslöser verworfen, wenn der erkannte Fuß $|aZ| < 0{,}90\,g$ zeigt, während der Gegenfuß $|aZ| > 1{,}1\,g$ aufweist (klar das belastete Standbein). Dadurch werden Schwungphasenartefakte eliminiert — Wischbewegungen, Bodentipps oder abrupte Abhebbewegungen, die einen hohen Jerk-Impuls ohne echte Gewichtsübertragung erzeugen. Ohne diese Prüfung kann ein kurzes Streifen des Schwungfußes ein falsches `HARD IMPACT ⚠`-Badge auslösen (Beispiel: 48 g/s Phantom-Trigger bei Sekunde 18, während der Standfuß die volle Last trug).

3. **Pro-Fuß Kadenz-Adaptives Sperrzeitfenster & Alternierungswächter:**
   * **Sperrzeitkonzept:** Das System verwaltet unabhängige Letztschritt-Zeitstempel für jedes Bein (`lastStepTimeLeft` und `lastStepTimeRight`). Wenn ein Schrittkandidat erkannt wird, prüft die Zustandsmaschine, ob die seit dem letzten Schritt *an diesem spezifischen Bein* verstrichene Zeit kleiner als das dynamische Sperrzeitfenster ist.
   * **Kadenz-Adaptives Fenster:** $t_{\text{lockout}} = \text{clamp}(t_{\text{step}} \times 0.55,\ 180\text{ ms},\ 320\text{ ms})$. Bei 120 BPM → 275 ms; bei 160 BPM → 206 ms; bei 200 BPM → 180 ms (Untergrenze).
   * **Alternierungswächter:** Schritte müssen alternieren (`Links → Rechts → Links`). Gleicher Fuß zweimal ohne Gegenfuß-Kontakt dazwischen wird als Artefakt verworfen.
   * **Globaler Cross-Fuß-Lockout (130 ms):** Jeder Schrittauslöser — unabhängig vom Fuß — wird verworfen, wenn er innerhalb von 130 ms nach dem letzten bestätigten Schritt eintrifft. Dieser übergreifende Lockout verhindert False-Trigger des ruhenden Fußes (~0,92–0,95 g Oszillation) kurz nach einem echten Schritt: Das per-Fuß-Sperrzeitfenster des Gegenfußes ist veraltet und würde ihn nicht blockieren. `lastStepTimestamp` wird bei jedem bestätigten Schritt aktualisiert und gilt für beide Füße.

---

## 5. UI-Architektur & Kamera-HUD-Overlay

Das Solo-Training-Dashboard ist für die mobile Browser-Nutzung optimiert (Tablets/Smartphones auf einem Stativ, dem Tänzer zugewandt):

* **Transparentes WebRTC-Kamera-HUD:** Das HTML-Videoelement ist im Hintergrund fixiert (`z-index: -1`). Dashboard-Kacheln verwenden **35% Hintergrunddeckkraft** (`rgba(10, 14, 22, 0.35)`), sodass der Tänzer seine Körperausrichtung direkt hinter den Live-Telemetriekurven sehen kann.
* **Responsives Hoch- & Querformat-Split:**
  * **Hochformat:** Vertikales Layout für die Stativ-Ansicht.
  * **Querformat:** 2-Spalten-Ansicht (Live-Neigungsgraph links, 2×2-Metrikkacheln rechts) ohne vertikales Scrollen.
* **Steuerungs-Header:**
  * `📷 CAM`: Aktiviert den WebRTC-Benutzermedien-Videostream.
  * `🔄 FLIP`: Wechselt zwischen Front- (`user`) und Rückkamera (`environment`).
  * `⛶ FULL`: Aktiviert die native Vollbild-API.
  * `📐 ZERO`: Kalibriert statische Neigungswinkel beider Füße neu.
  * `🔊 Audio`: Schaltet synthetische Web-Audio-API-Biofeedback-Töne EIN/AUS.

> 📸 **[Screenshot: Solo-Dashboard mit aktivem Kamera-HUD — halbtransparente Datenkarten überlagern die Live-Körperansicht im Querformat]**

---

## 6. Becken-Metriken (Solo-Dashboard)

Wenn der Beckensensor (ID 4) verbunden ist, zeigt das Solo-Dashboard sechs zusätzliche Badge-Kacheln auf Basis der Beckenkinematik. Der Sensor sitzt auf einem Gürtel am Kreuzbein und überträgt 3-Achsen-Beschleunigung (`pAx`, `pAy`, `pAz`) sowie Gier-Gyro (`pYaw`) mit 200 Hz.

### gYaw-Kurve im Live-Abroll-Dynamik-Graphen

Eine **gestrichelte gelbe Linie** im Live-Abroll-Dynamik-Graphen zeigt die gemessene Hüftgier-Rate (`pYaw` des Beckensensors), auf den gleichen Anzeigebereich wie die Fußneigungskurven skaliert. Die Kurve ist sichtbar, sobald der Beckensensor verbunden ist, und gibt in Echtzeit Aufschluss darüber, wie das Hüftrotations-Timing mit den Fußabroll-Ereignissen zusammenfällt.

> 📸 **[Screenshot: Abroll-Dynamik-Graph mit linker (Cyan) und rechter (Magenta) Fußneigungskurve sowie übergelagerter gestrichelter gelber Hüftgier-Kurve]**

### Hip Activation (Hüftaktivierung)

Misst die Spitzen-Rotationsgeschwindigkeit des Beckens um die Vertikalachse — ein Proxy für aktives Hüftengagement bei jedem Schritt.

**Signalverarbeitung:**
- Gleitpuffer: `gYawAbsHistory` — 25 Samples (~0,5 s Fenster) der Beträge von `pYaw`
- Spitzenwertextraktion: `gYawPeak = max(gYawAbsHistory)`
- Exponentielle Glättung: `hipActSmoothed = hipActSmoothed × 0,9 + gYawPeak × 0,1` (τ ≈ 2 s)

**Schwellenwerte (tempoabhängig):**

$$\text{scaleFactor} = \frac{500}{\max(400,\; \text{stepDurationMs})}$$

| Zustand | Schwellenwert | Farbe |
| :--- | :--- | :--- |
| ACTIVE | `hipActSmoothed ≥ round(60 × scaleFactor)` °/s | Grün |
| MODERATE | `hipActSmoothed ≥ round(25 × scaleFactor)` °/s | Gelb |
| STIFF HIPS | unterhalb MODERATE | Rot |

Referenzwerte bei 500 ms/Schritt (scaleFactor = 1,0): ACTIVE ≥ 60 °/s, MODERATE ≥ 25 °/s. Bei langsamem Tempo (700 ms/Schritt, scaleFactor ≈ 0,71): ACTIVE ≥ 43 °/s, MODERATE ≥ 18 °/s. Bei schnellem Tempo (400 ms/Schritt, scaleFactor = 1,25): ACTIVE ≥ 75 °/s, MODERATE ≥ 31 °/s.

### Lateral Stability (Seitliche Stabilität)

Überwacht die seitliche Schwingung des Beckens (mediolateral).

- Puffer: `aXPHistory` — 50 Samples (~1 s) der lateralen Beschleunigung `pAx`
- Metrik: `aXVar = variance(aXPHistory)`

| Zustand | Bedingung | Farbe |
| :--- | :--- | :--- |
| STABLE | `aXVar < 0,004` | Grün |
| SLIGHT SWAY | `aXVar < 0,015` | Gelb |
| LATERAL SWAY | `aXVar ≥ 0,015` | Rot |

### Hip-Foot Coupling (Hüft-Fuß-Kopplung)

Bewertet, ob die Beckenrotation vor dem Fußkontakt einsetzt (WCS-Ideal) oder hinterher.

- Puffer: `gYawTimedBuf` — Ringpuffer mit `{Wert, Zeitstempel}`-Einträgen der letzten 600 ms
- Auslöser: bei jedem bestätigten Schritt — Zeitstempel des maximalen `pYaw`-Betrags im Puffer ermitteln
- Vorlaufzeit: `leadMs = Schritt-Zeitstempel − Peak-Zeitstempel`

| Zustand | Bedingung | Farbe |
| :--- | :--- | :--- |
| HIP LEADS | `leadMs > 100 ms` | Grün |
| IN SYNC | `leadMs > 40 ms` | Gelb |
| HIP LAGS | `leadMs ≤ 40 ms` | Rot |

### Vertical Bounce (Vertikales Hüpfen)

Erkennt übermäßige vertikale Schwingung des Beckens — ein Zeichen für springende oder fersenbelastete Bewegung statt der geerdeten, ebenen Haltung im WCS.

- Puffer: `aZPDynHistory` — 50 Samples (~1 s) von `(pAz − 1,0)` (Schwerkraft abgezogen)
- Metrik: `aZVar = variance(aZPDynHistory)`

| Zustand | Bedingung | Farbe |
| :--- | :--- | :--- |
| GROUNDED | `aZVar < 0,006` | Grün |
| SLIGHT BOUNCE | `aZVar < 0,030` | Gelb |
| BOUNCY | `aZVar ≥ 0,030` | Rot |

### Pelvic Tilt (Beckenneigung)

Erkennt Anterior Tilt (Hohlkreuz / Lordose) als kontinuierliche Haltungsmetrik. Der sagittale Neigungswinkel wird aus den Beckenbeschleunigungsachsen `pAy` (sagittal) und `pA` (vertikal) abgeleitet — dasselbe Prinzip wie beim Fußneigungswinkel:

$$\theta_{\text{Becken}} = \arctan2(aY_P,\; aZ_P) \cdot \frac{180°}{\pi} - \theta_{\text{Offset}}$$

Der Offset wird beim Drücken von `📐 ZERO` erfasst (Tänzer steht in neutraler Tanzposition). Der Rohwinkel wird IIR-geglättet (α = 0,08, τ ≈ 250 ms) um Schritterschütterungen zu unterdrücken.

| Zustand | Bedingung | Farbe | Biomechanische Bedeutung |
| :--- | :--- | :--- | :--- |
| ALIGNED | $|\theta_{\text{Becken}}| \le 6°$ | Grün | Neutrales Becken — Lendenwirbelsäule geschützt, Core aktiv |
| SLIGHT ARCH | $6° < \theta_{\text{Becken}} \le 12°$ | Gelb | Leichter Anterior Tilt — Verbindung noch funktional, aber Core-Spannung reduziert |
| LORDOSIS ⚠ | $\theta_{\text{Becken}} > 12°$ | Rot | Deutliche Überstreckung — Partnerverbindung gestört, Verletzungsrisiko |
| TUCKED | $\theta_{\text{Becken}} < -6°$ | Gelb | Übermäßiger Posterior Tilt — Überkorrektur reduziert Hüftbeweglichkeit und Schwung |

**Kalibrierung:** `📐 ZERO` muss in neutraler Tanzposition gedrückt werden (aufrecht, Gewicht ausgeglichen). Der Becken-Offset wird gleichzeitig mit den Fuß-Offsets genullt.

### Anchor Settle (Anker-Einschwingen)

Bewertet die Qualität des Abbremsens und Einschwingens des Beckens nach jedem Anker-Rückwärtsschritt — der entscheidende Moment, in dem WCS-Dehnung in geerdet kontrollierten Gewichtstransfer umgewandelt wird.

**Auslöser:** Jeder bestätigte BACKWARD-Schritt (unabhängig vom Pelvis-Sensor-Status) öffnet ein frisches Auswertungsfenster. Das Fenster bleibt offen, solange AMBIGUOUS-Schritte innerhalb von 2 Sekunden folgen und bwd < 2. Der Timer feuert nach einem tempoadaptiven Fenster nach dem letzten relevanten Rückwärtsschritt:

$$t_{\text{eval}} = t_{\text{letzter BACKWARD-Schritt}} + t_{\text{Settle-Fenster}}$$

$$t_{\text{Settle-Fenster}} = \text{clamp}(\text{stepDurationMs} \times 0{,}55,\ 280\,\text{ms},\ 400\,\text{ms})$$

Typische Werte: 367 ms bei 90 BPM · 400 ms bei 80 BPM · 400 ms bei 75 BPM (begrenzt). Das Fenster schließt früher, wenn der erste Vorwärtsschritt erkannt wird (erzwingt sofortige Auswertung). Der Score bleibt 3 Sekunden sichtbar.

**Hintergrund des kürzeren Fensters:** Bei typischen WCS-Tempos (80–100 BPM) reichte das alte 700-ms-Fenster in den nächsten Beat hinein und erfasste die Übergangsbewegung der nächsten Figur statt des Anchor-Einschwingvorgangs. Das tempoadaptive Fenster schließt vor Count 1 der Folgephrase.

**Mindest-Samples:** 3 Pelvis-Datenpunkte erforderlich (Pelvis-Sensor überträgt bei ~7–12 Hz über WLAN; ein 280–400-ms-Fenster liefert unter normalen Bedingungen 2–5 Samples).

Gesammelte Signale: sagittale Beckenbeschleunigung (`aSagP`), Hüftgier-Rate (`gYawP`) und laterale Beckenbeschleunigung (`aLatP`) für Hip Settle.

**Score-Zusammensetzung (skaliert 0–100):**

$$\text{score} = \text{decelScore} \times 0{,}35 + \text{yawDampScore} \times 0{,}35 + \text{stabilScore} \times 0{,}30$$

| Komponente | Formel | Bedeutung |
| :--- | :--- | :--- |
| **decelScore** | `clamp((earlyRMS / (lateRMS + 0,01) − 1,0) / 1,5, 0, 1)` | Sagittales Abbremsen: Frühphasen-RMS höher als Spätphase |
| **yawDampScore** | `clamp((earlyYawRMS / (lateYawRMS + 0,5) − 1,0) / 1,5, 0, 1)` | Gier-Dämpfung: Hüftrotation stoppt nach Landung |
| **stabilScore** | `max(0, 1 − lateYawVariance / 400)` | Spät-Phasen-Stabilität: geringe Gier-Varianz in zweiter Fensterhälfte |

| Zustand | Bedingung | Farbe |
| :--- | :--- | :--- |
| ANCHORED | score ≥ 50 | Grün |
| SETTLING | score 30–49 | Gelb |
| UNSTABLE | score < 30 | Rot |

> 📸 **[Screenshot: Beckenkarte mit Anchor-Settle-Badge und numerischem Score (z. B. ANCHORED 74) in Grün nach einem Rückwärts-Ankerschritt]**

### Hip Settle (Hüft-Einschwingen)

Wird am Ende des Anchor-Settle-Fensters ausgewertet. Verwendet laterale Beckenbeschleunigung (`pAx`) aus der ersten Fensterhälfte, um die charakteristische seitliche Gewichtsverlagerung eines gelungenen Ankers zu erkennen.

- `earlyLatPeak` = maximaler Betrag von `pAx` in der ersten Fensterhälfte
- `lateLatVar` = Varianz von `pAx` in der zweiten Fensterhälfte

| Zustand | Bedingung | Farbe |
| :--- | :--- | :--- |
| HIP SETTLE ✓ | `earlyLatPeak > 0,10 g` UND `lateLatVar < 0,015` | Grün |
| SLIGHT SETTLE | `earlyLatPeak > 0,05 g` | Gelb |
| OVERSWING ⚠ | `earlyLatPeak > 0,30 g` UND `lateLatVar ≥ 0,015` | Gelb |
| NO HIP SETTLE | `earlyLatPeak ≤ 0,05 g` | Rot |

---

## 7. Torso- / Poise-Metriken (Solo-Dashboard)

> **⚠ Provisorisch.** Der Thorax-Sensor (ID 5) und sämtliche Schwellenwerte unten sind neu und **noch nicht WCS-validiert**. Alle Zahlen sind Kalibrier-Startwerte, die über den üblichen Validierungs-Prompt-Workflow an eigenen Aufnahmen justiert werden. Nur das qualitative Verhalten steht fest; die genauen Bänder verschieben sich noch.

Ist der Thorax-Sensor online, erscheinen vier Zeilen in der oberen linken Karte (geteilt mit den Becken-Zeilen; die Karte erscheint, sobald Becken **oder** Thorax online ist). Getragen hoch am oberen Rücken (C7–T1), montiert wie der Beckenknoten, überträgt vertikal `tA`, sagittal `tAy`, lateral `tAx` (Beschleunigung) sowie `tYaw`/`tG` (Gyro) bei 200 Hz.

**Designprinzip — nur driftfrei.** Poise nutzt **schwerkraftreferenziertes Pitch/Roll** (aus dem Beschleunigungssensor) und **Beschleunigungs-Varianz**; bewusst **kein integriertes Yaw** (das ohne Magnetometer driftet). Torso-Becken-Rotationsseparation / Counter-Body-Movement ist daher hier *nicht* implementiert — siehe §7.5.

### Poise (sagittale Brustneigung)

- Genullt bei `📐 ZERO`, gedrückt in der **natürlichen Tanz-Bereitschaftshaltung** (ein ZERO tariert Füße, Becken & Thorax): `thoraxPitchOffset = atan2(tAy, tA)·180/π`.
- Pro Frame: `raw = atan2(tAy, tA)·180/π − thoraxPitchOffset`; IIR `s = s·0,92 + raw·0,08` (τ ≈ 0,9 s @ 50 Hz).

| Zustand | Bedingung (° von deiner Neutrallage) | Farbe |
| :--- | :--- | :--- |
| `UPRIGHT ✓` | `|s| ≤ 7` | Grün |
| `SLIGHT LEAN` | `+7 < s ≤ +13` | Gelb |
| `SLOUCHING ⚠` | `s > +13` | Rot |
| `LEANING BACK` | `−13 ≤ s < −7` | Gelb |
| `LEANING BACK ⚠` | `s < −13` | Rot |

> **Symmetrisch — und warum die ZERO-Haltung zählt:** Das Band ist symmetrisch, weil `0` deine **Tanz-Arbeitshaltung** ist, nicht die echte Senkrechte — du drückst `ZERO` in derselben natürlichen Haltung, in der auch Füße und Becken kalibriert werden (ein Knopf nullt alle drei). Abweichungen in beide Richtungen (weiteres Vorsacken oder Zurücklehnen) sind dann echte Fehler. **Nicht** aufrecht nullen und dann in die Tanzhaltung lehnen — das würde sowohl die Thorax-Poise- als auch die (symmetrische) Becken-Tilt-Basis verschieben, da beide die Segmentneigung im Raum relativ zur ZERO-Haltung messen.

> **Vorzeichen-Konvention:** auf Hardware bestätigt für Montage mit **USB/Anschluss nach unten** — Vorwärtslehnen → positives `s` (der sagittale Eingang `tAy` ist im Code negiert). Bei geänderter Montageorientierung neu prüfen und ggf. umdrehen. Die grüne ±Zone und alle Betrags-/Varianz-Badges sind unabhängig davon vorzeichenrobust.

### Level (seitliche Neigung)

`roll = atan2(tAx, tA)·180/π − thoraxRollOffset`, IIR 0,92/0,08.

| Zustand | Bedingung | Farbe |
| :--- | :--- | :--- |
| `LEVEL ✓` | `|roll| ≤ 6°` | Grün |
| `SLIGHT TILT` | `6° < |roll| ≤ 11°` | Gelb |
| `TILTED ⚠` | `|roll| > 11°` | Rot |

### Top-Line Quiet

Horizontal-Magnitude `h = √(tAy² + tAx²)`, Varianz über `aHorizTHistory` (50 Samples ≈ 1 s). **Tempo-adaptiv**: `tlScale = 500 / max(400, stepDurationMs)` (1,0 @120 BPM, bis 1,25 bei schnellem Tempo, < 1 bei langsam) — schnelleres Tanzen toleriert mehr Fuß-Impuls-Mikrobewegung.

| Zustand | Bedingung | Farbe |
| :--- | :--- | :--- |
| `QUIET ✓` | `var(h) < 0,010 × tlScale` | Grün |
| `SOME MOTION` | `0,010 × tlScale ≤ var(h) < 0,035 × tlScale` | Gelb |
| `RESTLESS ⚠` | `var(h) ≥ 0,035 × tlScale` | Rot |

### Torso–Becken-Stack (braucht Beckensensor)

`flex = thoraxPitchSmoothed − pelvicPitchSmoothed` (beide sind Abweichungen von ihrer eigenen ZERO-Neutrallage, `flex` ist also die relative sagittale Beugung). Asymmetrisch: Pike (Vorbeugen) spricht etwas früher an als Öffnen (Überstrecken).

| Zustand | Bedingung | Bedeutung | Farbe |
| :--- | :--- | :--- | :--- |
| `STACKED ✓` | `−5 ≤ flex ≤ +7` | Torso und Becken bewegen sich als eine Säule | Grün |
| `PIKING` | `+7 < flex ≤ +12` | Brust beginnt gegenüber dem Becken nach vorn zu klappen | Gelb |
| `PIKING ⚠` | `flex > +12` | Klares Einknicken in der Taille (Brust klappt nach vorn) | Rot |
| `OPENING` | `−10 ≤ flex < −5` | Brust öffnet sich gegenüber dem Becken nach hinten | Gelb |
| `OPENING ⚠` | `flex < −10` | Übermäßiges Überstrecken / Hohlkreuz | Rot |
| `— needs pelvis` | Becken offline | Ohne beide Sensoren nicht berechenbar | Grau |

### Level- & Sensor-Gating

Zeilen erscheinen nur, wenn der Thorax-Sensor online ist (`tOk`) **und** das Level es erlaubt: `Poise` ab **INT**; `Level`, `Top-Line Quiet`, `Torso–Becken-Stack` ab **ADV** — dieselbe `.pelvis-int` / `.pelvis-adv`-CSS-Gatung wie bei den Becken-Zeilen.

### §7.5 Designhinweis — warum hier kein Yaw / CBM

Die Torso-Becken-Rotationsseparation (Counter-Body-Movement) wurde geprüft und bewusst **ausgeschlossen**: Ein absoluter Separationswinkel erfordert Yaw-Integration, die auf einem magnetometerfreien 6-Achsen-IMU in 20–40 s um ≈ 5–20° driftet. Eine ratenbasierte Dissoziations-Metrik (Δω-Peak, proximal-distaler Phasenvorlauf) ist der prinzipiell valide Weg und kann später ergänzt werden; sie ist nicht Teil dieses Poise-Releases.


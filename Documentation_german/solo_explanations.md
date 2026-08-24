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
* **Zentrale Master-Einheit:** Aggregiert ESP-NOW-Streams und liefert JSON-Datenpakete (`lG`, `lA`, `lAy`, `lGr`, `rG`, `rA`, `rAy`, `rGr`; optional `pA`, `pAy`, `pAx`, `pYaw`, `pG`, `pOk`) über den `/data`-Endpunkt an den Browser.
* **Web-Dashboard (`/solo`):** Clientseitiges JavaScript führt Zustandsmaschinen-Filterung, Richtungszuordnung, Neigungsintegration, Standphasen-Berechnungen und Web-Audio-API-Feedback aus.

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

   **T-1-Schnappschuss für Schrittklassifikation:** Zum Zeitpunkt des Aufpralls wird der Winkel vom *vorherigen Frame* (T-1) verwendet — nicht der Momentanwert. Der aZ > 1,00 g-Auslöser feuert nach teilweiser Gewichtsbelastung, wenn das Abrollen bereits begonnen hat; der T-1-Frame erfasst die Fußausrichtung vor dem Kontakt, bevor Verzerrungen auftreten.

2. **Nullpunkt-Tarierung ($\theta_{\text{calibrated}}$):**
   Zur Anpassung individueller Schuhabsatz-Neigungen erfasst der `📐 ZERO`-Button statische Montageversätze ($\text{leftMountOffset}$, $\text{rightMountOffset}$):
   $$\theta = \theta_{\text{raw}} - \text{mountOffset}$$

3. **Richtungsanzeige & Landungsqualitäts-Badge:**

   **Validierte Einschränkung:** Empirische Tests (n=15 Rückwärtsschritte im Flat-Walk, inklusive Beckensensor) bestätigten, dass WCS-Rückwärtsschritte im Flat-Walk konsistent bei θ = +2° bis +9° landen — identisch mit der mehrdeutigen Zone. Weder der dθ-Neigungstrend noch die sagittale Beckenbeschleunigung (durchschnittlicher Richtungsunterschied < 0,03 g über alle Achsen) können in diesem Bereich zuverlässig zwischen Rückwärts und Vorwärts unterscheiden. Das Richtungs-Badge wird daher nur angezeigt, wenn θ eindeutige physikalische Evidenz liefert:

   | θ bei T-1 | Richtungs-Badge |
   | :---: | :--- |
   | θ ≥ +8° | ➡️ FORWARD (blau) — zuverlässiger Fersenerstkontakt |
   | θ < −8° | ⬅️ BACK (lila) — zuverlässiger Zehenerstkontakt |
   | −8° ≤ θ < +8° | — (grau) — mehrdeutig; Richtung nicht angezeigt |

   **Interne Richtungsklassifikation (Anchor-Settle-Auslöser):** Unabhängig von der Badge-Anzeige verwendet die interne `activeDir`-Logik eine engere mehrdeutige Zone: Nur `0° < θ < +10°` erfordert eine Neigungstrend-Prüfung. Für `θ ≤ 0°` (jede Plantarflexion / Zehenerstkontakt) wird `activeDir` direkt auf BACKWARD gesetzt, ohne Trendanalyse. Dies stellt sicher, dass Rückwärtsschritte, die bei θ = −2° bis −5° landen, das Anchor-Settle-Auswertungsfenster korrekt auslösen, obwohl das Richtungs-Badge weiterhin „—" anzeigt (da |θ| < 8°).

   **Landungsqualitäts-Badge — richtungsunabhängig, basierend auf θ-Zone + Jerk:**

   Das Strike-Badge bewertet *wie* der Fuß gelandet ist, unabhängig von der Richtung. Dies ist für Vorwärts- und Rückwärtsschritte gleichermaßen nützlich: `SOFT ✓` bei θ ≈ 0° zeigt einen kontrollierten Rückwärtsschritt an; `HARD IMPACT ⚠` bei θ ≈ 0° bedeutet, dass der Tänzer auf den Fuß gefallen ist.

   Jerk-Schwellenwerte (gleiche Skalierung wie die angezeigte Aufprall-Jerk-Anzeige ÷ 4):
   - **HART:** J > 22 g/s (intern > 88)
   - **MODERAT:** 20 g/s < J ≤ 22 g/s (intern 80–88)
   - **WEICH:** J ≤ 20 g/s (intern ≤ 80)

   | θ-Zone | Jerk | Badge | Bedeutung |
   | :---: | :---: | :--- | :--- |
   | ≥ +8° (Ferse) | ≤ 22 g/s | `HEEL STRIKE ✓` (Grün) | Saubere Fersenlandung — korrekte Vorwärtstechnik |
   | ≥ +8° (Ferse) | > 22 g/s | `HEEL SLAM ⚠` (Rot) | Fersenkontakt, aber zu abrupt — mit Knie/Knöchel abfedern |
   | < −8° (Zehe) | ≤ 22 g/s | `TOE-FIRST ✓` (Grün) | Kontrollierter Zehenerstkontakt — korrekt für tiefe Rückwärtsschritte oder Ball-Steps |
   | < −8° (Zehe) | > 22 g/s | `TOE JAM ⚠` (Rot) | Zehenkontakt zu hart |
   | −8° bis +7° (mehrdeutig) | ≤ 20 g/s | `SOFT ✓` (Grün) | Kontrollierte Landung — gute Qualität unabhängig von der Richtung |
   | −8° bis +7° (mehrdeutig) | 20–22 g/s | `MODERATE` (Gelb) | Akzeptabel; Aufprall reduzieren |
   | −8° bis +7° (mehrdeutig) | > 22 g/s | `HARD IMPACT ⚠` (Rot) | Auf den Fuß gefallen — löst 1200-Hz-Klick aus |

   * **BRUSH+HEEL-Neuklassifikation (200-ms-Fenster):** Wenn eine Landung in der mehrdeutigen Zone innerhalb von 200 ms von einem zweiten aZ > 1,05 g-Peak mit accelAngle > 8° am gleichen Fuß gefolgt wird, wird das Badge zu `BRUSH+HEEL` (grün) aufgewertet und das Richtungs-Badge zeigt ➡️ FORWARD.

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

* **Weiche Dämpfung ($1\text{ bis }15\text{ g/s}$):** Hervorragende Gelenkabsorption (`SOFT`).
* **Moderater Aufprall ($15\text{ bis }30\text{ g/s}$):** Akzeptabler Schrittaufprall.
* **Hartes Stampfen ($> 30\text{ g/s}$ oder $J_{\text{native}} > 120$):** Übermäßiger Schock auf die Gelenke; löst einen niederfrequenten 500-Hz-Aufprallklick aus.

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
* **Elastische Ausdehnung & Timing:** Ein gesundes Überlappungsverhältnis ($18\%\text{ bis }38\%$) erzeugt die charakteristische „elastische" Dehnung und den reibungslosen Impulsübergang im WCS. Zu wenig Überlappung zeigt Hetzen oder Springen an, zu viel führt zu schwerfälligen Übergängen.
* **Hinweis zur Fachliteratur:** Die klassische Ganganalyse (Perry & Burnfield, 2010 — zitiert in §2A; Winter, D.A., 1990: *Biomechanics and Motor Control of Human Gait*, University of Waterloo Press) gibt die Standphase mit ~60 % und die Schwungphase mit ~40 % des Gangzyklus bei komfortabler Gehgeschwindigkeit an. Dies ist eine andere Messung — sie beschreibt, wie lange *ein* Fuß während eines Gangzyklus auf dem Boden bleibt. Die hiesige Metrik misst das *gleichzeitige bilaterale Kontaktverhältnis* innerhalb eines Schrittintervalls, was ein Subset der Einzel-Fuß-Standphase ist. Freier Überblick über Gangphasendefinitionen: [Wikipedia — Ganganalyse](https://de.wikipedia.org/wiki/Ganganalyse).

| Verhältnisbereich (%) | Badge-Bewertung | Biomechanische Bedeutung |
| :---: | :---: | :--- |
| **18% bis 38%** | `OPTIMAL ROLL` | Ideale geerdete Abrollphase für Läufe und Ausdehnung. |
| **< 18%** | `HECTIC` | Gehetzter Gewichtstransfer; fehlende Abrollartikulierung. |
| **> 38%** | `SLUGGISH` | Übermäßiger Bodenkontakt; schwerfälliger Tempoübergang. |

---

### F. Abroll-Symmetrie-Index (ASI) & Glätteindex

1. **Asymmetrie-Index (ASI):**
   Vergleicht die integrierte Winkelarbeit über linke und rechte Fuß-Abrollzyklen während die Füße aktiv in Bewegung sind ($|\omega_{\text{pitch}}| > 15^\circ/\text{s}$):
   $$\text{ASI} = \frac{2 \cdot \left|\int|\omega_{\text{left}}|\,dt - \int|\omega_{\text{right}}|\,dt\right|}{\int|\omega_{\text{left}}|\,dt + \int|\omega_{\text{right}}|\,dt} \times 100\%$$
   * **Ziel:** $< 10\%$ (`SYMMETRIC`), $11\text{--}25\%$ (`MINOR ASYM`), $>25\%$ (`ASYMMETRIC`).

2. **Abroll-Glätteindex (Roll-Smoothness Index):**
   Misst den kombinierten Winkelgeschwindigkeits-Jerk beider Füße, geglättet über ein 25-Frame (0,5 s) gleitendes Fenster:
   $$\text{Smoothness} = 100 - \text{Mean}_{25}\!\left(\min\!\left(100,\;\left(\left|\frac{\Delta\omega_L}{\Delta t}\right| + \left|\frac{\Delta\omega_R}{\Delta t}\right|\right) \times 0.018\right)\right)$$
   * **Ziel:** Höhere Werte ($\ge 65$) zeigen `SMOOTH`, kontinuierliche Knöchelartikulierung ohne Mikrostottern an.

---

### G. Gewichtstransfer-Gradient & Sprunggelenk-Stoßdämpfung

Beide Metriken werden aus einem Post-Aufprall-Überwachungsfenster berechnet, das unmittelbar nach jedem Schrittauslöser öffnet.

**Gewichtstransfer-Gradient** (240-ms-Fenster, 12 Samples):

$$\text{loadRise} = \overline{aZ}_{[160-240\text{ ms}]} - \overline{aZ}_{[0-80\text{ ms}]}$$

| loadRise | Badge | Biomechanische Bedeutung |
| :---: | :---: | :--- |
| $> 0.12\,g$ | `SMOOTH LOAD` (Grün) | Progressiver Gewichtstransfer — Körperschwerpunkt bewegt sich schrittweise über den Fuß |
| $-0.08\text{ bis }+0.12\,g$ | `INSTANT LOAD` (Gelb) | Gewicht sofort beim Aufprall übertragen — weniger Gelenkschutz |
| $< -0.08\,g$ | `EARLY UNLOAD` (Gelb) | Gewicht verlagert sich bereits vor Stabilisierung zum nächsten Fuß |

**Sprunggelenk-Stoßdämpfung + Abrollumkehr** (200-ms-Fenster, 10 Samples von `gRoll`):

> **Messtechnischer Hinweis:** `gRoll` misst die Rotation des *Schuhsegments* um die Roll-Achse des Sensors, nicht direkt den Subtalargelenk-Eversionswinkel. `rollIntegral` ist ein Fußrotations-Proxy für Pronations-Stoßdämpfung; die Umkehrprüfung ist ein Proxy für den Pronation→Supination-Zyklus, der den Windlass-Mechanismus vorspannt. Beide sind als Trainingsindikatoren validiert, keine anatomischen Gelenkmessungen.

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

**Berechnung:** Nach jedem Schrittauslöser wird ein 500-ms-Überwachungsfenster gestartet. Das Fenster gilt als abgeschlossen, wenn $|aZ - 1.0| < 0.08g$ UND $|\omega_{\text{pitch}}| < 15°/\text{s}$ für 2 aufeinanderfolgende Samples erfüllt sind.

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
| **Fersenzone — kontrolliert** | $\theta \ge +8°$, Jerk $\le 30$ g/s | `HEEL STRIKE ✓` (Grün) | Kein |
| **Fersenzone — abrupt** | $\theta \ge +8°$, Jerk $> 30$ g/s | `HEEL SLAM ⚠` (Rot) | 1200-Hz-Klick |
| **Zehenzone — kontrolliert** | $\theta < -8°$, Jerk $\le 30$ g/s | `TOE-FIRST ✓` (Grün) | Kein |
| **Zehenzone — abrupt** | $\theta < -8°$, Jerk $> 30$ g/s | `TOE JAM ⚠` (Rot) | 1200-Hz-Klick |
| **Mehrdeutig — weich** | $-8° \le \theta < +8°$, Jerk $\le 20$ g/s | `SOFT ✓` (Grün) | Kein |
| **Mehrdeutig — moderat** | $-8° \le \theta < +8°$, Jerk $20\text{–}22$ g/s | `MODERATE` (Gelb) | Kein |
| **Mehrdeutig — hart** | $-8° \le \theta < +8°$, Jerk $> 30$ g/s | `HARD IMPACT ⚠` (Rot) | 1200-Hz-Klick |
| **BRUSH+HEEL-Neuklassifikation** | mehrdeutig → zweites aZ $> 1{,}05\,g$ + accelAngle $> 8°$ innerhalb 200 ms | `BRUSH+HEEL` (Grün) → ➡️ FORWARD | Kein |
| **Standbein-Abstoß (vorwärts, optimal)** | BACKWARD letzter Schritt + $-\omega_{\text{pitch}} \ge 200^\circ/\text{s}$ UND $aY > 0.15g$ | `🚀 POWER PUSH` (Grün) — hält 400 ms | Kein |
| **Standbein-Abstoß (rückwärts/Anker, optimal)** | FORWARD letzter Schritt + $-\omega_{\text{pitch}} \ge 160^\circ/\text{s}$ UND $aY > 0.15g$ | `🚀 POWER PUSH` (Grün) — hält 400 ms | Kein |
| **Standbein-Abstoß (schwach)** | Beide Richtungen, $120\text{–}159/199^\circ/\text{s}$ UND $aY > 0.15g$ | `↗ PUSH` (Gelb) — hält 400 ms | Kein |
| **Aufprall-Jerk ($J_{\text{impact}}$)** | $> 30\text{ g/s}$ | Karten-Rand blinkt | 500-Hz-Aufprallklick (80 ms) |
| **Doppelstandphase — Optimal** | 18% bis 38% | `OPTIMAL ROLL` (Grün) | Kein |
| **Doppelstandphase — Hetzen** | $< 18\%$ | `HECTIC` (Gelb) | Kein |
| **Doppelstandphase — Träge** | $> 38\%$ | `SLUGGISH` (Gelb) | Kein |
| **Gewichtstransfer — Progressiv** | loadRise $> 0.12\,g$ | `SMOOTH LOAD` (Grün) | Kein |
| **Gewichtstransfer — Sofort** | $-0.08 \le$ loadRise $\le 0.12$ | `INSTANT LOAD` (Gelb) | Kein |
| **Gewichtstransfer — Frühzeitig** | loadRise $< -0.08\,g$ | `EARLY UNLOAD` (Gelb) | Kein |
| **Verzögerungsrampe — VW verzögert** | ratio 0,12–0,38 (VW) | `DELAYED ✓` (Grün) | Kein |
| **Verzögerungsrampe — VW schnell** | ratio $< 0.12$ (VW) | `QUICK` (Gelb) | Kein |
| **Verzögerungsrampe — VW spät** | ratio $> 0.38$ (VW) | `LATE` (Gelb) | Nur ADV |
| **Verzögerungsrampe — RW verzögert** | ratio 0,18–0,50 (RW) | `DELAYED ✓` (Grün) | Kein |
| **Verzögerungsrampe — RW schnell** | ratio $< 0.18$ (RW) | `QUICK` (Gelb) | Kein |
| **Verzögerungsrampe — RW spät** | ratio $> 0.50$ (RW) | `LATE` (Gelb) | Nur ADV |
| **Rigid Lever** | Pronation $> 8°/\text{s}$ UND Vorzeichenumkehr in 200 ms | `RIGID LEVER` (Grün) | Kein |
| **Sprunggelenk-Stoßdämpfung** | rollIntegral $> 4°$ (keine Umkehr) | `ANKLE FLEX` (Grün) | Kein |
| **Sprunggelenk-Steifigkeit** | rollIntegral $< 1°$ | `STIFF ANKLE` (Gelb) | Kein |
| **Hitch & Go — erkannt** | $|aZ| < 0{,}35\,g$ für 50–380 ms innerhalb 700 ms nach letztem Schritt | `✓ HITCH (L/R)` (Grün) | Kein |
| **Ball→Ferse — optimal** | earlyMean $< -2°$, rise $> 3°$ | `BALL→HEEL ✓` (Grün) | Kein |
| **Ball→Ferse — partiell** | earlyMean $< 0°$, rise $> 1°$ | `PARTIAL ROLL` (Gelb) | Kein |
| **Ball→Ferse — Ferse zuerst** | earlyMean $\ge 0°$ | `HEEL-FIRST` (Gelb) | Kein |
| **Ball→Ferse — nur Ballen** | earlyMean $< 0°$, rise $\le 1°$ | `BALL ONLY ⚠` (Rot) | Kein |
| **Ferse→Ball — optimal** | earlyMean $> 2°$, drop $> 3°$ | `HEEL→BALL ✓` (Grün) | Kein |
| **Ferse→Ball — partiell** | earlyMean $> 0°$, drop $> 1°$ | `PARTIAL ROLL` (Gelb) | Kein |
| **Ferse→Ball — blockiert** | earlyMean $> 2°$, drop $\le 1°$ | `HEEL STUCK ⚠` (Rot) | Kein |
| **Zehe→Ferse — optimal (Ballensch.)** | earlyMean $< -2°$, rise $> 3°$ | `TOE→HEEL ✓` (Grün) | Kein |
| **Zehe→Ferse — partiell (Ballensch.)** | earlyMean $< 0°$, rise $> 1°$ | `PARTIAL ROLL` (Gelb) | Kein |
| **Ferse→Ball — Flachfuß** | earlyMean $\le 0°$, rise $\le 1°$ | `FLAT-FOOT` (Gelb) | Kein |
| **Pro-Fuß-Sperrzeitfenster** | $180\text{–}320\text{ ms}$ (kadenzadaptiv) + Alternierungswächter | Unterdrückt Doppelauslösung | Kein |

---

### I. Hitch & Go Erkennung

Ein *Hitch* ist ein kurzes, bewusstes Anheben des gerade aufgesetzten Fußes — ein Jazz/Blues-Akzent zur Phrasierungssynchronisation. Der Sensor erkennt ihn als kurzzeitigen Bodenkontaktverlust am aktiven Fuß innerhalb von 80–700 ms nach dem letzten Schritt auf diesem Fuß.

**Erkennungslogik:**

| Zustand | Bedingung | Übergang |
| :--- | :--- | :--- |
| `ground` → `lifted` | $|aZ| < 0{,}35\,g$ UND $80\,\text{ms} < t_{\text{seit Schritt}} < 700\,\text{ms}$ | Liftzeit-Start |
| `lifted` → `ground` (gültig) | $|aZ| \ge 0{,}55\,g$ UND $50\,\text{ms} \le t_{\text{lift}} \le 380\,\text{ms}$ | `✓ HITCH (L/R)` (Grün) |
| `lifted` → `ground` (zu lang) | $t_{\text{lift}} > 380\,\text{ms}$ | Reset ohne Badge — Gewichtsverlagerung, kein Hitch |
| Sensor offline | — | Zustand auf `ground` zurückgesetzt |

Der 0,35g-Schwellenwert liegt klar unterhalb des normalen belasteten Fuß-aZ von ~1,0g und oberhalb des Rauschpegels. Der 0,55g-Rückkehrschwellenwert sorgt für Hysterese beim Wiederaufsetzen (Hinweis: der DS-Bodenkontakt-Eintrittsschwellenwert beträgt 0,65g; der 0,55g-Wert ist spezifisch für die Hitch-Wiederaufsetz-Erkennung).

**Hinweis:** Hitch-Erkennung ist nicht levelabhängig — das Badge erscheint in der Letzter-Schritt-Kachel bei allen Trainingslevels.

---

### J. Ball-to-Heel Anker-Progression

Bei einem gut ausgeführten Rückwärts-Anker setzt der Fuß zunächst auf dem Ballen auf (θ negativ — Plantarflexion) und senkt sich dann zur Ferse, während das Körpergewicht einsinkt. Der Sensor quantifiziert diese Progression durch Tracking des Fußneigungswinkels θ während eines tempoadaptiven Fensters nach jedem Rückwärtsschritt.

**Fenster:** `anchorWindowMs = clamp(stepDurationMs, 280 ms, 500 ms)` — dasselbe Fenster wie beim Becken-Anchor-Settle; unabhängig davon, ob der Beckensensor angeschlossen ist.

**Berechnung:**

$$\theta_{\text{früh}} = \overline{\theta}_{[0,\,\lfloor n/2 \rfloor]} \qquad \theta_{\text{spät}} = \overline{\theta}_{[\lfloor n/2 \rfloor,\,n]}$$

$$\text{rise} = \theta_{\text{spät}} - \theta_{\text{früh}}$$

| Bedingung | Badge | Biomechanische Bedeutung |
| :---: | :---: | :--- |
| $\theta_{\text{früh}} < -2°$ UND rise $> 3°$ | `BALL→HEEL ✓` (Grün) | Klassische WCS-Landung: Ballen-zuerst, Ferse senkt sich — kontrollierte exzentrische Belastung |
| $\theta_{\text{früh}} < 0°$ UND rise $> 1°$ | `PARTIAL ROLL` (Gelb) | Etwas Ballen-zu-Ferse-Bewegung erkannt, aber geringer Anstieg |
| $\theta_{\text{früh}} \ge 0°$ | `HEEL-FIRST` (Gelb) | Fuß kam flach oder Ferse-zuerst an — kein initialer Ballenkontakt |
| Sonst | `BALL ONLY ⚠` (Rot) | θ blieb durchgehend negativ — Fuß blieb auf dem Ballen; typisch bei Hetzen oder unvollständigem Gewichtstransfer |

**Mindestanzahl Samples:** Die Auswertung erfordert mindestens 6 Samples im Fenster (~120 ms). Wenn ein neuer Schritt auslöst, bevor das Fenster schließt, wird `anchorThetaActive` zurückgesetzt und kein Badge ausgegeben.

---

### K. Ferse-zu-Ballen / Zehe-zu-Ferse Vorwärts-Progression

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

## 4. Signalfilterung & Sperrzeitkonzept (Lockout)

Um falsche sekundäre Schrittauslöser durch Mikrotipps, Fußentlastungen oder Bodenschwingungen zu verhindern, führt die DSP-Pipeline (Digital Signal Processing) ein **Zweistufiges Filterungs- & Sperrzeitkonzept** aus:

1. **Transientes Signal-Kandidaten-Sensing:**
   Jeder Fuß qualifiziert sich unabhängig als Aufprallkandidat über ODER-Logik:

   $$\text{signal}_{\text{foot}} = \bigl(\lvert aZ\rvert > 1.00\,g\bigr) \quad\mathbf{ODER}\quad \bigl(\lvert\omega_{\text{pitch}}\rvert > 80\,\text{deg/s} \quad\mathbf{UND}\quad \text{preJerk} > 8\bigr)$$

   Das `preJerk`-Gate (`|aZ_t - aZ_{t-1}| / Δt > 8`) auf dem Gyro-Pfad unterdrückt Abhebebewegungs-Artefakte. Wenn beide Füße im gleichen Frame signalisieren, wird der dominante Fuß nach maximaler Bodenreaktionskraft ausgewählt: $\text{detectedFoot} = \arg\max(|aZ_L|, |aZ_R|)$.

2. **Pro-Fuß Kadenz-Adaptives Sperrzeitfenster & Alternierungswächter:**
   * **Sperrzeitkonzept:** Das System verwaltet unabhängige Letztschritt-Zeitstempel für jedes Bein (`lastStepTimeLeft` und `lastStepTimeRight`). Wenn ein Schrittkandidat erkannt wird, prüft die Zustandsmaschine, ob die seit dem letzten Schritt *an diesem spezifischen Bein* verstrichene Zeit kleiner als das dynamische Sperrzeitfenster ist.
   * **Kadenz-Adaptives Fenster:** $t_{\text{lockout}} = \text{clamp}(t_{\text{step}} \times 0.55,\ 180\text{ ms},\ 320\text{ ms})$. Bei 120 BPM → 275 ms; bei 160 BPM → 206 ms; bei 200 BPM → 180 ms (Untergrenze).
   * **Alternierungswächter:** Schritte müssen alternieren (`Links → Rechts → Links`). Gleicher Fuß zweimal ohne Gegenfuß-Kontakt dazwischen wird als Artefakt verworfen.
   * **Globaler Cross-Fuß-Lockout (130 ms):** Jeder Schrittauslöser — unabhängig vom Fuß — wird verworfen, wenn er innerhalb von 130 ms nach dem letzten bestätigten Schritt eintrifft. Dieser übergreifende Lockout verhindert False-Trigger des ruhenden Fußes (~1,00 g Oszillation) kurz nach einem echten Schritt: Das per-Fuß-Sperrzeitfenster des Gegenfußes ist veraltet und würde ihn nicht blockieren. `lastStepTimestamp` wird bei jedem bestätigten Schritt aktualisiert und gilt für beide Füße.

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

---

## 6. Becken-Metriken (Solo-Dashboard)

Wenn der Beckensensor (ID 4) verbunden ist, zeigt das Solo-Dashboard sechs zusätzliche Badge-Kacheln auf Basis der Beckenkinematik. Der Sensor sitzt auf einem Gürtel am Kreuzbein und überträgt 3-Achsen-Beschleunigung (`pAx`, `pAy`, `pAz`) sowie Gier-Gyro (`pYaw`) mit 200 Hz.

### gYaw-Kurve im Live-Abroll-Dynamik-Graphen

Eine **gestrichelte gelbe Linie** im Live-Abroll-Dynamik-Graphen zeigt die gemessene Hüftgier-Rate (`pYaw` des Beckensensors), auf den gleichen Anzeigebereich wie die Fußneigungskurven skaliert. Die Kurve ist sichtbar, sobald der Beckensensor verbunden ist, und gibt in Echtzeit Aufschluss darüber, wie das Hüftrotations-Timing mit den Fußabroll-Ereignissen zusammenfällt.

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
| SLIGHT BOUNCE | `aZVar < 0,020` | Gelb |
| BOUNCY | `aZVar ≥ 0,020` | Rot |

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

Bewertet die Qualität des Abbremsens und Einschwingens des Beckens nach jedem Rückwärtsschritt — der entscheidende Moment, in dem WCS-Dehnung in geerdet kontrollierten Gewichtstransfer umgewandelt wird.

**Auslöser:** Jeder bestätigte BACKWARD-Schritt. Fensterdauer skaliert mit dem Tempo:

$$\text{anchorWindowMs} = \min(500,\; \max(280,\; \text{stepDurationMs}))$$

Im Fenster gesammelte Signale: sagittale Beckenbeschleunigung `aYP` und Hüftgier-Rate `gYawP`.

**Score-Zusammensetzung (skaliert 0–100):**

$$\text{score} = \text{decelScore} \times 0{,}35 + \text{yawDampScore} \times 0{,}35 + \text{stabilScore} \times 0{,}30$$

| Komponente | Formel | Bedeutung |
| :--- | :--- | :--- |
| **decelScore** | `(earlyAYMag − lateAYMag + 0,05) / 0,25`, begrenzt 0–1 | Sagittales Abbremsen: Becken bremst Vorwärtsmomentum |
| **yawDampScore** | `(earlyYawMean − lateYawMean) / 25`, begrenzt 0–1 | Gier-Dämpfung: Hüftrotation stoppt nach Landung |
| **stabilScore** | `max(0, 1 − lateYawVariance / 400)` | Spät-Phasen-Stabilität: geringe Gier-Varianz in zweiter Fensterhälfte |

| Zustand | Bedingung | Farbe |
| :--- | :--- | :--- |
| ANCHORED | score ≥ 60 | Grün |
| SETTLING | score ≥ 30 | Gelb |
| UNSTABLE | score < 30 | Rot |

### Hip Settle (Hüft-Einschwingen)

Wird am Ende des Anchor-Settle-Fensters ausgewertet. Verwendet laterale Beckenbeschleunigung (`pAx`) aus der ersten Fensterhälfte, um die charakteristische seitliche Gewichtsverlagerung eines gelungenen Ankers zu erkennen.

- `earlyLatPeak` = maximaler Betrag von `pAx` in der ersten Fensterhälfte
- `lateLatVar` = Varianz von `pAx` in der zweiten Fensterhälfte

| Zustand | Bedingung | Farbe |
| :--- | :--- | :--- |
| OVERSWING ⚠ | `earlyLatPeak > 0,30 g` | Gelb |
| HIP SETTLE ✓ | `earlyLatPeak > 0,10 g` UND `lateLatVar < 0,015` | Grün |
| SLIGHT SETTLE | `earlyLatPeak > 0,05 g` | Gelb |
| NO HIP SETTLE | `earlyLatPeak ≤ 0,05 g` | Rot |


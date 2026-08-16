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
                 +--------------------------------+
                 | Zentrale Master-Einheit        |
                 | M5Stick S3 Web-Server          |
                 +---------------+----------------+
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
* **Zentrale Master-Einheit:** Aggregiert ESP-NOW-Streams und liefert JSON-Datenpakete (`lG`, `lA`, `lAy`, `lGr`, `rG`, `rA`, `rAy`, `rGr`) über den `/data`-Endpunkt an den Browser.
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

   **T-1-Schnappschuss für Schrittklassifikation:** Zum Zeitpunkt des Aufpralls wird der Winkel vom *vorherigen Frame* (T-1) verwendet — nicht der Momentanwert. Der aZ > 1,08 g-Auslöser feuert nach teilweiser Gewichtsbelastung, wenn das Abrollen bereits begonnen hat; der T-1-Frame erfasst die Fußausrichtung vor dem Kontakt, bevor Verzerrungen auftreten.

2. **Nullpunkt-Tarierung ($\theta_{\text{calibrated}}$):**
   Zur Anpassung individueller Schuhabsatz-Neigungen erfasst der `📐 ZERO`-Button statische Montageversätze ($\text{leftMountOffset}$, $\text{rightMountOffset}$):
   $$\theta = \theta_{\text{raw}} - \text{mountOffset}$$

3. **Richtungsklassifikation & Aufprallwinkel-Bewertung:**

   Die Richtung wird in zwei Stufen bestimmt. Stufe 1 nutzt den kalibrierten T-1-Neigungswinkel zur Auflösung der eindeutigen Extreme; Stufe 2 wendet einen 160-ms-Neigungswinkeltrend (dθ) zur Auflösung der mehrdeutigen Zone an.

   **Stufe 1 — θ-Zonenklassifikation:**

   $$\text{activeDirection} = \begin{cases} \text{BACKWARD} & \theta \le -5° \\ \text{AMBIGUOUS} & -5° < \theta < 10° \\ \text{FORWARD} & \theta \ge 10° \end{cases}$$

   **Warum asymmetrisch:** Negatives θ zeigt zuverlässig Zehenerstkontakt an (eindeutig rückwärts); positives θ unter +10° ist tatsächlich mehrdeutig — ein flacher Vorwärtsschritt und ein Rückwärtsschritt mit frühem Fersenabsatz landen beide im gleichen Winkelbereich (+5° bis +9°).

   **Stufe 2 — dθ-Neigungstrend (nur mehrdeutige Zone):**

   Wenn Stufe 1 AMBIGUOUS ergibt ($-5° < \theta < 10°$), berechnet das System einen 160-ms-Neigungswinkeltrend aus dem Vorkontakt-Ringpuffer (10 kalibrierte θ-Samples bei 50 Hz):
   $$d\theta = \theta_{\text{calibrated}}[T{-1}] - \theta_{\text{calibrated}}[T{-8}]$$

   $$\text{activeDirection} = \begin{cases} \text{FORWARD} & d\theta > +2° \\ \text{BACKWARD} & d\theta < -2° \\ \text{AMBIGUOUS} & |d\theta| \le 2° \end{cases}$$

   Vorwärtsschwung = Dorsalflexion → θ steigt → positives dθ. Rückwärtsschwung = Plantarflexion → θ fällt → negatives dθ. Validierter Schwellenwert: Vorwärts dθ +2,6° bis +7,9°, Rückwärts dθ −2,3° bis −3,1°, neutral/Anker −0,4° bis +1,9°.

   * **Vorwärtsschritt (FORWARD via $[\theta]$ oder $[d\theta]$):**
     * $\theta > 35° \longrightarrow$ `HEEL SPIKE` (extreme Dorsalflexion)
     * $10° \le \theta \le 35° \longrightarrow$ `OPTIMAL HEEL` (saubere Fersenartikulierung)
     * BRUSH+HEEL-Neuklassifikationsfenster (200 ms) kann vorheriges `FLAT-FOOT!` zu `BRUSH+HEEL` aufwerten, wenn ein zweiter aZ > 1,05 g-Peak mit accelAngle > 8° am gleichen Fuß erkannt wird.
   * **Rückwärtsschritt (BACKWARD via $[\theta]$ oder $[d\theta]$):**
     * Via $[\theta]$: $-5° > \theta \ge -20° \longrightarrow$ `OPTIMAL TOE`; $\theta < -20° \longrightarrow$ `HEEL SPIKE`
     * Via $[d\theta]$: `OPTIMAL TOE` für jedes θ, das die Unterbedingungen unten nicht erfüllt
     * $-2° \le \theta \le +5°$ via $[d\theta]$ UND $aZ > 0.85g$ → `ANCHOR SETTLE` (voller Gewichtseinsatz, stabilisierter Kontakt)
     * $+5° < \theta \le +9°$ via $[d\theta]$ → `HEEL DROP` (Ferse kontaktiert früh beim Rückwärtsschritt)
   * **Mehrdeutig ($|d\theta| \le 2°$ oder Ringpuffer < 9 Samples):**
     * ↔️ FLAT + `FLAT-FOOT!` (Rot) — Richtung durch θ oder dθ nicht auflösbar; öffnet auch das 200-ms-Brush+Heel-Neuklassifikationsfenster.
     * Erfasst: flache Vorwärtsschritte, Rückwärtsschritte ohne ausreichenden Neigungstrend und tatsächlich flache Landungen.

---

### C. Terminale Standphase & Power Push-Antrieb

WCS-Antrieb erfordert ein aktives Zehenabdrücken (*Windlass-Mechanismus*) vom Standbein am Ende der Standphase. Die optimale Plantarflexions-Winkelgeschwindigkeit hängt von der Bewegungsrichtung ab — Vorwärtsantrieb erfordert mehr Kraft als die subtilere Umverteilung bei einem Anker oder Rückwärtslauf.

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

West Coast Swing betont einen kontinuierlichen, gegrundeten „Abroll"-Gewichtstransfer anstelle von abruptem Hüpfen oder verfrühtem Abheben vom Boden. Bodenkontakt wird registriert, wenn die vertikale Beschleunigung den statischen Schwerkraft-Basiswert überschreitet ($|aZ| > 0.55g$).

> **Signalhinweis:** $|aZ| > 0.55g$ ist eine Sensor-Heuristik für bilateralen Bodenkontakt — keine direkte Kraftmessung. Dynamische Fußrotationen können $aZ$ unabhängig vom tatsächlichen Bodenkontakt verschieben. Die folgenden Schwellenwerte wurden empirisch kalibriert.

$$\text{Standphasenverhältnis} = \left( \frac{\Delta t_{\text{double-stance}}}{t_{\text{step}}} \right) \times 100\%$$

#### Warum Überlappung in der WCS-Mechanik wichtig ist:
* **Gegrundetes Abrollen:** Im West Coast Swing ist der Gewichtstransfer graduell. Während ein Fuß den Boden verlässt, nimmt der andere das Gewicht auf und erzeugt eine natürliche bilaterale Überlappungsphase ($|aZ| > 0.55g$).
* **Elastische Ausdehnung & Timing:** Ein gesundes Überlappungsverhältnis ($18\%\text{ bis }38\%$) erzeugt die charakteristische „elastische" Dehnung und den reibungslosen Impulsübergang im WCS. Zu wenig Überlappung zeigt Hetzen oder Springen an, zu viel führt zu schwerfälligen Übergängen.
* **Hinweis zur wissenschaftlichen Literatur:** Biomechanische Quellen nennen eine Einzel-Fuß-Standphase von ~60 % des Gangzyklus. Dies ist eine andere Messung — sie beschreibt, wie lange *ein* Fuß auf dem Boden bleibt. Die Metrik hier misst das *gleichzeitige bilaterale Kontaktverhältnis* innerhalb eines Schrittzyklusses, das sich grundlegend davon unterscheidet.

| Verhältnisbereich (%) | Badge-Bewertung | Biomechanische Bedeutung |
| :---: | :---: | :--- |
| **18% bis 38%** | `OPTIMAL ROLL` | Ideale gegrundete Abrollphase für Läufe und Ausdehnung. |
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
$$\text{loadRise} = \overline{aZ}_{[160\text{–}240\,\text{ms}]} - \overline{aZ}_{[0\text{–}80\,\text{ms}]}$$

| loadRise | Badge | Biomechanische Bedeutung |
| :---: | :---: | :--- |
| $> 0.12\,g$ | `SMOOTH LOAD` (Grün) | Progressiver Gewichtstransfer — Körperschwerpunkt bewegt sich schrittweise über den Fuß |
| $-0.08\text{ bis }+0.12\,g$ | `INSTANT LOAD` (Gelb) | Gewicht sofort beim Aufprall übertragen — weniger Gelenkschutz |
| $< -0.08\,g$ | `EARLY UNLOAD` (Gelb) | Gewicht verlagert sich bereits vor Stabilisierung zum nächsten Fuß |

**Sprunggelenk-Stoßdämpfung + Abrollumkehr** (200-ms-Fenster, 10 Samples von `gRoll`):

> **Messtechnischer Hinweis:** `gRoll` misst die Rotation des *Schuhsegments* um die Roll-Achse des Sensors, nicht direkt den Subtalargelenk-Eversionswinkel. `rollIntegral` ist ein Fußrotations-Proxy für Pronations-Stoßdämpfung; die Umkehrprüfung ist ein Proxy für den Pronation→Supination-Zyklus, der den Windlass-Mechanismus vorspannt. Beide sind als Trainingsindikatoren validiert, keine anatomischen Gelenkmessungen.

Fußroll-Integral über die ersten 100 ms (Samples 0–4):
$$\text{rollIntegral} = \left|\sum_{i=0}^{4} \omega_{\text{roll},i} \times 0.02\,\text{s}\right| \quad [\text{Grad}]$$

Abrollumkehr-Prüfung — Vorzeichenwechsel zwischen früher (Samples 0–3) und später (Samples 6–9) Phase:
$$\text{rollReversal} = |\overline{\omega}_{[0\text{–}3]}| > 8°/\text{s} \;\;\text{UND}\;\; \overline{\omega}_{[0\text{–}3]} \cdot \overline{\omega}_{[6\text{–}9]} < 0$$

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
| **Vorwärts Ferse — Optimal** | $10^\circ \le \theta \le 35^\circ$ | `OPTIMAL HEEL` (Grün) | Kein |
| **Vorwärts Ferse — Spike** | $\theta > 35^\circ$ | `HEEL SPIKE` (Gelb) | Kein |
| **Vorwärts Brush+Heel** | flach → accelAngle $> 8^\circ$ innerhalb 200 ms | `BRUSH+HEEL` (Grün) — neuklassifiziert von FLAT-FOOT! | Kein |
| **Mehrdeutiger Flachkontakt** | $-5° < \theta < 10°$ und $|d\theta| \le 2°$ (oder Puffer < 9 Samples) | ↔️ FLAT + `FLAT-FOOT!` (Rot) | 1200-Hz-Klick bei Aufprall-Jerk > 40 g/s |
| **Rückwärts Zehe — Optimal** | Via [θ]: $-20° \le \theta < -5°$; oder via [dθ]: θ in $-5°$ bis $+9°$, nicht ANCHOR SETTLE/HEEL DROP | `OPTIMAL TOE` (Grün) | Kein |
| **Rückwärts Zehe — Spike** | $\theta < -20°$ | `HEEL SPIKE` (Gelb) | Kein |
| **Rückwärts — Anchor Settle** | BACKWARD via [dθ] + $-2° \le \theta \le +5°$ + $aZ > 0.85g$ | `ANCHOR SETTLE` (Grün) | Kein |
| **Rückwärts — Heel Drop** | BACKWARD via [dθ] + $+5° < \theta \le +9°$ | `HEEL DROP` (Gelb) | Kein |
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
| **Pro-Fuß-Sperrzeitfenster** | $180\text{–}320\text{ ms}$ (kadenzadaptiv) + Alternierungswächter | Unterdrückt Doppelauslösung | Kein |

---

## 4. Signalfilterung & Sperrzeitkonzept (Lockout)

Um falsche sekundäre Schrittauslöser durch Mikrotipps, Fußentlastungen oder Bodenschwingungen zu verhindern, führt die DSP-Pipeline (Digital Signal Processing) ein **Zweistufiges Filterungs- & Sperrzeitkonzept** aus:

1. **Transientes Signal-Kandidaten-Sensing:**
   Jeder Fuß qualifiziert sich unabhängig als Aufprallkandidat über ODER-Logik:
   $$\text{signal}_{\text{foot}} = \bigl(|aZ| > 1.08\,g\bigr) \;\mathbf{ODER}\; \bigl(|\omega_{\text{pitch}}| > 80\,\text{deg/s} \;\mathbf{UND}\; \text{preJerk} > 8\bigr)$$
   Das `preJerk`-Gate (`|aZ_t - aZ_{t-1}| / Δt > 8`) auf dem Gyro-Pfad unterdrückt Abhebebewegungs-Artefakte. Wenn beide Füße im gleichen Frame signalisieren, wird der dominante Fuß nach maximaler Bodenreaktionskraft ausgewählt: $\text{detectedFoot} = \arg\max(|aZ_L|, |aZ_R|)$.

2. **Pro-Fuß Kadenz-Adaptives Sperrzeitfenster & Alternierungswächter:**
   * **Sperrzeitkonzept:** Das System verwaltet unabhängige Letztschritt-Zeitstempel für jedes Bein (`lastStepTimeLeft` und `lastStepTimeRight`). Wenn ein Schrittkandidat erkannt wird, prüft die Zustandsmaschine, ob die seit dem letzten Schritt *an diesem spezifischen Bein* verstrichene Zeit kleiner als das dynamische Sperrzeitfenster ist.
   * **Kadenz-Adaptives Fenster:** $t_{\text{lockout}} = \text{clamp}(t_{\text{step}} \times 0.55,\ 180\text{ ms},\ 320\text{ ms})$. Bei 120 BPM → 275 ms; bei 160 BPM → 206 ms; bei 200 BPM → 180 ms (Untergrenze).
   * **Alternierungswächter:** Schritte müssen alternieren (`Links → Rechts → Links`). Gleicher Fuß zweimal ohne Gegenfuß-Kontakt dazwischen wird als Artefakt verworfen.

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

## 6. Tänzerguide

> **Dieses Kapitel wurde in eine separate Datei ausgelagert: [`dancer_guide_solo.md`](dancer_guide_solo.md)**
>
> Der Tänzerguide ist eigenständig — Tänzer können ihn direkt öffnen, ohne die technischen Abschnitte dieses Dokuments lesen zu müssen. Er behandelt das Dashboard-Layout, die Level-Auswahl (BEG / INT / ADV), alle Badge-Karten, Level-basierte Übungsempfehlungen und eine Problemreferenz.
> Für die Partner-/Trainer-Ansicht siehe [`dancer_guide_partner.md`](dancer_guide_partner.md).
>
> Der folgende verkürzte Abschnitt dient als Schnellreferenz innerhalb dieses Dokuments.

---

### 6.1 Einrichtung vor dem Start

1. **Smartphone oder Tablet auf ein Stativ montieren** auf Augenhöhe, dem Tänzer zugewandt. Querformat bietet die beste Ansicht.
2. **`📷 CAM` antippen** um das Kamera-Overlay zu aktivieren.
3. **Tanzschuhe anziehen** vor dem nächsten Schritt.
4. **Natürlich in der Tanzhaltung stehen** (Füße schulterbreit, leichte Vorwärtsneigung). **`📐 ZERO` antippen.** Das System kennt jetzt „flacher Fuß auf dem Boden" für die spezifischen Schuhe.
5. **`🔊 Audio` EIN schalten.** Die Audio-Pieptöne reagieren schneller als die Anzeige gelesen werden kann.
6. Mit dem Gehen oder Tanzen beginnen. 10–15 Aufwärmschritte einplanen.

> **Neu tarieren bei Schuh- oder Untergrundwechsel.** Die `📐 ZERO`-Kalibrierung ist schuhanfangsspezifisch.

---

### 6.2 Die Schritt-Badge-Karte lesen

Aktualisiert sich bei jedem erkannten Fußkontakt.

| Anzeigelement | Bedeutung |
| :--- | :--- |
| **➡️ FORWARD** | Gerade gelandeter Vorwärtsschritt |
| **⬅️ BACKWARD** | Gerade gelandeter Rückwärtsschritt |
| **↔️ FLAT** | Fuß zu flach gelandet — Richtung nicht klassifizierbar, als Flachfuß-Warnung behandeln |
| **θ (vorzeichenbehafteter Winkel)** | Fußneigung zum Landezeitpunkt. Positiv = Ferse höher; negativ = Zehe höher. |
| **Badge** | Klassifikation dieser Landung |
| **Jerk (g/s)** | Rate der Aufprallkraft |

#### Vorwärtsschritt-Badges

| Badge | θ | Was getan | Ziel |
| :--- | :--- | :--- | :--- |
| `OPTIMAL HEEL` ✅ | +10° bis +35° | Sauberer Fersenaufsatz | Ziel für alle Vorwärtsläufe |
| `FLAT` ⚠️ | +5° bis +10° | Leichter Fersenvorsprung, fast flach | Fersenartikulierung erhöhen |
| `FLAT-FOOT!` ❌ | < +5° | Flach oder Zehenerstkontakt | Häufige Ursache: zu eilig, angespannte Knöchel |
| `HEEL SPIKE` ⚠️ | > +35° | Extrem steiler Fersenwinkel | Antriebskraft reduzieren oder Knöchel entspannen |

#### Rückwärtsschritt-Badges

| Badge | θ | Was getan | Ziel |
| :--- | :--- | :--- | :--- |
| `OPTIMAL TOE` ✅ | −20° bis +5° | Saubere Zehen-Ballen-Landung | Ziel für alle Rückwärtsläufe und Anker |
| `ANCHOR SETTLE` ✅ | −2° bis +5° + volles Gewicht | Ferse berührt Boden mit vollem Gewichtseinsatz | Ideale Anker-Vollendung |
| `HEEL DROP` ⚠️ | +5° bis +9° | Ferse sinkt vor vollständigem Gewichtstransfer | Knöchel länger dorsiflektiert halten |
| `HEEL LANDING!` ❌ | ≥ +10° | Ferse schlug zuerst auf | Häufigster WCS-Technikfehler |
| `HEEL SPIKE` ⚠️ | < −20° | Übermäßig gespitzter Fuß | Ausdehnung leicht modulieren |

#### Aufprall-Jerk-Balken

- **Kurzer Balken, kein Klick** → weiche, kontrollierte Landung. Ideal.
- **Voller roter Balken + 500-Hz-Klick** → hartes Stampfen. Knie beim Bodenkontakt beugen und mit dem Knöchel absorbieren.

---

### 6.3 Push-Off & Loading-Badges lesen

| Badge | Bedeutung | Verbesserung |
| :--- | :--- | :--- |
| `🚀 POWER PUSH` ✅ | Starker, biomechanisch optimaler Abstoß | Beibehalten |
| `↗ PUSH` ⚠️ | Abstoß erkannt, unter optimaler Kraft | Aktiver durch den Fußballen treiben |
| `— PUSH-OFF` | Kein signifikanter Abstoß | Standbein ist passiv — Knöchel am Ende jedes Laufs strecken |
| `SMOOTH LOAD` ✅ | Progressiver Gewichtstransfer | Gute Gelenkmechanik — beibehalten |
| `INSTANT LOAD` ⚠️ | Gewicht sofort fallen gelassen | COM verlangsamen; „Boden empfangen" statt drauf landen |
| `EARLY UNLOAD` ⚠️ | Gewicht verlagert sich vor Stabilisierung | Aktuellen Gewichtseinsatz abschließen, bevor weitergegangen wird |
| `DELAYED ✓` ✅ | Gewicht 12–38% (VW) oder 18–50% (RW) des Beats nach Kontakt | WCS-typisches Schweben — beibehalten |
| `QUICK` ⚠️ | Gewicht sofort übertragen | Kein musikalischer Atem im Transfer — bewusst pausieren |
| `LATE` ⚠️ | Gewicht kam nie vollständig an (nur ADV) | Früher committen — Körper muss ankommen |
| `ANKLE FLEX` ✅ | Knöchel rollt durch Aufprall | Gute Stoßdämpfung — beibehalten |
| `MODERATE ROLL` ⚠️ | Etwas Knöchelmobilität | Knöchel bewusst entspannen |
| `STIFF ANKLE` ⚠️ | Minimales Abrollen | Mit weichem, entspanntem Knöchel landen |

---

### 6.4 Die Doppelstand-Karte lesen

| Badge | Verhältnis | Bedeutung | Trainingsimplikation |
| :--- | :--- | :--- | :--- |
| `OPTIMAL ROLL` ✅ | 18%–38% | Glatter, gegrundeter Transfer | Beibehalten — charakteristische WCS-Abrollverbindung |
| `HECTIC` ⚠️ | < 18% | Gehetzter Transfer | Verlangsamen; „Abschälen nicht Heben" |
| `SLUGGISH` ⚠️ | > 38% | Übermäßiger Doppelkontakt | Früher committen — Körper bewegen, nicht nur den Fuß |

---

### 6.5 Die Symmetrie & Glättigkeits-Karte lesen

| Anzeige | Bedeutung | Grünes Ziel |
| :--- | :--- | :--- |
| **ASI %** | Unterschied zwischen linkem und rechtem Fuß-Abrollen | `SYMMETRIC` — unter 10% |
| **Smoothness** | Flüssigkeit der Knöchelartikulierung über beide Füße | `SMOOTH` — 65 oder höher |

---

### 6.6 Fokus je nach Fertigkeitslevel

#### Anfänger
Nur auf die **Schritt-Badge Richtung + Badge** konzentrieren.

1. Vorwärts gehen → `OPTIMAL HEEL`? Falls nicht: Ferse anheben.
2. Rückwärts gehen → `OPTIMAL TOE` oder `ANCHOR SETTLE`? Falls nicht: Zehe zuerst schicken.
3. **1200-Hz-Piep** = stoppen und verlangsamen (`HEEL LANDING!` oder hartes `FLAT-FOOT!`).

#### Mittelstufe
1. Konsistentes `OPTIMAL HEEL` — Jerk-Balken unter 50%.
2. `OPTIMAL TOE` → `ANCHOR SETTLE`-Sequenz bei Ankern.
3. **Doppelstand-Karte**: `OPTIMAL ROLL` bei Triples anstreben.
4. **POWER PUSH-Badge**: Standbein aktiv einsetzen.

#### Fortgeschritten
1. **SMOOTH LOAD vs INSTANT LOAD** zum Feintuning des Gewichtsempfangs.
2. **ASI** links/rechts vergleichen — schlechtere Seite isoliert üben.
3. **ANKLE FLEX vs STIFF ANKLE** zur Ermüdungsüberwachung.
4. Mit `📷 CAM` filmen und während Pausen analysieren.

---

### 6.7 Häufige Probleme und Lösungen

| Was zu sehen | Grundursache | Lösung |
| :--- | :--- | :--- |
| `FLAT-FOOT!` bei jedem Vorwärtslauf | Knöchel starr; keine Fersenartikulierung | Verlangsamen. Fersen-Erstkontakt bewusst übertreiben. |
| `HEEL LANDING!` bei Rückwärtsschritten | Körpergewicht bewegt sich zu früh rückwärts | COM verzögern — erst Fuß schicken, Körper folgt. |
| `HECTIC` Doppelstand | Transfer gehetzt; Fuß hebt zu früh ab | „Letzter, der den Boden verlässt" — ganzen Fuß abschälen. |
| `SLUGGISH` Doppelstand | Zögern vor Gewichts-Commitment | Dem Boden vertrauen. Körper bewegen, nicht nur Fuß. |
| `STIFF ANKLE` konstant | Gebremstes Sprunggelenk | Vorstellen, auf einem Schwamm zu landen. |
| `ASYMMETRIC` ASI | Ein Fuß steifer oder weniger artikuliert | Schlechteren Fuß isoliert mit langsamen Abrollbewegungen üben. |
| `— PUSH-OFF` (kein Badge) | Standbein passiv | „Boden schieben, nicht nur Fuß heben." |

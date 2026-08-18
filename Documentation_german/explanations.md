# WCS-Verbindungs-Dashboard — Technische Referenz

Dieses Dokument beschreibt die mathematischen und biomechanischen Grundlagen des **Verbindungs-Dashboards** (`/`) — Verbindungskraftmessung, Jerk/Führungsqualität und Fußabroll-Visualisierung im Partner-Graphen.

> **Zielgruppe:** Entwickler und technisch interessierte Trainer. Für den tänzergerichteten Leitfaden siehe [dancer_guide_partner.md](dancer_guide_partner.md).

> Systemarchitektur, Sensorfusion, ESP-NOW-Protokoll und die vollständige Schritterkennungs-Pipeline sind unter [Solo-Dashboard — Technische Referenz](solo_explanations.md) beschrieben.

---

## 1. Fußabrollqualität (Partner-Graph)

Die Cyan- (links) und Magenta- (rechts) Linien im kombinierten Analyse-Graphen zeigen einen kontinuierlichen Qualitätswert aus Aufprallbeschleunigung und Rotationsartikulation:

$$\text{FootQuality} = \frac{\left| \text{gyro\_x} \right|}{1{,}0 + \max(0,\; \left| \text{accel\_z} \right| - 1{,}0) \times 2{,}0}$$

> Der Nenner bestraft nur Aufpralle oberhalb von 1 g — das Anheben des Fußes (Werte unter 1 g) verschlechtert den Qualitätswert nicht.

Ein **Stampf-Fehler** (vertikaler Cyan/Magenta-Marker) wird ausgelöst, wenn beide Bedingungen gleichzeitig gelten:

`|accel_z| > ACCEL_MAX`  und  `|gyro_x| < GYRO_MIN`

Die vollständige biomechanische Herleitung dieser Schwellenwerte (Rocker-Modell, Plantarflexionsphasen, empirische Validierung) ist unter [solo_explanations.md](solo_explanations.md) beschrieben.

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
    1.  **Gewichtsdelta (ΔW):** $\Delta W = \left| \text{weight}_{\text{current}} - \text{weight}_{\text{previous}} \right|$
    2.  **Räumlicher Beschleunigungs-Jerk-Vektor:**
        *   $\Delta a_x = a_{x,\text{current}} - a_{x,\text{previous}}$
        *   $\Delta a_y = a_{y,\text{current}} - a_{y,\text{previous}}$
        *   $\Delta a_z = a_{z,\text{current}} - a_{z,\text{previous}}$
        *   $\text{accelJerk} = \sqrt{(\Delta a_x)^2 + (\Delta a_y)^2 + (\Delta a_z)^2}$
    3.  **Roher Führungshärte-Index (`fuehrungshaerteRaw`):** $\text{fuehrungshaerteRaw} = \left(\frac{\Delta W}{50{,}0}\right) + (\text{accelJerk} \times 15{,}0)$

*   **Biomechanische Bedeutung:** Hohe Werte weisen auf abrupte Stöße, plötzliche Rucke oder Mikrostottern im Führungs-/Folge-Rahmen hin — kein kontinuierlicher, flüssiger Impulstransfer.

---

## 4. Schwellenwerte

| Parameter | Wert | Herleitung / Begründung |
| :--- | :--- | :--- |
| **`GYRO_MIN`** | $80{,}0\,\text{deg/s}$ | Empirisch aus Tests sauberer Fußartikulierung abgeleitet; Werte darunter fehlt die notwendige Rollbewegung. |
| **`ACCEL_MAX`** | $1{,}5\,g$ | Typische Grenze zwischen gedämpften Schritten und harten Aufprallstößen. |
| **Jerk-Schwellenwert** | $12{,}0$ (Rohindex) | Aktivierungswert für akustische Auslöser bei starken, unangenehmen Zugkräften. |
| **Diagramm-Skalierung** | $-3500\,\text{g}$ bis $+3500\,\text{g}$ | Erfasst die üblichen WCS-Verbindungskraftschwankungen ohne Clipping. |

---

## 5. Visualisierungsarchitektur

Das Frontend stellt Echtzeit-Daten über HTML5-Canvas-Elemente dar, die über `requestAnimationFrame`-Schleifen mit 20 ms Abfrageintervall aktualisiert werden:

1.  **Verbindungskraft-Diagramm (`graph_kraft`):**
    *   Echtzeit-Gewichtskurven über ein gleitendes 10-Sekunden-Fenster.
    *   Dynamische Linienfärbung: **Grün** (Kompression / positive Last) und **Rot** (Zugspannung / negative Last) basierend auf Nulldurchgangslogik.
2.  **Kombiniertes Analyse-Diagramm (`graph_kombi`):**
    *   **Fußqualitätskurven:** Linker Fuß (Cyan), rechter Fuß (Magenta) — kontinuierlicher Qualitätswert aus der Formel in §1.
    *   **Jerk-Verfolgungskurve:** Gelbe Linie — Führungshärte-/Jerk-Profil, auf Canvas-Koordinaten skaliert.
    *   **Fehlermarkierungen:** Vertikale Vollhöhen-Linien bei Schwellenwertüberschreitung:
        *   **Cyan / Blau:** Linker Fuß — Aufprall-/Artikulationsfehler.
        *   **Magenta / Lila:** Rechter Fuß — Aufprall-/Artikulationsfehler.
        *   **Yellow / Orange:** Hand-Jerk / Führungshärte-Spitze.

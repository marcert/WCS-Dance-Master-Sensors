# WCS Dashboard: Technische & biomechanische Dokumentation

Dieses Dokument bietet eine umfassende Aufschlüsselung der Messungen, mathematischen und biomechanischen Berechnungen, expliziten Schwellenwert-Konfigurationen und Visualisierungstechniken, die im **WCS-Dance-Master**-System implementiert sind.

---

## 1. Systemübersicht & Datenerfassung

Die Architektur basiert auf einem Mehrknotensensor-Netzwerk (Multi-Node Sensor Network), das über ESP-NOW-Protokolle mit einer zentralen Master-Einheit (M5Stack mit Webserver) kommuniziert.

*   **Fußknoten (IDs 1 & 2):** Ausgestattet mit IMUs (Inertial Measurement Units, Inertialmesseinheiten), die die Winkelgeschwindigkeit (`gyro_x`) und die vertikale Aufprallbeschleunigung (`accel_z`) erfassen.
*   **Hand-/Waageknoten (ID 3):** Misst strukturelle Lastkräfte (`weight`) kombiniert mit dreiachsigen Raumbeschleunigungen (`accel_x`, `accel_y`, `accel_z`).
*   **Controller-Knoten:** Empfängt die Sensordaten, verbindet sich mit dem WLAN (oder spannt einen eigenen Zugangspunkt auf) und liefert die HTML-Seite zur Visualisierung.

---

## 2. Mathematische, biomechanische & physikalische Definitionen

### A. Fußmechanik: Abrollqualität & Stampfen-Erkennung
Im West Coast Swing erfordert eine korrekte Fußmechanik eine gleichmäßige Gewichtsverlagerung und eine schrittweise Fersen-zu-Zehen- oder Zehen-zu-Fersen-Abwicklung (**Roll-off**, Abrollen), um abrupte, unarticulierte Aufpralle (**Stomping**, Stampfen) zu vermeiden.

*   **Gemessene Werte:**
    *   `gyro_x` (Winkelgeschwindigkeit um die Rollachse, gemessen in Grad pro Sekunde `deg/s`).
    *   `accel_z` (Vertikaler g-Kraft-Aufprallvektor).
*   **Berechnungen & Fehlerzustände:**
    Das System prüft auf starke Aufprallereignisse, denen eine ausreichende rotatorische Artikulation fehlt. Ein Fehlerzustand wird an einem der Füße ausgelöst, wenn:
    
    `|accel_z| > ACCEL_MAX`  und  `|gyro_x| < GYRO_MIN`
    
    *   **Biomechanische Bedeutung:** Der Tänzer trifft den Boden hart (überschreitet $1{,}5\,g$), ohne ausreichend durch das Fußgelenk abzurollen (ohne $80\,\text{deg/s}$ Rotation zu erreichen). Dies weist auf eine harte, ungepufferte Fußplatzierung hin.
### B. Handverbindungskraft & Gewichtsverteilung
*   **Gemessene Werte:**
    *   `weight` (Druck-/Zugkraft in Gramm, $\text{g}$).
*   **Biomechanische Bedeutung:** Repräsentiert die physische Spannung und Kompression, die durch den Verbindungsrahmen zwischen den Tanzpartnern übertragen wird.
    *   **Positive Werte ($> 0\,\text{g}$):** Kompression (drücken/aufeinander zugehen).
    *   **Negative Werte ($< 0\,\text{g}$):** Zugspannung (ziehen/voneinander wegstrecken).

### C. Hand-Jerk & Führungsgeschmeidigkeit
Jerk (Ruck) bezeichnet die Änderungsrate der Beschleunigung (oder abrupte Kraftspitzen), die direkt mit ruckartiger, unangekündigter Umlenkung oder Ziehen in der Partnerverbindung korreliert.

*   **Berechnungen:**
    1.  **Gewichtsdelta ($\Delta W$):** $\Delta W = \left| \text{weight}\_{\text{current}} - \text{weight}\_{\text{previous}} \right|$
    2.  **Räumlicher Beschleunigungs-Jerk-Vektor:**
        *   $\Delta a_x = a_{x,current} - a_{x,previous}$
        *   $\Delta a_y = a_{y,current} - a_{y,previous}$
        *   $\Delta a_z = a_{z,current} - a_{z,previous}$
        *   $\text{accelJerk} = \sqrt{(\Delta a_x)^2 + (\Delta a_y)^2 + (\Delta a_z)^2}$
    3.  **Roher Führungshärte-Index (`fuehrungshaerteRaw`):** $\text{fuehrungshaerteRaw} = \left(\frac{\Delta W}{50.0}\right) + (\text{accelJerk} \times 15.0)$

*   **Biomechanische Bedeutung:** Hohe Werte weisen auf abrupte Stöße, plötzliche Rucke oder Mikrostottern im Führungs-/Folge-Rahmen hin, anstatt auf einen kontinuierlichen, flüssigen Impulstransfer.
---

## 3. Schwellenwerte und Herleitung

| Parameter | Wert | Herleitung / Begründung |
| :--- | :--- | :--- |
| **`GYRO_MIN`** | $80{,}0\,\text{deg/s}$ | Abgeleitet aus empirischen Tests sauberer tänzerischer Fußartikulierung; Werte unterhalb dieser Schwelle weisen die notwendige Rollbewegung nicht auf. |
| **`ACCEL_MAX`** | $1{,}5\,g$ | Repräsentiert die typische Basisbeschleunigungsgrenze, die gedämpfte Schritte von harten Aufprallstößen trennt. |
| **Jerk-Schwellenwert** | $12{,}0$ (Rohindex) | Schwellenwert, ab dem taktiles Feedback oder akustische Auslöser aktiviert werden, um starke, unangenehme strukturelle Zugkräfte zu markieren. |
| **Diagramm-Skalierungsgrenzen** | $-3500\,\text{g}$ bis $+3500\,\text{g}$ | Gewählt, um die üblichen WCS-Verbindungskraftschwankungen komfortabel zu erfassen, ohne Clipping zu verursachen. |

---

## 4. Visualisierungsarchitektur

Das Frontend stellt Echtzeit-Daten über Hardware-Grenzen hinweg dar und verwendet HTML5 Canvas-Elemente, die über `requestAnimationFrame`-Schleifen mit einem Abfrageintervall von 20 ms aktualisiert werden:

1.  **Verbindungskraft-Diagramm (`graph_kraft`):**
    *   Zeigt Echtzeit-Gewichtskurven über ein gleitendes 10-Sekunden-Fenster an.
    *   Wechselt die Linienfärbung dynamisch zwischen **Green** (Kompression / positiver oder ausgeglichener Last) und **Red** (Zugspannung / negative Lastextreme) basierend auf der Nulldurchgangslogik.
2.  **Kombiniertes Analyse-Diagramm (`graph_kombi`):**
    *   **Fußqualitätskurven:** Visualisiert die berechnete Fußqualität links (Cyan-Linie) und rechts (Magenta-Linie), abgeleitet aus Aufprallabweichungsformeln:
        $$\text{LeftQuality} = \frac{\left| \text{currentLG} \right|}{1.0 + \max(0,\; \left| \text{currentLA} \right| - 1.0) \times 2.0}$$
        
        > Der Nenner bestraft nur Aufpralle oberhalb von 1 g — das Anheben des Fußes (Werte unter 1 g) verschlechtert den Qualitätswert nicht mehr.
    *   **Jerk-Verfolgungskurve:** Stellt das berechnete Führungshärte-/Jerk-Profil dar (gelbe gestrichelte/durchgezogene Linie, auf Canvas-Koordinaten skaliert).
    *   **Synchrone Fehlerlinien:** Vertikale, vollständig durchgehende Fehlermarkierungen werden automatisch gerendert, sobald Fußfehler (`isError`) oder Jerk-Spitzen (`isJerkPeak`) die konfigurierten Schwellenwerte überschreiten. Sie ermöglichen eine sofortige visuelle Korrelation mit der Kamerabild-Wiedergabe.
        *   **Cyan / Blue Lines:** Ausgelöst spezifisch durch **linke Fuß**-Aufprall-/Artikulationsfehler.
        *   **Magenta / Purple Lines:** Ausgelöst spezifisch durch **rechte Fuß**-Aufprall-/Artikulationsfehler.
        *   **Yellow / Orange Lines:** Ausgelöst durch plötzliche **Hand-Jerk / Führungshärte**-Spitzen.

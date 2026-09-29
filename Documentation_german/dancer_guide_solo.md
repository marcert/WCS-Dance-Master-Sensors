# WCS Dance Master Sensors — Tanzleitfaden: Solo-Dashboard

> Dieser Leitfaden richtet sich an Tänzer, nicht an Ingenieure. Die Mathematik dahinter muss nicht verstanden werden.  
> Öffne das Dashboard auf deinem Smartphone oder Tablet, folge den Einrichtungsschritten und nutze die Badge-Farbe als Echtzeit-Coach.

> ⚡ **Willst du einfach loslegen?** Überspring das hier und lies den **[Solo-Quickstart](quickstart_solo.md)** — in 60 Sekunden am Tanzen, der Rest kann warten.

→ Zur Partner-/Traineransicht siehe [dancer_guide_partner.md](dancer_guide_partner.md).

---

## 1. Erste Schritte

1. **Halte dein Smartphone oder Tablet in der Hand** vor dir im **Querformat**, sodass du die Badges zwischen den Schritten ablesen kannst. *(Möchtest du die Kameraüberlagerung nutzen? Dann stell es stattdessen auf ein Stativ auf Augenhöhe mit Blick auf dich.)*
2. **Öffne das Solo-Dashboard** in deinem Browser (`http://192.168.4.1/solo` im M5-Hotspot oder die auf dem M5-Display angezeigte Heimnetz-IP).
3. **Tippe auf `📷 CAM`**, um die Kameraüberlagerung zu aktivieren. Dein Live-Bild erscheint hinter den Datenkarten.
4. **Ziehe deine Tanzschuhe an**, bevor du mit dem nächsten Schritt fortfährst.
5. **Nimm deine natürliche Tanzstellung ein** — Füße schulterbreit, leichtes Gewicht nach vorne. Tippe auf `📐 ZERO`. Das System kennt jetzt, was „flacher Fuß auf dem Boden" für deine Schuhe bedeutet.
6. **Tippe auf `🔊 Biofeedback: OFF`**, um Audio einzuschalten. Die Pieptöne reagieren schneller als du den Bildschirm lesen kannst — sie sind dein primärer Hinweis.
7. Beginne zu gehen oder zu tanzen. Gönne dir 10–15 Schritte zum Aufwärmen, bevor du etwas analysierst.

> **Kalibriere neu, wann immer du Schuhe oder Untergrund wechselst.** Die `📐 ZERO`-Kalibrierung ist schuhspezifisch.

---

## 2. Layout

Im Querformat ist der Bildschirm in zwei Spalten unterteilt:

```
┌─────────────────────────┬──────────────────┬──────────────────┐
│                         │  Becken –        │  Doppelstand-    │
│   📷  KAMERA            │  Hüftmechanik    │  Überschneidung  │
│   hier sichtbar         │  (bei Sensor)    │                  │
├─────────────────────────├──────────────────┼──────────────────┤
│   Roll-off-Dynamik-     │  Roll-off-       │  Letzter Schritt │
│   Diagramm              │  Symmetrie (ASI) │  (Schritt-Badge) │
└─────────────────────────┴──────────────────┴──────────────────┘
```

- **Oben links**: zeigt die Karte **Becken — Hüftmechanik**, wenn der Beckensensor angebracht und eingeschaltet ist; andernfalls leer, damit die Kamera ungehindert sichtbar ist.
- **Unten links**: Live-Roll-off-Dynamik-Diagramm (wie schnell sich jeder Fuß beim Abrollen dreht, über die Zeit).
- **Oben rechts**: Doppelstand-Überschneidungs-Karte.
- **Unten rechts**: Letzter-Schritt-Karte (dein primäres Echtzeit-Feedback).
- **Unten Mitte**: Roll-off-Symmetrie- und Gleichmäßigkeits-Karte.

<p align="center">
  <kbd><img src="https://raw.githubusercontent.com/marcert/WCS-Dance-Master-Sensors/refs/heads/main/Attachments/scr.jpg" width="600"></kbd>
</p>

---

## 3. Trainingsgrad wählen

Der **`👤 BEG`**-Knopf in der oberen rechten Ecke der Kopfzeile wechselt zwischen drei Trainingsgraden. Tippe darauf, um zu wechseln; die Auswahl wird zwischen Sitzungen gespeichert.

| Knopf | Grad | Was sichtbar ist |
| :--- | :--- | :--- |
| `👤 BEG` (grün) | **Anfänger** | Nur [Schritt-Badge](#4-die-schritt-badge-karte)-Karte — Richtung, Strike-Badge, ROLL-Badge. [Beckenkarte](#9-die-beckenkarte-optionaler-sensor) zeigt [Hüftaktivierung](#hüftaktivierung), wenn Sensor aktiv. |
| `🏃 INT` (orange) | **Fortgeschritten** | [Schritt-Badge](#4-die-schritt-badge-karte) (vollständig, mit [Push-Off](#push-off-badge-int--adv)) + [Doppelstand](#6-die-doppelstand-karte). [Beckenkarte](#9-die-beckenkarte-optionaler-sensor) fügt [Laterale Stabilität](#laterale-stabilität-int), [Hüft-Fuß-Kopplung](#hüft-fuß-kopplung-int), [Vertikales Auf-und-Ab](#vertikales-auf-und-ab-int) hinzu. |
| `⭐ ADV` (lila) | **Experte** | Alle Karten — [Schritt-Badge](#4-die-schritt-badge-karte), [Doppelstand](#6-die-doppelstand-karte), [Roll-off-Symmetrie & Gleichmäßigkeit](#7-die-roll-off-symmetrie--und-gleichmäßigkeits-karte), [Grounding-Kachel](#8-die-grounding-kachel-adv). [Beckenkarte](#9-die-beckenkarte-optionaler-sensor) fügt [Anchor Settle](#anchor-settle-adv) hinzu. |

Karten, die für deinen Grad nicht relevant sind, werden ausgeblendet und geben der Kamera maximalen Bildschirmplatz.

> **Hinweis:** Diese Stufen beschreiben die **Komplexität des auf dem Bildschirm angezeigten Feedbacks** — sie haben nichts mit deiner WSDC-Wettkampfklasse zu tun. Ein Tänzer auf Champion-Niveau, der eine neue Übung beginnt, sollte `👤 BEG` verwenden, um sich auf eine Sache zu konzentrieren. Ein absoluter Neueinsteiger, der etwas Bestimmtes übt, kann von einem Wechsel zu `🏃 INT` profitieren. Wähle den Grad, der zu dem passt, woran du gerade arbeitest, nicht zu deinem Wettkampf-Lebenslauf.

<p align="center">
  <kbd><img src="https://raw.githubusercontent.com/marcert/WCS-Dance-Master-Sensors/refs/heads/main/Attachments/Buttons.jpg" width="600"></kbd>
</p>

### Anfängeransicht

Eine Karte, unten rechts. Alles andere ist Kamera. Schau nach dem [Richtungs-Badge](#wie-die-richtung-bestimmt-wird) und dem [Strike-Badge](#4-die-schritt-badge-karte) nach jedem Schritt. Nichts anderes.

### Fortgeschrittene Ansicht

Zwei Karten unten. [Schritttechnik](#4-die-schritt-badge-karte) rechts, [Timing-Qualität (Doppelstand)](#6-die-doppelstand-karte) links. Die obere Bildschirmhälfte bleibt Kamera.

### Experten-Ansicht

Vier Metrikkarten plus das Live-Diagramm: [Schritt-Badge](#4-die-schritt-badge-karte), [Doppelstand](#6-die-doppelstand-karte), [Roll-off-Symmetrie & Gleichmäßigkeit](#7-die-roll-off-symmetrie--und-gleichmäßigkeits-karte) und [Grounding-Kachel](#8-die-grounding-kachel-adv). Die Grounding-Kachel erscheint links neben dem (schmaleren) Diagramm und fasst Stoßdämpfung, Becken-Reaktion und den Grounding-Gesamtwert in einem Panel zusammen. Verwende diesen Grad für detaillierte Analysesitzungen, nicht zum Erlernen neuer Muster.

### Leader / Follower Modus

Der **👤 LEADER**-Button (blau) in der oberen Leiste schaltet auf **💃 FOLLOWER**-Modus (pink) um und zurück. Die Einstellung wird im Browser gespeichert und bleibt über Sessions hinweg erhalten.

**Wann Follower-Modus aktivieren:** Wenn du die Follower-Rolle tanzt. Das System passt drei Bewertungen an, weil reaktive (Follower-)Bewegung strukturell anders ist:

| Was angepasst wird | Was im Follower-Modus anders ist |
|---|---|
| **Gewichtsübergabe-Timing** (DELAY) | Der erwartete Zeitpunkt ist früher — Follower reagieren auf die Führung, ihre Gewichtsübergabe ist von Natur aus schneller |
| **Abdruck** (Push-Off) | Es wird ein etwas kompakterer Abdruck erwartet als beim Leader |
| **Seiten-Symmetrie** (ASI, links vs. rechts) | Größere Toleranz — durch die Verbindungsseite sind Follower strukturell etwas asymmetrischer |

**Kalibrierungsanzeige:** Im Follower-Modus zeigt der Richtungs-Badge zusätzlich den gemessenen Fußwinkel an (z. B. `⬅ BWD −4°`) und der Abdruck-Badge einen Stärke-Wert (z. B. `↗ PUSH 148`). Diese Zusatzwerte sind im Leader-Modus ausgeblendet, um die Anzeige übersichtlich zu halten.

---

## 4. Die Schritt-Badge-Karte

Dies ist die **primäre Echtzeit-Feedback-Karte**. Sie wird bei jedem erkannten Fußkontakt aktualisiert.

### Was die Anzeigeelemente bedeuten

| Element | Was es dir sagt |
| :--- | :--- |
| **Richtungs-Badge** | ➡ FWD (Fußwinkel +6° oder mehr), ⬅ BWD (unter −6°), oder — wenn der Winkel in der unklaren Zone liegt |
| **Strike-Badge** (großes farbiges Label) | Klassifizierung dieser Landung — siehe Tabellen unten |
| **ROLL-Badge** | Abrollqualität — wie der Vorfuß nach dem Fersenkontakt abgesenkt wird (alle Stufen) — [siehe Abschnitt unten](#roll-badge-alle-stufen) |
| **PUSH-OFF-Badge** | Push-off-Kraft deines hinteren Fußes (Anfänger: ausgeblendet) |
| **LOADING-Badge** | Wie gleichmäßig (Gradient) das Gewicht auf den Landefuß geladen wurde — [siehe Abschnitt unten](#lade-badge-nur-adv) (nur Experte) |
| **DELAY-Badge** | Wie schnell das Gewicht relativ zum Beat-Tempo übertragen wurde — [siehe Abschnitt unten](#verzögerungs-badge-int--adv) (Fortgeschritten + Experte) |
| **ANKLE ROLL-Badge** | Sprunggelenksdämpfung beim Aufsetzen (nur Experte) |

### Wie die Richtung bestimmt wird

Das System liest die Richtung aus der Neigung deines Fußes beim Aufsetzen ab (als Winkel angezeigt). Die Richtung ist **nur an den Extremen zuverlässig**:

| Richtungs-Badge | Fußwinkel beim Aufsetzen | Bedeutung |
| :--- | :--- | :--- |
| **➡ FWD** | +6° oder mehr (Zehen hoch) | Ferse hatte klar zuerst Kontakt |
| **—** (grau) | −6° bis +5° | Unklare Zone — Fuß zu flach, um die Richtung zu erkennen |
| **⬅ BWD** | unter −6° (Zehen runter) | Ballen hatte klar zuerst Kontakt |

Liegt der Fußwinkel zwischen −6° und +5°, kann das System die Richtung nicht zuverlässig bestimmen. Der Richtungs-Badge zeigt — (grau). Zur Richtungsprüfung die Kameraansicht nutzen.

### HEEL-Zone-Badges (➡ FWD, +6° oder mehr)

| Badge | Aufprall | Was du getan hast | Ziel |
| :--- | :--- | :--- | :--- |
| `HEEL STRIKE ✓` | sanft | Sauberer Fersenauftritt — kontrollierter Kontakt | Ziel für alle Vorwärtsgänge und Breaks |
| `HEEL SLAM ⚠` | hart | Harter Fersenaufprall — zu viel Landekraft | Knie beim Aufsetzen beugen und Sprunggelenk weicher machen |

### Unklare Zone (—, −6° bis +5°)

Der Fuß ist zu flach, um die Richtung zu bestimmen. Der Qualitäts-Badge wird dennoch ausgelöst:

| Badge | Aufprall | Was es bedeutet |
| :--- | :--- | :--- |
| `SOFT ✓` | sehr sanft | Leichte, kontrollierte Landung — gute Technik in dieser Zone |
| `MODERATE` | mittel | Mittlerer Aufprall — akzeptabel, aber verbesserungswürdig |
| `HARD IMPACT ⚠` | hart | Schwerer Flachfuß-Aufprall — Stampfmuster |
| `BRUSH+HEEL` | — | Flache Landung gefolgt von Fersenauftritt kurz danach — automatisch zu ➡ FWD umklassifiziert; korrekte Technik bestätigt |

Wenn der Richtungs-Badge — zeigt, die Kameraansicht zur Richtungsprüfung nutzen.

### TOE-Zone-Badges (⬅ BWD, unter −6°)

| Badge | Aufprall | Was du getan hast | Ziel |
| :--- | :--- | :--- | :--- |
| `TOE-FIRST ✓` | sanft | Sauberer Ballenauftritt — kontrollierte Landung | Ziel für alle Rückwärtsgänge, Anker, Streckungen |
| `TOE JAM ⚠` | hart | Harter Ballenaufprall — zu viel Landekraft | Streckung mäßigen; Landung durch das Sprunggelenk abfedern |

> **Hinweis zu frühem Fersenabsatz:** Setzt die Ferse beim Rückwärtsschritt vor dem Ballen auf, landet der Schritt in der unklaren Zone (—) statt bei ⬅ BWD. Wenn bei Schritten, die als Rückwärtsschritte gemeint sind, konstant SOFT/MODERATE/HARD IMPACT erscheinen, setzt die Ferse zu früh auf. Darauf achten, zuerst den Ballen aufkommen zu lassen und das Sprunggelenk entspannt zu halten, bis der Fuß vollständig steht.

### ROLL-Badge (alle Stufen)

Das ROLL-Badge misst, wie gleichmäßig sich der vordere Teil deines Fußes nach dem Fersenkontakt absenkt — ob du ihn kontrolliert herunterführst oder fallen lässt.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `CLEAN ROLL ✓` ✅ | Gleichmäßiges, kontrolliertes Absenken des Fußes — Sprunggelenk bleibt durchgehend aktiv | Gut — beibehalten |
| `MODERATE ROLL` ⚠️ | Etwas ungleichmäßiges Abrollen | Auf die Sequenz Ferse → Außenkante → Ballen achten; Sprunggelenk leicht aktiv halten |
| `SLAPPING` ❌ | Der vordere Fuß fällt unkontrolliert nach dem Fersenkontakt | Fuß bewusst abrollen: Ferse setzt auf, Gewicht wandert entlang der Außenkante, dann zum Ballen — Sprunggelenk durchgehend aktiv |

> **SLAPPING erklärt:** Nach dem Fersenkontakt muss der vordere Teil des Fußes *aktiv abgesenkt* werden — er darf nicht einfach fallen gelassen werden. Lässt du ihn fallen, klatscht er auf den Boden — meist hörbar. Das erzeugt einen harten Aufprall, dämpft weniger Stoß und schwächt deine Verbindung. Die Lösung: bewusste Sequenz Ferse → Außenkante → Ballen, mit durchgehend leicht aktivem Sprunggelenk.

> 📸 **[Screenshot: Schritt-Badge-Karte mit CLEAN ROLL ✓ Badge in der unteren Badge-Reihe]**

---

## 5. Abdruck- und Lade-Badges

Diese Badges erscheinen in der unteren Badge-Reihe der Schritt-Karte und werden nach jedem Schritt aktualisiert. Der Abdruck (Push-Off) ist ab **Fortgeschritten** sichtbar; Loading und Ankle Roll erst ab **Experte**.

### Abdruck-Badge (Push-Off, INT + ADV)

Zeigt, wie kräftig du dich mit dem hinteren Fuß vom Boden abdrückst.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `🚀 POWER PUSH` ✅ | Kräftiger Abdruck vom hinteren Fuß — gut sowohl für Vorwärtsantrieb als auch für die Anker-Umverteilung | Beibehalten — das System erwartet automatisch mehr Abdruck bei Vorwärtsgängen und weniger bei Ankern und Rückwärtsschritten |
| `↗ PUSH` ⚠️ | Abdruck erkannt, aber schwächer als gewünscht | Aktiver durch den Ballen des hinteren Fußes drücken; denke „Boden wegschieben" |
| `— PUSH-OFF` | Kein nennenswerter Abdruck erkannt | Hinterbein ist passiv. Am Ende jedes Ganges aktiv das Sprunggelenk strecken |

### Lade-Badge (nur ADV)

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `SMOOTH LOAD` ✅ | Gewicht wurde schrittweise auf den Landefuß übertragen | Gute Gelenkbiomechanik — beibehalten |
| `INSTANT LOAD` ⚠️ | Gewicht wurde beim Aufprall sofort und vollständig auf den Landefuß übertragen | Dein Gewicht langsamer ankommen lassen; den Boden „empfangen" statt darauf zu fallen |
| `EARLY UNLOAD` ⚠️ | Gewicht verlagert sich, bevor der Fuß sicher steht | Du eilst zum nächsten Schritt. Aktuelle Gewichtsverlagerung vollständig abschließen, bevor du dich bewegst |

### Verzögerungs-Badge (DELAY, INT + ADV)

Zeigt, wie schnell du dein Gewicht nach dem Fußkontakt auf den neuen Fuß übertragen hast — im Verhältnis zum Musiktempo, sodass dieselbe Bewegung bei langsamer und schneller Musik gleich bewertet wird. Im Idealfall „schwebt" dein Gewicht kurz und kommt erst nach dem Fuß an (typisches WCS-Gefühl).

| Badge | Was es bedeutet |
| :--- | :--- |
| `DELAYED ✓` ✅ | Für WCS typisches Schweben — dein Gewicht kommt kurz nach dem Fuß an |
| `QUICK` ⚠️ | Gewicht sofort beim Kontakt verlagert — mechanisch, nicht musikalisch |
| `LATE` ⚠️ | Gewicht nie vollständig übertragen — zu langes Schweben oder unvollständige Verlagerung (nur ADV) |

Bei **INT**-Stufe werden nur `DELAYED ✓` / `QUICK` angezeigt — jede verzögerte Übertragung ist bereits Fortschritt. `LATE` wird bei **ADV**-Stufe hinzugefügt, wo übermäßiges Schweben ebenfalls ein Problem wird.

> Das **LOADING-Badge** (`SMOOTH LOAD` / `INSTANT LOAD`) und das **DELAY-Badge** beantworten unterschiedliche Fragen:
> - LOADING: *War die Rampe gleichmäßig?* (Qualität des Ankommens)
> - DELAY: *Bist du rechtzeitig angekommen?* (Timing relativ zum Beat)

### Sprunggelenk-Roll-Badge (nur ADV)

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `RIGID LEVER` ✅ | Vollständiger Pronations-Supinations-Zyklus erkannt: Sprunggelenk hat Aufprall absorbiert UND für den Abstoß arretiert | Optimale Sprunggelenks-Biomechanik — beibehalten |
| `ANKLE FLEX` ✅ | Sprunggelenk rollt durch den Aufprall (Pronationsimpuls erkannt) | Gute Stoßdämpfung — Sprunggelenk wirkt als natürliche Feder |
| `MODERATE ROLL` ⚠️ | Etwas Sprunggelenksbeweglichkeit, könnte aber mehr sein | Sprunggelenk beim Aufsetzen bewusst entspannen; Fuß nicht starr halten |
| `STIFF ANKLE` ⚠️ | Minimales Sprunggelenks-Rollen — Aufprall direkt in die Kette weitergeleitet | Mit weichem, entspanntem Sprunggelenk landen. Langfristig reduziert dies die Knie- und Hüftbelastung |

---

## 6. Die Doppelstand-Karte

Sichtbar ab **Fortgeschritten**. Diese Karte zeigt, wie lange beide Füße während jeder Gewichtsverlagerung gleichzeitig auf dem Boden sind.

| Badge | Was es bedeutet | Trainingsimplikation |
| :--- | :--- | :--- |
| `OPTIMAL ROLL` ✅ | Gleichmäßige, geerdete Gewichtsverlagerung | Die charakteristische WCS-Roll-Verbindung |
| `HECTIC` ⚠️ | Gehetzt — ein Fuß verlässt den Boden, bevor der andere sicher steht | „Abrollen, nicht abheben" — durch den Fuß rollen, bevor man schreitet |
| `SLUGGISH` ⚠️ | Zu langer Doppelkontakt — Zögern oder schwere Stellung (die Toleranz ist bei langsamer Musik automatisch größer) | Dein Gewicht früher verlagern |

Beobachte diese Karte während **Tripleschritten und Gängen**. `HECTIC` bei einem Ankerschritt bedeutet oft, dass du den Anker verlässt, bevor du Verbindung aufgebaut hast.

> 📸 **[Screenshot: Doppelstand-Überschneidungs-Karte mit OPTIMAL ROLL Badge und angezeigtem Überschneidungsprozentsatz]**

---

## 7. Die Roll-off-Symmetrie- und Gleichmäßigkeits-Karte

Nur auf **Experten**-Stufe sichtbar. Diese Karte vergleicht deine beiden Füße und zeigt, wie flüssig du abrollst.

| Anzeige | Was es dir sagt | Grünes Ziel |
| :--- | :--- | :--- |
| **Seiten-Symmetrie** (ASI) | Wie ähnlich dein linker und rechter Fuß abrollen | `SYMMETRIC` — beide Seiten gleichwertig |
| **Gleichmäßigkeit** | Wie flüssig deine Sprunggelenke arbeiten | `SMOOTH` |

- `ASYMMETRIC` bei der Seiten-Symmetrie bedeutet meist, dass ein Sprunggelenk steifer ist oder eine Seite eine alte Verletzung kompensiert.
- Niedrige **Gleichmäßigkeit** (`ROUGH`) bedeutet, dass deine Sprunggelenksbewegungen ruckartig sind. Verlangsame das Tempo und konzentriere dich darauf, durch den ganzen Fuß zu rollen statt flach aufzusetzen.

---

## 8. Die Grounding-Kachel (ADV)

Nur auf **Experten**-Stufe sichtbar. Die Grounding-Kachel erscheint **links neben dem Diagramm** (das in ADV-Mode schmaler dargestellt wird, um Platz zu schaffen). Sie fasst drei Grounding-Qualitätssignale in einer Anzeige zusammen.

### Inhalte der Kachel

| Element | Benötigt | Was gemessen wird |
| :--- | :--- | :--- |
| **Stoßdämpfung** (SDR-Badge) | Beckensensor | Wie gut die Aufprallenergie vom Fuß bis zur Hüfte abgefedert wird |
| **Becken-Reaktion** (SETTLE-Badge) | Beckensensor | Ob dein Becken den Aufprall aktiv und rechtzeitig abfedert |
| **Grounding-Gesamtwert** (GND-Score) | Fußsensoren | Ein Gesamtwert (0–100), der Stoßdämpfung, Becken-Reaktion und Abrollqualität zusammenfasst; Balken wird grün / gelb / rot |

> Wenn der Beckensensor offline ist, werden Stoßdämpfung und Becken-Reaktion ausgeblendet. Der Gesamtwert spiegelt dann nur die Abrollqualität der Fußsensoren wider.

### Stoßdämpfung (SDR-Badge)

Zeigt, wie viel der Aufprallkraft auf dem Weg vom Fuß bis zum Becken abgefedert wird — also wie gut die Kette aus Sprunggelenk, Knie und Hüfte die Landung schluckt.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `ABSORBING ✓` ✅ | Wirksame Stoßdämpfung — die Bein-Kette schluckt die Aufprallenergie | Gut — weiche, entspannte Gelenke beim Aufsetzen beibehalten |
| `PARTIAL SDR` ⚠️ | Teilweise Dämpfung, aber ein Teil des Aufpralls wird weitergeleitet | Tiefer ins Knie gehen beim Fersenkontakt; Sprunggelenk mehr nachgeben lassen |
| `STIFF` ❌ | Kaum Dämpfung — der Aufprall gelangt direkt ins Becken | Mit weicherem Knie landen; bewusst durch Sprunggelenk und Knie abfedern, bevor die Hüfte reagiert |

### Becken-Reaktion (SETTLE-Badge)

Zeigt, ob dein Becken nach dem Fußkontakt aktiv nach unten „nachgibt" und damit den Aufprall abfedert — und ob das im richtigen Moment passiert (nicht zu früh, nicht zu spät). Der erwartete Zeitpunkt passt sich automatisch an das Musiktempo an.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `SETTLING ✓` ✅ | Die Beinkette federt den Aufprall aktiv ab — nachgiebige Knie- und Hüftreaktion | Beibehalten |
| `QUICK` ⚠️ | Das Becken gibt zu früh nach — die Gelenke sind zu steif für eine echte Dämpfungsphase | Knie beim Aufsetzen weicher lassen; die Beinkette zuerst abfedern lassen |
| `SLOW` ⚠️ | Die Beckenreaktion kommt zu spät — Gelenke werden träge aktiviert, der Aufprall wird passiv übertragen | Knie und Hüfte schon beim Fußkontakt aktiv einsetzen, nicht erst danach |

### Grounding-Gesamtwert (GND-Score)

Ein Gesamtwert von 0–100, der die drei Grounding-Signale zusammenfasst:

- **Stoßdämpfung** (Beckensensor erforderlich)
- **Becken-Reaktion** (Beckensensor erforderlich)
- **Abrollqualität** aus der Schritt-Karte

Der Balken unterhalb des Werts wird **grün**, **gelb** oder **rot**. Nutze ihn als schnellen Gesamtüberblick — fällt er ab, prüfe, welches der drei Signale zuerst die Farbe gewechselt hat.

> Den GND-Score als schnellen Überblick während intensiver Übungseinheiten nutzen. Wenn er abfällt, prüfen, welches Komponenten-Badge zuerst die Farbe gewechselt hat.

---

## 9. Die Beckenkarte (Optionaler Sensor)

Die Beckenkarte erscheint im **oberen linken Slot** des Dashboards, sobald der Beckensensor eingeschaltet ist. Wenn der Sensor offline ist, bleibt dieser Slot leer und die Kamera ist sichtbar.

### Sensor befestigen

Befestige den Sensor am **hinteren Hosenbund auf Höhe des unteren Rückens** (L5/Kreuzbeinniveau), Display von deinem Körper weg. Mittig an der Wirbelsäule ist ideal, aber einige Zentimeter links oder rechts der Mitte machen keinen praktischen Unterschied. Er sollte flach und fest sitzen — nicht herunterhängen.

### Befestigung überprüfen

Sobald der Beckensensor aktiv ist, erscheint die Beckenkarte oben links. Wenn die Hüftaktivierungs-Badge beim normalen Stehen dauerhaft `🌀 ACTIVE` zeigt (ohne Bewegung), ist der Sensor wahrscheinlich falsch befestigt oder dreht sich. Sensor neu anklipsen: flach am Rücken, Display nach außen.

### Badge-Übersicht

| Badge-Reihe | Stufe | Was gemessen wird |
| :--- | :--- | :--- |
| **Hüftaktivierung** | BEG+ | Wie stark sich das Becken während der Bewegung dreht (Gieren) |
| **Laterale Stabilität** | INT+ | Seitliches Schwingen des Beckens während der Bewegung |
| **Beckenkippung** | INT+ | Vor-/Rückwärts-Neigung des Beckens — Haltungsprüfung (`ALIGNED` / `SLIGHT ARCH` / `LORDOSIS ⚠` / `TUCKED`) |
| **Hüft-Fuß-Kopplung** | INT+ | Ob die Hüften jeden Schritt initiieren oder den Füßen folgen |
| **Vertikales Auf-und-Ab** | INT+ | Wie viel vertikale Bewegung das Becken erzeugt |
| **Anchor Settle** | ADV | Qualität der Beckensetzung kurz nach jedem Ankerschritt |
| **Hip Settle** | ADV | Ob du dich nach dem Ankerschritt in die Hüfte setzt (laterale Beckenneigung) |

> 📸 **[Screenshot: Beckenkarte oben links mit allen Badge-Reihen (Hüftaktivierung bis Anchor Settle) bei aktivem Sensor]**

---

### Hüftaktivierung

Misst, wie schnell sich deine Hüften drehen — der stärkste Moment innerhalb der letzten halben Sekunde.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `🌀 ACTIVE` ✅ | Starke Hüftrotation trägt zur Bewegung bei | Beibehalten |
| `MODERATE` ⚠️ | Etwas Rotation, aber Hüftbeitrag ist begrenzt | Hüfte vor jedem Schritt aufwickeln — Rotation sollte im Becken beginnen, nicht im Sprunggelenk |
| `STIFF HIPS` ❌ | Becken dreht sich kaum — Bewegung nur durch Beine angetrieben | Zuerst isolierte Hüftrotationen üben, dann Füße hinzufügen. „Mit der Hüfte führen, nicht mit der Ferse." |

> Im WCS sollte Hüftrotation jeden Schritt begleiten oder ihm vorausgehen. `STIFF HIPS` ist eines der häufigsten Anfängermuster und für Fußsensoren allein unsichtbar.

---

### Laterale Stabilität (INT+)

Misst, wie stark deine Hüften seitlich schwingen, über 1 Sekunde.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `STABLE` ✅ | Minimale laterale Beckenbewegung — gute horizontale Kontrolle | Gut |
| `SLIGHT SWAY` ⚠️ | Etwas seitliche Schwingung — häufig bei Drehungen oder Übergängen | Auf Hüftheben auf der Schrittseite achten; Becken waagerecht halten |
| `LATERAL SWAY` ❌ | Deutliche Seitwärtsbewegung | Nach kompensatorischem Hüftdruck bei jedem Schritt suchen; Gänge mit bewusst waagerechtem Becken üben |

---

### Hüft-Fuß-Kopplung (INT+)

Vergleicht, wann die maximale Hüftrotation relativ zum Moment des Fußkontakts aufgetreten ist.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `HIP LEADS` ✅ | Die Hüftrotation ist deutlich vor dem Fußkontakt am stärksten | Gute Initiierung — Hüften treiben den Schritt an |
| `IN SYNC` ⚠️ | Hüftmaximum und Fußkontakt fallen fast zusammen | Akzeptabel — versuche den „Abschuss" der Hüfte vor dem Schritt zu verstärken |
| `HIP LAGS` ❌ | Hüften rotieren erst beim oder nach dem Fußkontakt | Beine bewegen sich unabhängig vom Rumpf. Verlangsamen und jeden Gang von der Hüfte aus initiieren üben, dabei den Fuß folgen lassen |

---

### Vertikales Auf-und-Ab (INT+)

Misst, wie stark sich das Becken beim Tanzen auf und ab bewegt. Der Sensor erkennt dabei nur die Auf-und-Ab-Bewegung — reines Stehen ruhig ergibt `GROUNDED`.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `GROUNDED` ✅ | Minimale vertikale Bewegung | Gut |
| `SLIGHT BOUNCE` ⚠️ | Etwas vertikale Schwingung | Leichte Kniebeugung durchgehend beibehalten; Knie beim Vorwärtsgehen nicht ganz durchstrecken |
| `BOUNCY` ❌ | Deutliche Auf-und-Ab-Bewegung | In den Knien bleiben. Denke: „Tief bleiben, verbunden bleiben." |

---

### Anchor Settle (ADV)

Nach jedem Rückwärts-(Anker-)Schritt öffnet das System ein **tempo-adaptives Messfenster** und wertet drei Signale aus:

1. **Abbremsen** — haben deine Hüften ihre Vorwärts-/Rückwärtsbewegung abgestoppt?
2. **Rotation lässt nach** — hat deine Hüftrotation nach dem Schritt nachgelassen?
3. **Ruhe** — wie ruhig waren deine Hüften in der zweiten Hälfte des Fensters?

> **Was gemessen wird:** Der Anchor-Settle-Score wertet Abstopp-Bewegung (vorwärts-rückwärts), Rotationsdämpfung und Stabilität aus. Das „In-die-Hüfte-Setzen" (leichte seitliche Beckenneigung beim Anker) wird separat vom **Hip-Settle**-Badge erfasst (siehe unten) — es fließt nicht in den Anchor-Settle-Score ein.

Diese drei Komponenten werden zu einem Wert von 0–100 zusammengefasst, der im Badge angezeigt wird.

| Badge | Wert | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- | :--- |
| `ANCHORED (n)` ✅ | ≥ 50 | Starkes Abbremsen + nachlassende Rotation + stabile Haltung | Gut — an Konsistenz bei jedem Ankerschritt arbeiten |
| `SETTLING (n)` ⚠️ | 30–49 | Teilweise Setzung — eine oder zwei Komponenten schwach | Schwache Komponente mit den untenstehenden Tipps identifizieren |
| `UNSTABLE (n)` ❌ | < 30 | Becken bewegt sich oder wackelt noch nach dem Anker | „Anker festsetzen" — das Ende des Slots erreichen und halten |

**Niedrigen Wert diagnostizieren:**
- **Niedriger Wert trotz sauberer Fußtechnik** → das Problem liegt im Becken, nicht im Fußwinkel. An der Setzung selbst arbeiten, nicht am Schritt.
- **`HECTIC`-Doppelstand + niedriger Anchor-Settle** → du verlässt den Anker, bevor sich das Becken stabilisiert hat.
- **`🌀 ACTIVE`-Hüfte + `UNSTABLE`-Anker** → Hüften rotieren gut während der Bewegung, dämpfen aber am Anker nicht ab. Einen bewussten „sanften Stopp" üben.

---

### Hip Settle (ADV)

Misst, ob du dich nach dem Ankerschritt „in die Hüfte setzt" — d. h. ob eine kurze seitliche Hüftbewegung zur Standbeinseite hin stattfindet und dann stabil gehalten wird. Das System betrachtet deine seitliche Hüftbewegung im gleichen Messfenster wie Anchor Settle.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `HIP SETTLE ✓` | Klarer lateraler Impuls früh im Fenster, danach stabile Haltung — Becken setzt sich bewusst auf das Standbein | Beibehalten |
| `SLIGHT SETTLE` | Kleiner lateraler Impuls vorhanden, aber nicht ausgeprägt | Bewusst mehr Gewicht auf das Standbein „fallen lassen" und halten |
| `NO HIP SETTLE` | Kein lateraler Impuls — Becken bleibt neutral nach dem Anker | Das Standbein aktiv belasten: nach dem Ankerschritt die Hüfte leicht zur Standbeinseite absinken lassen |
| `OVERSWING ⚠` | Zu starker lateraler Impuls — Becken schwingt zu weit zur Seite | Bewegung mäßigen; laterale Neigung soll subtil sein, nicht sichtbar pendeln |

> **Hinweis:** Das „In-die-Hüfte-Setzen" ist ein stilistisches Merkmal — manche Lehrstile betonen es stark, andere weniger. Im WCS ist die laterale Beckenbewegung bewusst subtiler als z. B. im Latin-Tanz: es geht um ein „geerdetes Ankommen", nicht um eine sichtbare Schwingung. Der Badge gibt Information, keine Bewertung. Wenn dein Trainer keinen lateralen Settle möchte, ignoriere diesen Badge.
>
> **Hinweis zu den Schwellenwerten:** Diese Werte basieren auf biomechanischen Referenzdaten und können nach dem ersten Testlauf mit Beckensensor angepasst werden.

---

## 10. Training nach Level

### Anfänger — `👤 BEG` verwenden

**Ein Fokus: Fersen- vs. Zehenkontakt.**

1. Vorwärts gehen. Zeigt das Badge `HEEL STRIKE ✓`? Wenn nicht — die Ferse hebt sich vor dem Aufsetzen nicht genug an. Bewusst die Ferse zuerst aufkommen lassen.
2. Rückwärts gehen. Zeigt das Badge `TOE-FIRST ✓`? Wenn nicht — zuerst den Ballen aufkommen lassen. Wenn bei Rückwärtsschritten der Richtungs-Badge — (grau) erscheint, setzt die Ferse vor dem Ballen auf.
3. Wenn du den **1200-Hz-Piep** hörst, anhalten und verlangsamen. Das ist `HEEL SLAM ⚠`, `TOE JAM ⚠` oder `HARD IMPACT ⚠` — zu viel Aufprallkraft.
4. Bei langsamem Tempo üben, bis `HEEL STRIKE ✓` und `TOE-FIRST ✓` konsistent erscheinen. Erst dann Tempo erhöhen.

**Mit Beckensensor:** Nur **Hüftaktivierung** beobachten. Wenn `STIFF HIPS` konstant erscheint, bewegen sich deine Beine ohne Rumpfbeteiligung.

### Fortgeschritten — `🏃 INT` verwenden

**Zwei Fokuspunkte: Technik-Konsistenz + Gewichtsübertragungs-Timing.**

1. Vorwärtsgänge → auf konstantes `HEEL STRIKE ✓` achten. ROLL-Badge prüfen: `CLEAN ROLL ✓` bedeutet, dass du den vorderen Teil des Fußes kontrolliert absetzt.
2. Rückwärtsgänge → auf `TOE-FIRST ✓` achten. Ein — Richtungs-Badge bei einem Rückwärtsschritt bedeutet, dass der Fuß zu flach landet — die Ferse setzt vor dem Ballen auf.
3. Das **POWER PUSH-Badge** beobachten: ist dein hinteres Bein passiv?
4. Die **Doppelstand-Karte** einführen: bei Tripleschritten auf `OPTIMAL ROLL` hinarbeiten.
5. Das neue **DELAY-Badge** beobachten: bei Ankerschritten auf `DELAYED ✓` achten. Konstantes `QUICK` bedeutet, dass das Gewicht sofort auf den Standfuß übertragen wird — kein musikalischer Atemzug in der Verbindung.

**Mit Beckensensor:** **Laterale Stabilität**, **Hüft-Fuß-Kopplung** und **Vertikales Auf-und-Ab** hinzufügen. Die wertvollste Einzelmetrik auf dieser Stufe ist die Hüft-Fuß-Kopplung — konstantes `HIP LAGS` bedeutet, dass du mit deinen Füßen gehst, nicht mit deinem Körper.

### Experte — `⭐ ADV` verwenden

**Vollständige biomechanische Feedback-Schleife.**

1. **SMOOTH LOAD vs. INSTANT LOAD** verwenden, um den Gewichtstransfer zu verfeinern — besonders bei synkopierten Mustern. SMOOTH LOAD bedeutet: das Gewicht fließt gleichmäßig über eine Rampe auf den Standfuß. INSTANT LOAD bedeutet: das Gewicht springt sofort beim Aufprall auf den Standfuß, ohne diese Rampe.
2. **DELAY-Badge** mit **SMOOTH LOAD** kreuzlesen: `DELAYED ✓` + `SMOOTH LOAD` ist die ideale Kombination — sowohl Timing als auch Qualität der Gewichtsrampe stimmen. `DELAYED ✓` + `INSTANT LOAD` bedeutet, du hast gewartet, dann aber das Gewicht schlagartig übertragen; `QUICK` + `SMOOTH LOAD` bedeutet, die Gewichtsrampe war gut, aber der Moment des Übertragens kam zu früh.
3. **ASI** zwischen links und rechts über eine vollständige Trainingssitzung vergleichen. Eine konstant schlechtere Seite weist auf ein Kompensationsmuster hin.
4. **ANKLE FLEX vs. STIFF ANKLE** zur Ermüdungsüberwachung nutzen — Sprunggelenkssteifigkeit nimmt bei Muskelermüdung zu.
5. Mit `📷 CAM` aufnehmen und während der Pausen abspielen.
6. Das **Roll-off-Dynamik-Diagramm** verwenden, um zu vergleichen, wie schnell sich jeder Fuß beim Abrollen dreht.

**Mit Beckensensor:** **Anchor Settle** als Maß für die Anker-Qualität verwenden. Grundfiguren wie Sugar Push, Left Side Pass oder Underarm Turn tanzen und den Wert nach jedem Ankerschritt prüfen. Den **GND-Score** auf der Grounding-Kachel als Gesamt-Grounding-Indikator nutzen — fällt er ab, prüfen, welches Komponenten-Badge (ROLL, SDR oder SETTLE) zuerst die Farbe gewechselt hat.

---

## 11. Häufige Probleme und Lösungen

| Was du siehst | Ursache | Lösung |
| :--- | :--- | :--- |
| `HEEL SLAM ⚠` / `HARD IMPACT ⚠` bei Vorwärtsgängen | Zu wenig Knie- oder Sprunggelenksdämpfung beim Aufsetzen | Verlangsamen. Knie stärker beugen und Sprunggelenk weicher halten beim Aufsetzen. |
| `SLAPPING` bei Vorwärtsschritten | Der vordere Teil des Fußes fällt nach dem Fersenkontakt unkontrolliert herab | Bewusst abrollen: Ferse → Außenkante → Ballen, Sprunggelenk durchgehend aktiv. Der vordere Fuß muss aktiv abgesenkt werden, nicht fallen gelassen. |
| — Richtungs-Badge bei Rückwärtsschritten | Ferse setzt vor dem Ballen auf | Zuerst den Ballen aufkommen lassen, Sprunggelenk entspannt halten, bis der Fuß vollständig steht. |
| `TOE JAM ⚠` konstant | Harter Ballenaufprall bei Rückwärtsschritten | Streckung mäßigen; Landung durch das Sprunggelenk abfedern. |
| `HECTIC`-Doppelstand | Verlagerung gehetzt; Fuß hebt zu früh ab | „Als Letztes den Boden verlassen" — den ganzen Fuß von der Zehe abrollen lassen. |
| `SLUGGISH`-Doppelstand | Zögern vor dem vollständigen Gewichtstransfer | Dem Boden vertrauen. Den Körper bewegen, nicht nur den Fuß. |
| `STIFF ANKLE` konstant | Sprunggelenk beim Aufsetzen gespannt | Sich vorstellen, auf einem Schwamm zu landen. Sprunggelenk bewusst entspannen vor dem Kontakt. |
| `ASYMMETRIC`-ASI | Ein Fuß steifer oder weniger artikuliert | Betroffenen Fuß identifizieren und isoliert üben. |
| `— PUSH-OFF` (kein Badge) | Hinterbein passiv | „Boden drücken, nicht nur Fuß heben." |
| `INSTANT LOAD` bei Ankern | Gewicht schlagartig auf den Standfuß übertragen | „In den Anker schmelzen" statt „darauf fallen". |
| `QUICK` bei Ankerschritten | Kein Schweben vor dem Gewichtstransfer | „Zurücktreten, atmen, dann setzen." Bewusste Pause zwischen Fußkontakt und Gewichtsübernahme einplanen. |
| `LATE` bei Vorwärtsgängen | Gewicht kommt nie vollständig an | Der Verlagerung vertrauen — vollständig übertragen, bevor der nächste Schritt eingeleitet wird. |
| `STIFF HIPS` konstant | Beine ohne Rumpfbeteiligung | Jeden Schritt mit einem bewussten Hüftrotationsimpuls beginnen, bevor der Fuß sich bewegt. |
| `LATERAL SWAY` durchgehend | Hüftheben oder seitlicher Beckendruck | Becken waagerecht halten; auf asymmetrische Gewichtsverteilung achten. |
| `HIP LAGS` bei jedem Schritt | Beine und Rumpf entkoppelt | Sehr langsames Tempo. Hüftrotation initiieren, dann Fuß folgen lassen. |
| `BOUNCY` durchgehend | Kniestreckung während der Bewegung | Leichte Kniebeugung durchgehend beibehalten. Die Kopfhöhe sollte sich zwischen den Schritten nicht verändern. |
| `UNSTABLE`-Anker immer | Becken dreht sich noch nach dem Anker | Zurücktreten, beide Füße setzen und bewusst alle Hüftbewegung stoppen. Zwei Zählzeiten halten. |

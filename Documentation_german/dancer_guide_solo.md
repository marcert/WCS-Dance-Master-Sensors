# WCS Dance Master Sensors — Tanzleitfaden: Solo-Dashboard

> Dieser Leitfaden richtet sich an Tänzer, nicht an Ingenieure. Die Mathematik dahinter muss nicht verstanden werden.  
> Öffne das Dashboard auf deinem Smartphone oder Tablet, folge den Einrichtungsschritten und nutze die Badge-Farbe als Echtzeit-Coach.

→ Zur Partner-/Traineransicht siehe [dancer_guide_partner.md](dancer_guide_partner.md).

---

## 1. Erste Schritte

1. **Befestige dein Smartphone oder Tablet auf einem Stativ** auf Augenhöhe, mit Blick auf dich. Verwende das **Querformat** für beste Ergebnisse.
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
- **Unten links**: Live-Roll-off-Dynamik-Diagramm (Nickwinkel-Winkelgeschwindigkeit beider Füße über die Zeit).
- **Oben rechts**: Doppelstand-Überschneidungs-Karte.
- **Unten rechts**: Letzter-Schritt-Karte (dein primäres Echtzeit-Feedback).
- **Unten Mitte**: Roll-off-Symmetrie- und Gleichmäßigkeits-Karte.

---

## 3. Trainingsgrad wählen

Der **`👤 BEG`**-Knopf in der oberen rechten Ecke der Kopfzeile wechselt zwischen drei Trainingsgraden. Tippe darauf, um zu wechseln; die Auswahl wird zwischen Sitzungen gespeichert.

| Knopf | Grad | Was sichtbar ist |
| :--- | :--- | :--- |
| `👤 BEG` (grün) | **Anfänger** | Nur [Schritt-Badge](#4-die-schritt-badge-karte)-Karte — Richtung, Winkel, Strike-Badge. [Beckenkarte](#8-die-beckenkarte-optionaler-sensor) zeigt [Hüftaktivierung](#hüftaktivierung), wenn Sensor aktiv. |
| `🏃 INT` (orange) | **Fortgeschritten** | [Schritt-Badge](#4-die-schritt-badge-karte) (vollständig, mit [Jerk](#aufprall-jerk-leiste) + [Push-Off](#push-off-badge-int--adv)) + [Doppelstand](#6-die-doppelstand-karte). [Beckenkarte](#8-die-beckenkarte-optionaler-sensor) fügt [Laterale Stabilität](#laterale-stabilität-int), [Hüft-Fuß-Kopplung](#hüft-fuß-kopplung-int), [Vertikales Auf-und-Ab](#vertikales-auf-und-ab-int) hinzu. |
| `⭐ ADV` (lila) | **Experte** | Alle Karten — [Schritt-Badge](#4-die-schritt-badge-karte), [Doppelstand](#6-die-doppelstand-karte), [Roll-off-Symmetrie & Gleichmäßigkeit](#7-die-roll-off-symmetrie--und-gleichmäßigkeits-karte). [Beckenkarte](#8-die-beckenkarte-optionaler-sensor) fügt [Anchor Settle](#anchor-settle-adv) hinzu. |

Karten, die für deinen Grad nicht relevant sind, werden ausgeblendet und geben der Kamera maximalen Bildschirmplatz.

> **Hinweis:** Diese Stufen beschreiben die **Komplexität des auf dem Bildschirm angezeigten Feedbacks** — sie haben nichts mit deiner WSDC-Wettkampfklasse zu tun. Ein Tänzer auf Champion-Niveau, der eine neue Übung beginnt, sollte `👤 BEG` verwenden, um sich auf eine Sache zu konzentrieren. Ein absoluter Neueinsteiger, der etwas Bestimmtes übt, kann von einem Wechsel zu `🏃 INT` profitieren. Wähle den Grad, der zu dem passt, woran du gerade arbeitest, nicht zu deinem Wettkampf-Lebenslauf.

### Anfängeransicht

Eine Karte, unten rechts. Alles andere ist Kamera. Schau nach dem [Richtungs-Badge](#wie-die-richtung-bestimmt-wird) und dem [Strike-Badge](#4-die-schritt-badge-karte) nach jedem Schritt. Nichts anderes.

### Fortgeschrittene Ansicht

Zwei Karten unten. [Schritttechnik](#4-die-schritt-badge-karte) rechts, [Timing-Qualität (Doppelstand)](#6-die-doppelstand-karte) links. Die obere Bildschirmhälfte bleibt Kamera.

### Experten-Ansicht

Drei Metrikkarten plus das Live-Diagramm: [Schritt-Badge](#4-die-schritt-badge-karte), [Doppelstand](#6-die-doppelstand-karte) und [Roll-off-Symmetrie & Gleichmäßigkeit](#7-die-roll-off-symmetrie--und-gleichmäßigkeits-karte). Verwende diesen Grad für detaillierte Analysesitzungen, nicht zum Erlernen neuer Muster.

---

## 4. Die Schritt-Badge-Karte

Dies ist die **primäre Echtzeit-Feedback-Karte**. Sie wird bei jedem erkannten Fußkontakt aktualisiert.

### Was die Anzeigeelemente bedeuten

| Element | Was es dir sagt |
| :--- | :--- |
| **➡️ FORWARD** / **⬅️ BACKWARD** | Richtung des Schritts, der gerade gelandet ist |
| **↔️ FLAT** | Fuß zu flach gelandet, um zu klassifizieren — als Flachfuß-Warnung behandeln |
| **θ-Winkel** | Nickwinkel deines Fußes beim Aufsetzen. Positiv = Ferse höher als Zehe. |
| **Strike-Badge** (großes farbiges Label) | Klassifizierung dieser Landung — siehe Tabellen unten |
| **Jerk-Leiste** | Aufprallkraftrate — wie hart dein Fuß den Boden getroffen hat |
| **PUSH-OFF-Badge** | Push-off-Kraft deines hinteren Fußes (Anfänger: ausgeblendet) |
| **LOADING-Badge** | Wie gleichmäßig (Gradient) das Gewicht auf den Landefuß geladen wurde — [siehe Abschnitt unten](#lade-badge-nur-adv) (nur Experte) |
| **DELAY-Badge** | Wie schnell das Gewicht relativ zum Beat-Tempo übertragen wurde — [siehe Abschnitt unten](#verzögerungs-badge-int--adv) (Fortgeschritten + Experte) |
| **ANKLE ROLL-Badge** | Sprunggelenksdämpfung beim Aufsetzen (nur Experte) |

### Wie die Richtung bestimmt wird

Das System klassifiziert die Richtung anhand des Nickwinkels θ des Fußes. Die Richtung ist **nur an den Extremen zuverlässig**:

| Richtungs-Badge | θ beim Aufsetzen | Bedeutung |
| :--- | :--- | :--- |
| **➡️ FORWARD** | θ ≥ +10° | Klare Dorsalflexion — Ferse hat zuerst Kontakt |
| **↔️ FLAT** | −5° bis +9° | Unklare Zone — Fuß zu flach für Richtungsbestimmung |
| **⬅️ BACKWARD** | θ < −5° | Klare Plantarflexion — Ballen hat zuerst Kontakt |

Ein einzelner Fuß-IMU kann einen flachen Vorwärtsschritt nicht von einem Rückwärtsschritt mit frühem Fersenabsatz trennen, wenn θ zwischen −5° und +9° liegt. Beide zeigen ↔️ FLAT.

### Vorwärtsschritt-Badges (➡️ FORWARD, θ ≥ +10°)

| Badge | Winkel | Was du getan hast | Ziel |
| :--- | :--- | :--- | :--- |
| `HEEL SPIKE` ⚠️ | > +35° | Extrem steiler Fersenwinkel | Antriebskraft reduzieren oder Sprunggelenk entspannen |
| `OPTIMAL HEEL` ✅ | +10° bis +35° | Sauberer Fersenauftritt — Ferse setzt zuerst auf | Ziel für alle Vorwärtsgänge und Breaks |
| `BRUSH+HEEL` ✅ | flach → +8° Ferse innerhalb von 200 ms | Ballen während des Schwungs gestreift, dann Ferse sauber gesetzt | Wird von FLAT-FOOT! umklassifiziert, wenn der Sensor den zweiphasigen Kontakt erkennt |

### Unklare Zone (↔️ FLAT, −5° bis +9°)

| Badge | Bedingung | Was es bedeutet |
| :--- | :--- | :--- |
| `FLAT-FOOT!` ❌ | flache Landung, kein Nachgang | Könnte sein: Vorwärtsschritt mit unzureichendem Fersenanheben ODER Rückwärtsschritt mit zu frühem Fersenabsatz. Kamera zur Richtungsüberprüfung verwenden. |
| `BRUSH+HEEL` ✅ | flach → Fersenauftritt innerhalb von 200 ms erkannt | System klassifiziert automatisch um — korrekte Vorwärtstechnik bestätigt |

### Rückwärtsschritt-Badges (⬅️ BACKWARD, θ < −5°)

| Badge | Winkel | Was du getan hast | Ziel |
| :--- | :--- | :--- | :--- |
| `OPTIMAL TOE` ✅ | −5° bis −20° | Sauberer Ballen-Kontakt | Ziel für alle Rückwärtsgänge, Anker, Streckungen |
| `HEEL SPIKE` ⚠️ | < −20° | Übermäßig gestreckter Fuß beim Kontakt | Streckung leicht mäßigen |

> **Hinweis zu Rückwärts-Fersenfehlern:** Ein Rückwärtsschritt, bei dem die Ferse zu früh abgesetzt wird (θ = +5° bis +9°) oder zuerst auftrifft (θ ≥ +10°), zeigt ↔️ FLAT oder ➡️ FORWARD — der Sensor kann die Rückwärtsrichtung in diesen Bereichen nicht bestätigen. Wenn du bei Schritten, die du für Rückwärtsschritte hältst, konstant FLAT-FOOT! siehst, setzt deine Ferse höchstwahrscheinlich zu früh auf. Konzentriere dich darauf, zuerst die Zehe hinauszuschicken und das Sprunggelenk dorsalflektiert zu halten, bis der Fuß aufsetzt.

### Aufprall-Jerk-Leiste

- **Kurze Leiste, kein Klick** → weiche, kontrollierte Landung. Ideal.
- **Volle rote Leiste + 500-Hz-Klick** → schweres Stampfen. Knie beim Aufsetzen beugen und mit dem Sprunggelenk abfedern.

---

## 5. Push-Off- und Lade-Badges

Diese drei Badges erscheinen unterhalb der Jerk-Leiste und werden nach jedem Schritt aktualisiert. Sichtbar ab **Fortgeschritten** (Push-Off) oder **Experte** (Loading + Ankle Roll).

### Push-Off-Badge (INT + ADV)

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `🚀 POWER PUSH` ✅ | Starker Abstoß vom hinteren Fuß — optimal sowohl für Vorwärtsantrieb als auch für Anker-Umverteilung | Beibehalten — das System passt seinen Zielwert automatisch an: höherer Schwellenwert für Vorwärtsgänge, niedriger für Anker und Rückwärtsschritte |
| `↗ PUSH` ⚠️ | Abstoß erkannt, aber unter dem Richtungsziel | Aktiver durch den Ballen des hinteren Fußes drücken; denke „Boden wegschieben" |
| `— PUSH-OFF` | Kein signifikanter Abstoß erkannt | Hinterbein ist passiv. Am Ende jedes Ganges aktiv das Sprunggelenk strecken |

### Lade-Badge (nur ADV)

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `SMOOTH LOAD` ✅ | Gewicht wurde schrittweise auf den Landefuß übertragen | Gute Gelenkbiomechanik — beibehalten |
| `INSTANT LOAD` ⚠️ | Volles Gewicht beim Aufprall abgeworfen | Massenschwerpunkt verlangsamen; daran denken, den Boden zu „empfangen" statt darauf zu landen |
| `EARLY UNLOAD` ⚠️ | Gewicht verlagert sich, bevor der Fuß sicher steht | Du eilst zum nächsten Schritt. Aktuelle Gewichtsverlagerung vollständig abschließen, bevor du dich bewegst |

### Verzögerungs-Badge (INT + ADV)

Misst, wie schnell du dein Gewicht nach dem Fußkontakt übertragen hast, ausgedrückt als **Bruchteil deines aktuellen Schrittintervalls** — passt sich also automatisch an das Musiktempo an. Dieselbe körperliche Bewegung liest sich bei 90 BPM und 160 BPM identisch.

Die Schwellenwerte unterscheiden sich je nach Richtung: Ein Rückwärtsschritt (Zehen zuerst) braucht natürlich mehr Setzzeit als ein Vorwärtsschritt (Ferse zuerst).

| Badge | Vorwärtsschritt | Rückwärtsschritt | Was es bedeutet |
| :--- | :--- | :--- | :--- |
| `DELAYED ✓` ✅ | 12–38 % des Beats | 18–50 % des Beats | Für WCS typisches Schweben — Gewicht kommt nach dem Fußkontakt an |
| `QUICK` ⚠️ | < 12 % | < 18 % | Gewicht sofort beim Kontakt abgeworfen — mechanisch, nicht musikalisch |
| `LATE` ⚠️ | > 38 % | > 50 % | Gewicht nie vollständig übertragen — schwebend oder unvollständige Verlagerung (nur ADV) |

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

| Badge | Überschneidungsrate | Was es bedeutet | Trainingsimplikation |
| :--- | :--- | :--- | :--- |
| `OPTIMAL ROLL` ✅ | 18 %–38 % | Gleichmäßige, geerdte Gewichtsverlagerung | Die charakteristische WCS-Roll-Verbindung |
| `HECTIC` ⚠️ | < 18 % | Gehetzt — ein Fuß verlässt den Boden, bevor der andere sicher steht | „Abrollen, nicht abheben" — durch den Fuß rollen, bevor man schreitet |
| `SLUGGISH` ⚠️ | > 38 % | Verlängerter Doppelkontakt — Zögern oder schwere Stellung | Früher zum Massenschwerpunkt-Versatz bekennen |

Beobachte diese Karte während **Tripleschritten und Gängen**. `HECTIC` bei einem Ankerschritt bedeutet oft, dass du den Anker verlässt, bevor du Verbindung aufgebaut hast.

---

## 7. Die Roll-off-Symmetrie- und Gleichmäßigkeits-Karte

Nur auf **Experten**-Stufe sichtbar.

| Anzeige | Was es dir sagt | Grünes Ziel |
| :--- | :--- | :--- |
| **ASI %** | Unterschied zwischen linkem und rechtem Fuß-Roll-off | `SYMMETRIC` — unter 10 % |
| **Gleichmäßigkeit** | Flüssigkeit der Sprunggelenks-Artikulation über beide Füße | `SMOOTH` — 65 oder höher |

- Hoher **ASI** (z. B. `ASYMMETRIC` > 25 %) bedeutet meist, dass ein Sprunggelenk steifer ist oder eine Seite eine alte Verletzung kompensiert.
- Niedrige **Gleichmäßigkeit** bedeutet, dass deine Sprunggelenksbewegungen ruckartig sind. Verlangsame das Tempo und konzentriere dich darauf, durch den ganzen Fuß zu rollen statt flach aufzusetzen.

---

## 8. Die Beckenkarte (Optionaler Sensor)

Die Beckenkarte erscheint im **oberen linken Slot** des Dashboards, sobald der Beckensensor eingeschaltet ist. Wenn der Sensor offline ist, bleibt dieser Slot leer und die Kamera ist sichtbar.

### Sensor befestigen

Befestige den Sensor am **hinteren Hosenbund auf Höhe des unteren Rückens** (L5/Kreuzbeinniveau), Display von deinem Körper weg. Mittig an der Wirbelsäule ist ideal, aber einige Zentimeter links oder rechts der Mitte machen keinen praktischen Unterschied. Er sollte flach und fest sitzen — nicht herunterhängen.

### Befestigung überprüfen

Am oberen Rand der Beckenkarte zeigt eine kleine graue Datenzeile drei Live-Rohwerte:

```
aZ:+0.95  aX:-0.20  gY:   0
```

| Wert | Was er zeigt | Erwartet im Ruhezustand |
| :--- | :--- | :--- |
| `aZ` | Vertikale Beschleunigung | **+0,90 bis +1,00** (Schwerkraft) |
| `aX` | Laterale Beschleunigung | −0,30 bis +0,30 (kleine Neigungsabweichung ist normal) |
| `gY` | Hüft-Gierrate (°/s) | Nahe **0** |

Wenn `aZ` weit von +0,95 entfernt ist (z. B. nahe 0 oder negativ), ist der Sensor nicht korrekt befestigt — er könnte gedreht sein oder in die falsche Richtung zeigen. Neu befestigen: flach am Rücken, Display nach außen.

### Badge-Übersicht

| Badge-Reihe | Stufe | Was gemessen wird |
| :--- | :--- | :--- |
| **Hüftaktivierung** | BEG+ | Wie stark sich das Becken während der Bewegung dreht (Gieren) |
| **Laterale Stabilität** | INT+ | Seitliches Schwingen des Beckens während der Bewegung |
| **Hüft-Fuß-Kopplung** | INT+ | Ob die Hüften jeden Schritt initiieren oder den Füßen folgen |
| **Vertikales Auf-und-Ab** | INT+ | Wie viel vertikale Bewegung das Becken erzeugt |
| **Anchor Settle** | ADV | Qualität der Beckensetzung in den 500 ms nach jedem Ankerschritt |

---

### Hüftaktivierung

Misst die maximale transversale Hüftrotation (Gierrate) über ein rollendes 500-ms-Fenster.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `🌀 ACTIVE` ✅ | Starke Hüftrotation trägt zur Bewegung bei | Beibehalten |
| `MODERATE` ⚠️ | Etwas Rotation, aber Hüftbeitrag ist begrenzt | Hüfte vor jedem Schritt aufwickeln — Rotation sollte im Becken beginnen, nicht im Sprunggelenk |
| `STIFF HIPS` ❌ | Becken dreht sich kaum — Bewegung nur durch Beine angetrieben | Zuerst isolierte Hüftrotationen üben, dann Füße hinzufügen. „Mit der Hüfte führen, nicht mit der Ferse." |

> Im WCS sollte Hüftrotation jeden Schritt begleiten oder ihm vorausgehen. `STIFF HIPS` ist eines der häufigsten Anfängermuster und für Fußsensoren allein unsichtbar.

---

### Laterale Stabilität (INT+)

Misst die laterale Beschleunigungsvarianz des Beckens über 1 Sekunde.

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
| `HIP LEADS` ✅ | Maximale Hüftrotation mehr als 100 ms vor dem Fußkontakt | Gute Initiierung — Hüften treiben den Schritt an |
| `IN SYNC` ⚠️ | Hüftmaximum und Fußkontakt innerhalb von 40–100 ms voneinander | Akzeptabel — versuche den „Abschuss" der Hüfte vor dem Schritt zu verstärken |
| `HIP LAGS` ❌ | Hüften rotieren beim oder nach dem Fußkontakt | Beine bewegen sich unabhängig vom Rumpf. Verlangsamen und jeden Gang von der Hüfte aus initiieren üben, dabei den Fuß folgen lassen |

---

### Vertikales Auf-und-Ab (INT+)

Misst die Varianz der vertikalen Beckenbeschleunigung (Schwerkraft entfernt) über 1 Sekunde.

| Badge | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- |
| `GROUNDED` ✅ | Minimale vertikale Bewegung | Gut |
| `SLIGHT BOUNCE` ⚠️ | Etwas vertikale Schwingung | Leichte Kniebeugung durchgehend beibehalten; Knie beim Vorwärtsgehen nicht ganz durchstrecken |
| `BOUNCY` ❌ | Deutliche Auf-und-Ab-Bewegung | In den Knien bleiben. Denke: „Tief bleiben, verbunden bleiben." |

---

### Anchor Settle (ADV)

Nach jedem Rückwärts-(Anker-)Schritt öffnet das System ein 500-ms-Messfenster und wertet drei Signale aus:

1. **Verzögerung** — hat sich das Becken in der anterior-posterioren Richtung verlangsamt?
2. **Gier-Dämpfung** — hat die Hüftrotation nach dem Schritt abgenommen?
3. **Stabilität** — wie ruhig war das Becken in der zweiten Hälfte des Fensters?

Diese drei Komponenten werden zu einem Wert von 0–100 zusammengefasst, der im Badge angezeigt wird.

| Badge | Wert | Was es bedeutet | Wie man es verbessert |
| :--- | :--- | :--- | :--- |
| `ANCHORED (n)` ✅ | ≥ 60 | Starke Verzögerung + Gier-Dämpfung + stabile Haltung | Gut — an Konsistenz bei jedem Ankerschritt arbeiten |
| `SETTLING (n)` ⚠️ | 30–59 | Teilweise Setzung — eine oder zwei Komponenten schwach | Schwache Komponente mit den untenstehenden Tipps identifizieren |
| `UNSTABLE (n)` ❌ | < 30 | Becken bewegt sich oder wackelt noch nach dem Anker | „Anker festsetzen" — das Ende des Slots erreichen und halten |

**Niedrigen Wert diagnostizieren:**
- **Niedriger Wert trotz sauberer Fußtechnik** → das Problem liegt im Becken, nicht im Fußwinkel. An der Setzung selbst arbeiten, nicht am Schritt.
- **`HECTIC`-Doppelstand + niedriger Anchor-Settle** → du verlässt den Anker, bevor sich das Becken stabilisiert hat.
- **`🌀 ACTIVE`-Hüfte + `UNSTABLE`-Anker** → Hüften rotieren gut während der Bewegung, dämpfen aber am Anker nicht ab. Einen bewussten „sanften Stopp" üben.

---

## 9. Training nach Fertigkeitsstufe

### Anfänger — `👤 BEG` verwenden

**Ein Fokus: Fersen- vs. Zehenkontakt.**

1. Vorwärts gehen. Zeigt das Badge `OPTIMAL HEEL`? Wenn nicht — Ferse vor dem Aufsetzen etwas stärker anheben.
2. Rückwärts gehen. Zeigt das Badge `OPTIMAL TOE`? Wenn nicht — Zehe zuerst hinausschicken. Wenn du bei Rückwärtsschritten ↔️ FLAT + `FLAT-FOOT!` siehst, kontaktiert deine Ferse den Boden vor der Zehe.
3. Wenn du den **1200-Hz-Piep** hörst, anhalten und verlangsamen. Das ist ein harter `FLAT-FOOT!`-Aufprall.
4. Bei langsamem Tempo üben, bis `OPTIMAL HEEL` und `OPTIMAL TOE` konsistent erscheinen. Erst dann Tempo erhöhen.

**Mit Beckensensor:** Nur **Hüftaktivierung** beobachten. Wenn `STIFF HIPS` konstant erscheint, bewegen sich deine Beine ohne Rumpfbeteiligung.

### Fortgeschritten — `🏃 INT` verwenden

**Zwei Fokuspunkte: Technik-Konsistenz + Gewichtsübertragungs-Timing.**

1. Vorwärtsgänge → auf konstantes `OPTIMAL HEEL` mit Jerk-Leiste unter der Hälfte achten.
2. Rückwärtsgänge → auf `OPTIMAL TOE` achten. ↔️ FLAT bei einem Rückwärtsschritt bedeutet, dass der Fuß zu flach landet.
3. Das **POWER PUSH-Badge** beobachten: ist dein hinteres Bein passiv?
4. Die **Doppelstand-Karte** einführen: bei Tripleschritten auf `OPTIMAL ROLL` hinarbeiten.
5. Das neue **DELAY-Badge** beobachten: bei Ankerschritten auf `DELAYED ✓` achten. Konstantes `QUICK` bedeutet, dass du das Gewicht sofort abwirfst — kein musikalischer Atemzug in der Verbindung.

**Mit Beckensensor:** **Laterale Stabilität**, **Hüft-Fuß-Kopplung** und **Vertikales Auf-und-Ab** hinzufügen. Die wertvollste Einzelmetrik auf dieser Stufe ist die Hüft-Fuß-Kopplung — konstantes `HIP LAGS` bedeutet, dass du mit deinen Füßen gehst, nicht mit deinem Körper.

### Experte — `⭐ ADV` verwenden

**Vollständige biomechanische Feedback-Schleife.**

1. **SMOOTH LOAD vs. INSTANT LOAD** verwenden, um den Gewichtsempfang zu verfeinern — besonders bei synkopierten Mustern.
2. **DELAY-Badge** mit **SMOOTH LOAD** kreuzlesen: `DELAYED ✓` + `SMOOTH LOAD` ist die ideale Kombination — sowohl Timing als auch Rampenqualität stimmen. `DELAYED ✓` + `INSTANT LOAD` bedeutet, du hast gewartet, dann aber abgeworfen; `QUICK` + `SMOOTH LOAD` bedeutet, der Gradient war gut, aber das Schweben zu kurz.
3. **ASI** zwischen links und rechts über eine vollständige Trainingssitzung vergleichen. Eine konstant schlechtere Seite weist auf ein Kompensationsmuster hin.
4. **ANKLE FLEX vs. STIFF ANKLE** zur Ermüdungsüberwachung nutzen — Sprunggelenkssteifigkeit nimmt bei Muskelermüdung zu.
5. Mit `📷 CAM` aufnehmen und während der Pausen abspielen.
5. Das **Roll-off-Dynamik-Diagramm** verwenden, um Gyrospitzenwerte zwischen den Füßen zu vergleichen.

**Mit Beckensensor:** **Anchor Settle** als Anker-Qualitäts-KPI verwenden. Einen vollständigen 8-Zähler-Basic ausführen und den Wert nach jedem Ankerschritt prüfen.

---

## 10. Häufige Probleme und Lösungen

| Was du siehst | Ursache | Lösung |
| :--- | :--- | :--- |
| `FLAT-FOOT!` bei jedem Vorwärtsgang | Sprunggelenk starr gehalten; keine Fersenartikulierung | Verlangsamen. Bewusst übertriebenen Fersen-zuerst-Kontakt üben. |
| `FLAT-FOOT!` + ↔️ FLAT bei Rückwärtsschritten | Ferse kontaktiert vor Zehe | Zehe zuerst schicken, Sprunggelenk dorsalflektiert halten, bis der Fuß aufsetzt. |
| `HECTIC`-Doppelstand | Verlagerung gehetzt; Fuß hebt zu früh ab | „Als Letztes den Boden verlassen" — den ganzen Fuß von der Zehe abrollen lassen. |
| `SLUGGISH`-Doppelstand | Zögern vor dem Gewichtsbekenntnis | Dem Boden vertrauen. Den Körper bewegen, nicht nur den Fuß. |
| `STIFF ANKLE` konstant | Sprunggelenk beim Aufsetzen gespannt | Sich vorstellen, auf einem Schwamm zu landen. Sprunggelenk bewusst entspannen vor dem Kontakt. |
| `ASYMMETRIC`-ASI | Ein Fuß steifer oder weniger artikuliert | Betroffenen Fuß identifizieren und isoliert üben. |
| `— PUSH-OFF` (kein Badge) | Hinterbein passiv | „Boden drücken, nicht nur Fuß heben." |
| `INSTANT LOAD` bei Ankern | Gewicht abrupt abgeworfen | „In den Anker schmelzen" statt „darauf landen". |
| `QUICK` bei Ankerschritten | Kein Schweben vor Gewichtsbekenntnis | „Zurücktreten, atmen, dann setzen." Bewusste Pause zwischen Fußkontakt und Gewichtsankunft einplanen. |
| `LATE` bei Vorwärtsgängen | Gewicht kommt nie vollständig an | Der Verlagerung vertrauen — vollständig übertragen, bevor der nächste Schritt eingeleitet wird. |
| `STIFF HIPS` konstant | Beine ohne Rumpfbeteiligung | Jeden Schritt mit einem bewussten Hüftrotationsimpuls beginnen, bevor der Fuß sich bewegt. |
| `LATERAL SWAY` durchgehend | Hüftheben oder seitlicher Beckendruck | Becken waagerecht halten; auf asymmetrische Gewichtsverteilung achten. |
| `HIP LAGS` bei jedem Schritt | Beine und Rumpf entkoppelt | Sehr langsames Tempo. Hüftrotation initiieren, dann Fuß folgen lassen. |
| `BOUNCY` durchgehend | Kniestreckung während der Bewegung | Leichte Kniebeugung durchgehend beibehalten. Die Kopfhöhe sollte sich zwischen den Schritten nicht verändern. |
| `UNSTABLE`-Anker immer | Becken dreht sich noch nach dem Anker | Zurücktreten, beide Füße setzen und bewusst alle Hüftbewegung stoppen. Zwei Zählzeiten halten. |

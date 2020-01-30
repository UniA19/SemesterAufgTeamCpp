# Schiffe versenken / Battleship Anleitung

zuerst mit 'cd' in den richtigen Ordner wechseln

## Compilierung:
(*print.c*, *scan.c*, *bot.c*, *battleship.c* sind die C-Dateien, die für das Spiel notwendig sind, wobei *battleship.c* das Hauptprogram ist)  
**Entweder:**

 - `gcc -Wall -Wextra -ansi -pedantic print.c scan.c bot.c battleship.c -o battleship`

**oder:**

 - `gcc -Wall -Wextra -ansi -pedantic -c print.c`
 - `gcc -Wall -Wextra -ansi -pedantic -c scan.c`
 - `gcc -Wall -Wextra -ansi -pedantic -c bot.c`
 - `gcc -Wall -Wextra -ansi -pedantic print.o scan.o bot.o battleship.c -o battleship`

## Ausführung:
`./battleship -h` um die Liste der möglichen Optionen/Flags des Programms zu erhalten:
### Optionen/Flags des Programms:
 '`-a`'&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **Automatisches** setzen der Schiffe des Spielers aktivieren  
 '`-c`'&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Felder in **Farbe** ausdrucken (Nicht von allen Konsolen unterstützt)  
 '`-u`'&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Felder in **UTF-8** Zeichen ausdrucken (Nicht von allen Konsolen unterstützt)  
 '`-d=<n>`' prozentuale **Trefferquote** des Computers auf `<n>` einstellen  
 '`-s=<array>`' setzt die **Anzahl an Schiffen**, deren Länge gleich der Position von `<n>` in `<array>` ist auf `<n>`  
&nbsp; &nbsp; &nbsp; &nbsp; `<array> = [ "{" | "[" | "(" ] , [ <n> , { "," ,  <n> } ] , [ "}" | "]" | ")" ] ;`  
&nbsp; &nbsp; &nbsp; &nbsp; z.B. '`-s={3,2,1}`' heißt es gibt **3** Schiffe der Länge **1**, **2** Schiffe der Länge **2** und **1** Schiffe der Länge **3** und sonst keine  
&nbsp; &nbsp; &nbsp; &nbsp; **Achtung**: beim eintippen von Klammer muss man manchmal Anführungszeichen drum herum setzen, um eine Interpretaion von Shell zu vermeiden  
 '`-s[<length>]=<n>` setzt die **Anzahl an Schiffen** der Länge `<length>` auf `<n>`  
&nbsp; &nbsp; &nbsp; &nbsp; z.B. '-s[3]={2}' heißt es gibt **2** Schiffe der Länge **3** und die Anzahl an Schiffen der anderen Längen bleibt gleich  
&nbsp; &nbsp; &nbsp; &nbsp; **Achtung**: beim eintippen von Klammer muss man manchmal Anführungszeichen drum herum setzen, um eine Interpretaion von Shell zu vermeiden  
 '`-n=<n>`' **Höhe und Breite** des Feldes auf `<n>` einstellen  
 '`-x=<n>`' **Breite** des Feldes auf `<n>` einstellen  
 '`-y=<n>`' **Höhe** des Feldes auf `<n>` einstellen  
 '`-v`'&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Felder **übereinander** ausdrucken
### Allgemeine Hinweise:
Falls man beim Ausführen nicht weiß, wie man die Koordinaten eintippt oder man nicht weiß, welche Zeichen welche Schiffe sind, muss man '`-h`' eingeben, um eine ausführliche Beschreibung zu erhalten  
Um das Programm abzubrechen, einfach '`/`' eintippen
### Schiffe setzen:
Falls man '`-a`' nicht aktiviert hat, muss man sich erst seine Schiffe eigenstädig setzen, indem man:

 - zuerst die **Koordinate des Verankerungspunktes** des Schiffes bestehend aus der **X-Koordinate (_Buchstaben_)** und der **Y-Koordinate (_Zahlen_)** eintippt: z.B. '`A1`'
 - dann nach einem _Leerzeichen_ die **Länge/Richtung** in Form einer **Zahl** (Schiffslänge) und einem **Zeichen** zur Vorgabe der Richtung in der das Schiff gesetzt werden soll (N, O, S, W)
    - z.B. '`3N`': Schiff der Länge 3 soll von dem zuvor angegebenen Verankerungspunkt aus nach oben / norden gesetzt werden

### Schießen:
Zum Schießen die Koordinate bestehend aus der **X-Koordinate (_Buchstaben_)** und der **Y-Koordinate (_Zahlen_)** eintippen: z.B. '`A1`'

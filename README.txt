Schiffe versenken / Battleship Anleitung

zuerst mit 'cd' in den richtigen Ordner wechseln

Compilierung:
  (print.c, scan.c, bot.c, battleship.c sind die C-Dateien, die für das Spiel notwendig sind, wobei battleship.c das Hauptprogram ist)
    Entweder:
        'gcc -Wall -Wextra -ansi -pedantic print.c scan.c bot.c battleship.c -o battleship'
    oder:
        'gcc -Wall -Wextra -ansi -pedantic -c print.c'
        'gcc -Wall -Wextra -ansi -pedantic -c scan.c'
        'gcc -Wall -Wextra -ansi -pedantic -c bot.c'
        'gcc -Wall -Wextra -ansi -pedantic print.o scan.o bot.o battleship.c -o battleship'

Ausführung:

    './battleship -h' um die Liste der möglichen Optionen/Flags des Programms zu erhalten:
Optionen/Flags des Programms:
 -a     Automatisches setzen der Schiffe des Spielers aktivieren
 -c     Felder in Farbe ausdrucken (Nicht von allen Konsolen unterstützt)
 -u     Felder in UTF-8 Zeichen ausdrucken (Nicht von allen Konsolen unterstützt)
 -d=<n> prozentuale Trefferquote des Computers auf <n> einstellen
 -s=<array> setzt die Anzahl an Schiffen, deren Länge gleich der Position von <n> in <array> ist auf <n>
    <array> = [ "{" | "[" | "(" ] , [ <n> , { "," ,  <n> } ] , [ "}" | "]" | ")" ] ;
    z.B. '-s={3,2,1}' heißt es gibt 3 Schiffe der Länge 1, 2 Schiffe der Länge 2 und 1 Schiffe der Länge 3 und sonst keine
 -s[<length>]=<n> setzt die Anzahl an Schiffen der Länge <length> auf <n>
    z.B. '-s[3]={2}' heißt es gibt 2 Schiffe der Länge 3 und die Anzahl an Schiffen der anderen Längen bleibt gleich
 -n=<n> Höhe und Breite des Feldes auf <n> einstellen
 -x=<n> Breite des Feldes auf <n> einstellen
 -y=<n> Höhe des Feldes auf <n> einstellen
 -v     Felder übereinander ausdrucken


Falls man beim Ausführen nicht weiß, wie man die Koordinaten eintippt oder man nicht weiß, welche Zeichen welche Schiffe sind, muss man '-h' eingeben, um eine ausführliche Beschreibung zu erhalten
Um das Programm abzubrechen, einfach '/' eintippen

Falls man '-a' nicht aktiviert hat, muss man sich erst seine Schiffe eigenstädig setzen, indem man
 - zuerst die Koordinate des Verankerungspunktes des Schiffes bestehend aus der x-Koordinate (Buchstaben) und der Y-Koordinate (Zahlen) eintippt: z.B. 'A1'
 - dann nach einem Leerzeichen die Länge/Richtung in Form einer Zahl (Schiffslänge) und einem Zeichen zur Vorgabe der Richtung in der das Schiff gesetzt werden soll (N, O, S, W)
        z.B. '3N': Schiff der Länge 3 soll von dem zuvor angegebenen Verankerungspunkt aus nach oben / norden gesetzt werden

Zum Schießen die Koordinate bestehend aus der x-Koordinate (Buchstaben) und der Y-Koordinate (Zahlen) eintippen: z.B. 'A1'

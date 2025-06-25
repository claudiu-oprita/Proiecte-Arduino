 Sistem Automat de Parcare cu Barieră – Arduino
Acest proiect reprezintă un sistem automat de control al accesului într-o parcare, utilizând senzori, un servo pentru barieră, LED-uri de semnalizare și un display LCD pentru afișarea locurilor disponibile.

*Componente necesare:
1 x Placă Arduino (ex: Uno)
1 x Servo motor
1 x LCD 16x2 (cu pinii conectați la: 7, 6, 5, 4, 3, 2)
3 x LED-uri: verde, roșu, galben
2 x Senzori infraroșu (sau butoane): unul pentru intrare, unul pentru ieșire
Rezistențe (pentru LED-uri și senzori)
Cabluri și placă de test (breadboard)

*Conexiuni hardware:
Componentă	Pin Arduino
LED Verde	13
LED Roșu	12
LED Galben	11
Senzor Intrare	10
Servo Motor	9
Senzor Ieșire	8
LCD	7, 6, 5, 4, 3, 2

Funcționalitate:
*Intrare în parcare:
Dacă este detectat un vehicul la senzorul de intrare și există locuri disponibile, bariera se ridică.
LED-ul verde se aprinde, se incrementează numărul de locuri ocupate.
După un scurt delay, bariera se închide, LED-ul roșu se aprinde.
*Ieșire din parcare:
Dacă este detectat un vehicul la senzorul de ieșire și sunt mașini în parcare, bariera se ridică.
Numărul de locuri ocupate se decrementează, apoi bariera se închide.

*Afișaj LCD:
Arată numărul de locuri libere.
Dacă parcarea este plină, apare mesajul: Parcare plina și Reveniti!.
*LED-uri:
Verde: acces permis.
Roșu: acces interzis.
Galben: aprins când parcarea este plină.

*Setări importante:
maxLocuri = 5: Numărul maxim de locuri disponibile în parcare.
timpDebounce = 1500: Prevenirea declanșării duble a senzorilor (1.5 secunde).

*Structura codului:
setup(): Inițializează componentele (LED-uri, senzori, LCD, servo).
loop(): Monitorizează senzorii și gestionează logica de intrare/ieșire.
ridicaBariera() și coboaraBariera(): Controlează poziția servomotorului.
actualizeazaLCD(): Actualizează textul afișat pe LCD în funcție de locurile disponibile.
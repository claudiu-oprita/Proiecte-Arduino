# Control Avansat Motor Pas-cu-Pas cu A4988

Acest proiect Arduino extinde controlul motorului pas-cu-pas adăugând reglajul vitezei prin două butoane suplimentare. Se pot controla:
- direcția de rotație
- starea motorului (pornit/oprit)
- viteza motorului (mai rapid / mai lent)

#Componente necesare
- Placă Arduino (ex: Uno, Nano)
- Driver A4988
- Motor pas-cu-pas (ex: NEMA 17)
- 4 butoane tactile
- Rezistențe de pull-down/pull-up (dacă nu se folosesc interne)
- Alimentare externă pentru motor
- Fire de conectare

#Conexiuni
| Pin Arduino | Funcție                      |
|-------------|-------------------------------|
| D5          | STEP (semnal pași)            |
| D4          | DIR (direcție)                |
| D6          | ENABLE (activare driver)      |
| D9          | Buton schimbare direcție      |
| D12         | Buton ON/OFF motor            |
| D2          | Buton creștere viteză (↑)     |
| D3          | Buton scădere viteză (↓)      |

#Funcționalitate
- **Buton D9**: schimbă direcția de rotație.
- **Buton D12**: pornește sau oprește motorul.
- **Buton D2**: crește viteza motorului (delay mai mic).
- **Buton D3**: scade viteza motorului (delay mai mare).
- Motorul funcționează doar când este activ (`motorOn = true`).

#Specificații Viteză
- Viteza este controlată prin `stepDelay` (în microsecunde).
- Valorile posibile sunt între **500 µs (viteză maximă)** și **10000 µs (viteză minimă)**.
- Modificările de viteză sunt făcute în pași de 500 µs.

#Alte detalii
- Fiecare buton are implementat un sistem de debounce (întârziere anti-zgomot de 50ms).
- Poți ajusta valorile pentru viteza minimă/maximă modificând limitele `stepDelay` în cod.

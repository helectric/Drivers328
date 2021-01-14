Ez a könyvtár alapvető kommunikációs periféria drivereket fog össze ATMega328P mikrokontrollerhez.
Egyelőre az UART és az I2C (master) periféria van implementálva, valamit egy ettől elkülönülő gyűrűbuffer.
A driverek kipróbálását egy kis arudino mintapélda segíti, ez azonban elhagyható, a kódok megállnak az arduino keretrendszer nékül is AVR-GCC-vel fordítva.
A példakódok szintén nem használják ki az Arduino keretrendszer függvényeit.

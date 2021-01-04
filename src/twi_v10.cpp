// ATMega328P I2C kezelese gyuru bufferrel
// 2020.12.27 Hadobas Janos
// V1.0
// Buffer overflow nincs kezelve!!!

#include "twi_v10.h"

uint8_t twi_t_buffer[TWI_TX_BUFFER_SIZE];
uint8_t twi_r_buffer[TWI_RX_BUFFER_SIZE];

//Interrupt vezerelt kodhoz bufferek, transcriver
volatile CircularBuffer TWI_TXbuff(twi_t_buffer, TWI_TX_BUFFER_SIZE);

//receiver
volatile CircularBuffer TWI_RXbuff(twi_r_buffer, TWI_RX_BUFFER_SIZE);

 //Globalis valtozok az I2C kezelesehez
volatile bool twi_error = false;           //I2C hiba jelzo flag
volatile bool twi_bussy = false;  //I2C foglaltsag jelzo flag
volatile bool twi_mode_flag = TW_WRITE;    //I2C master iras/olvasas mod valtas TW_WRTIE = 0, TW_READ = 1
uint8_t tw_address = 0x01;        //I2C altal kezelt cim, iras elott meg kell adni egy valosat
uint8_t NumberOfRead = 1;         //I2C-n beolvasni kivant byteok szama, olvasas elott atirando valtozo

inline void TWI_START(){
    TWCR |=  (1 << TWSTA) | (1 <<TWEN) | (1 << TWINT);  //I2C START KONDICIO, PERIFERIA ENGEDELYEZESE, INTERRUPT ENGEDELYEZES UTOLJARA!
    twi_bussy = true;                                  //I2C ALLAPOGEP FUTASANAK JELZESE
    return;
}

void TWI_INIT() {
//Default I2C allapotgep valtozok
  twi_mode_flag = TW_WRITE;    //I2C master iras/olvasas mod valtas TW_WRTIE = 0, TW_READ = 1
  twi_error = false;           //I2C hiba jelzo flag
  twi_bussy = false;  //I2C foglaltsag jelzo flag
  tw_address = 0x01;        //I2C altal kezelt cim, iras elott meg kell adni egy valosat
  NumberOfRead = 1;         //I2C-n beolvasni kivant byteok szama, olvasas elott atirando valtozo
  CircularBuffer TWI_TXbuff(twi_t_buffer, TWI_TX_BUFFER_SIZE);
  CircularBuffer TWI_RXbuff(twi_r_buffer, TWI_RX_BUFFER_SIZE);
  //Default vege, periferia beallitasa
  TWSR = 0;                             //TWI status register 0-zas, bellaitja 1-re az orajelosztot a tobbi bit meg csak olvashato
  //TWSR |=(1 << TWPS0) |(1 << TWPS1);  //Pelda az orajeloszto atalitatasara
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2; //I2C baudrate beallitasa
  TWCR |= (1 << TWIE) | (1 << TWEN);    // I2C interrupt engedelyezese, I2C engedelyezese, tobbszor is ismetelve van, de itt sem elhagyhato!
  sei();                                //Globalis interruptok engedelyezese
  return;
}

uint8_t TWI_TryWriteArr(const uint8_t* data_arr, uint8_t data_arr_size) {
  if (TWI_TXbuff.isEmpty() && (!twi_bussy)) {       //Teszteli, hogy az olvasas buffer ures-e es az I2C periferia allapotgepe eppen fut-e
    twi_mode_flag = TW_WRITE;                       //Iras mod beallitasa
    TWI_TXbuff.read(data_arr, data_arr_size);       //Beirja a kiirando adatot a TX bfferbe
    TWI_START();                                    //Elinditja az I2C allapotgepet start kondicioval, es beallitja a foglaltsagot
    return (0);                                     //Ha rendben lefutott 0-val ter vissza       
  }
  else {                    //Ha valamiert nem tudja meghivni az I2C allapotgepet vagy a buffer felulirasanakveszelye allna fent
    return (1);             //Terjen vissza 1-el
  }
}

//A tomb iras miatt kicsit feleslees, lehetne egy hosszu tombot irni e helyett
uint8_t TWI_TryWriteByte(uint8_t data) {
  __asm__ __volatile__ ("nop");                 //Csak ezzel mukodik ez egy assembly ures utasitas, valami fordito optimalizacios gubanc van...
  if (TWI_TXbuff.isEmpty() && (!twi_bussy)) {   //Teszteli, hogy az olvasas buffer ures-e es az I2C periferia allapotgepe eppen fut-e
    twi_mode_flag = TW_WRITE;                   //Iras mod beallitasa
    TWI_TXbuff.readByte(data);                  //Beirja a kiirando adatot a TX bfferbe
    TWI_START();                                //Elinditja az I2C allapotgepet start kondicioval, es beallitja a foglaltsagot
    return (0);                                 //Ha rendben lefutott 0-val ter vissza
  }
  else {                //Ha valamiert nem tudja meghivni az I2C allapotgepet vagy a buffer felulirasanakveszelye allna fent
    return (1);         //Terjen vissza 1-el
  }
}

uint8_t TWI_Read_Arr(uint8_t lengthOfPackage) {
  if (TWI_TXbuff.isEmpty() && (!twi_bussy))     //Teszteli, hogy az olvasas buffer ures-e es az I2C periferia allapotgepe eppen fut-e
  {
    twi_mode_flag = TW_READ;            //I2C master olvasas modba valtas
    NumberOfRead = lengthOfPackage;     //Beallitom az olvasni kivant byteok szamat
    TWCR |= (1 << TWEA);                //I2C acknowledge engedelyezes, amig at nem allitom mindenre ack-t kuld vissza
    TWI_START();
    //A fogadott adatok megjelennek a TWI_RXbuff bufferben, itt at lehetne masolni egy visszatero tombbe de itt egy kicsit feleslegesnek erzem.
    return (0);
  }
  else {                                //Ha valamiert nem tudja meghivni az I2C allapotgepet vagy a buffer felulirasanakveszelye allna fent
    return (1);                         //Terjen vissza 1-el
  }
}

//I2C kezeleseert felelos allapotgep, I2C interruptra fut le, I2C status register ertekeit teszteli
ISR(TWI_vect) {
  switch (TW_STATUS) {
    //================================ Start kondiciok ==================================
    case TW_START:                                 //Start kondicio eseten
      TWDR = ((tw_address << 1) + twi_mode_flag);  //Valassz mukodesi modot... es kuldj cimet
      TWCR &= ~(1 << TWSTA);                       //Start kondicio torlese kotelezo miutan kikuldte, egyebkent nem fog mukodni, folyamatosan ujra startot fog majd kuldeni...
      TWINT_CLEAR();                               //Periferia interrupt flag torlese, folyamat folytatasa
      break;
    case TW_REP_START:                             //Ismetelt Start kondicio eseten
      TWDR = ((tw_address << 1) + twi_mode_flag);  //Valassz mukodesi modot... es kuldj cimet
      TWCR &= ~(1 << TWSTA);                       //Start kondicio torlese kotelezo miutan kikuldte, egyebkent nem fog mukodni, folyamatosan ujra startot fog majd kuldeni...
      TWINT_CLEAR();                               //Periferia interrupt flag torlese, folyamat folytatasa
      break;
    //================================ Master transmitter ===============================
    case TW_MT_SLA_ACK:                            //Valaszolt a megcimzett slave, varja a parancsot
      if (!TWI_TXbuff.isEmpty()) {                 //Ha az ado buffer ures nincs mar mit adni, kuldj stoppot
        TWI_TXbuff.writeByte(&TWDR);               //Beirja a TX buffer kovetkezo elemet a kuldo regiszterbe
        TWINT_CLEAR();                             //Periferia interrupt flag torlese, folyamat folytatasa
      }
      else {                                       //Ures a TX buffer
        TWI_STOP();                                //Stop kuldese
        twi_bussy = false;                         //Foglaltsag szabadra
      }
      //Ide berakni a TWINT_CLEART picit optimálisabb lenne... stop osszevonast meg kell hozza torni
      break;
    case TW_MT_SLA_NACK:                           //Nem valaszolt a slave a cimre, valami problema van...
      TWI_STOP();                                  //Stop kuldes
      twi_error = true;                            //Hiba flag bebillentese
      twi_bussy = false;                           //Foglaltsag szabadra
      break;
    case TW_MT_DATA_ACK:                           //Megkapta a slave az adatot, es nyugtazta, orulunk... es eldontjuk hogyan tovabb...
      if (!TWI_TXbuff.isEmpty()) {                 //Kuld ki a kovetkezo byteot ha van mit adni.
        TWI_TXbuff.writeByte(&TWDR);
        TWINT_CLEAR();
      }
      else {                                       //Ha az ado buffer ures nincs mar mit adni, kuldj stoppot
        TWI_STOP();
        twi_bussy = false;
      }
      //Ide berakni a TWINT_CLEART picit optimálisabb lenne... stop osszevonast meg kell hozza torni
      break;
    case TW_MT_DATA_NACK: //NACK jott valami problema lehet... 
      TWI_STOP();
      twi_error = true;
      twi_bussy = false;
      break;
    case TW_MT_ARB_LOST:  //Egy masik master is van, akivel szemben vesztettunk... //MR_ARB_LOST ugyan ez a kod... 
      twi_error = true;   //Lehetne default is, picit gyorsabb lenne
      twi_bussy = false;
      break;
    //================================ Master receiver ===============================
    case TW_MR_SLA_ACK:         //Meghallotta a cimet a slave, es nyugtazta, kuldeni fogja az adatot...
      if (NumberOfRead == 1) {  //Ha csak egy byte-ot kell beolvasni, akkor az elso olvasott byte-ra mar NACK-ot kell valaszolni
        TWCR &= ~(1 << TWEA);   //ACK valasz bellitasa a mi oldalunkrol, ilyenkor altalaban a slave folytatja az adast
      }
      TWINT_CLEAR();  
      break;
    case TW_MR_SLA_NACK:        //Nem hallotta a slave a cimet, biztos elfoglalt, esetleg nem letezik
      TWI_STOP();
      twi_error = true;
      twi_bussy = false;
      break;
    case TW_MR_DATA_ACK:        //Adat jott es meg fog is jonni
      TWI_RXbuff.readByte(TWDR);
      //Megj: itt lehetne foglalkozni az RX buffer overflow-al
      if (TWI_RXbuff.dataLenght() == (NumberOfRead - 1)) {  //Ha az olvasasnal az utolso byte-ot olvassa be, akkor legkozelebb NACK-ot kell valaszolni
        TWCR &= ~(1 << TWEA);   //ACK valasz atallitasa NACK-ra
      }
      TWINT_CLEAR();
      break;
    case TW_MR_DATA_NACK:   //Utolso byte jott, kommunikacio lezarasa
      TWI_RXbuff.readByte(TWDR);
      TWI_STOP();
      twi_bussy = false;
      break;
    //============================ Default, minden ami lehetne de itt nem kezeljuk
    default:  //Valami mas rosszul mukododtt, egyeb allapotba kerultunk amit nem kezelunk ebben a kodban (pl megcimeztek slave-nek)
      twi_error = true;
      twi_bussy = false;
      break;
  }
  return;
}

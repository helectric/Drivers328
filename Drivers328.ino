#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "src/uart_v20.h"
#include "src/twi_v10.h"

#define EEPROM_address 0x57;

//Az alabbi ket kis EEPROM driver fv. mutatja az I2C header hasznalatat, az ott megitrt fuggvenyek nelkul csak az 
//interruptban futo allapotgepet figyelembeveve. Ez az ajanlott ut a driverek irasahoz.

//Hibakezeles az I2C-hez. Egyelore villogtat egy LED-et ha valami rosszul mukodott.
//Ez a hibakezeles nem teljeskoru, csak azt veszi eszre amit az I2C allapotgep is felfed!
void TWI_ErrorHandler(){
    if (twi_error) {
    PORTB ^= (1 << 5);
    _delay_ms(500);
  }
  return;
}

//AT24C32 EEPROM byte iro fv.
//Csak egy byte-ot irok be, mert adatlap szerint worst case 10ms mir vegez az irassal a memoria, nem lehet folyamatosan irni!
uint8_t I2C_EEPROM_WriteByte(uint16_t adress, uint8_t data) {
  if (TWI_TXbuff.isEmpty() && (!twi_bussy)) {
    twi_mode_flag = TW_WRITE;    //I2C iras modba allitasa
    TWI_TXbuff.readByte((adress << 8)); //Memoriacim elso felenek beirasa
    TWI_TXbuff.readByte(uint8_t(adress)); //Memoriacim masodik felenek beirasa
    TWI_TXbuff.readByte(data);      //Beirom az adatbyte-ot a memoria megadott cimere
    TWI_START();
    return (0);
  }
  else {
    return (1);
  }
}

//Pelda I2C olvasas kezelesere. Ez egy iras es egy olvasas egymas utani vegrehejtasa
//AT24C32 EEPROM byte olvaso fv. egyelore pollinggal kezelem, lehetne interrupttal is, de az nagyon specifikus alkalmazas lenne
//Elso parameter memoria kezdocim az olvasasnal, masodik parameter az olvasando byteok szama
uint8_t I2C_EEPROM_Read(uint16_t address, uint8_t lengthOfPackage) {
  if (TWI_TXbuff.isEmpty() && (!twi_bussy))
  {
    twi_mode_flag = TW_WRITE;  //I2C master iras modba valtas
    TWI_TXbuff.readByte(uint8_t(address << 8));  //Memoriacim elso felenek beirasa
    TWI_TXbuff.readByte(uint8_t(address));     //Memoriacim masodik felenek beirasa
    TWI_START();
    while (twi_bussy){};  //Varok amig az I2C foglalt...
    twi_mode_flag = TW_READ;   //I2C master olvasas modba valtas
    NumberOfRead = lengthOfPackage;   //Beallitom az olvasni kivant byteok szamat
    TWCR |= (1 << TWEA);    //I2C acknowledge engedelyezes, amig at nem allitom mindenre ack-t kuld vissza
    TWI_START();
    //A fogadott adatok megjelennek a TWI_RXbuff bufferben
    return (0);
  }
  else {
    return (1);
  }
}

int main() {
  DDRB |= (1 << 5);  //LED pin kimenet
  PORTB |= (1 << 5); //LED pin HIGH
  USART_Init();   //USART periferia beallitasa
  TWI_INIT();     //I2C periferia beallitasa
  char hello[6] = "Hello";    //Karaktertmb kuldes tesztelese
  USART_Transmit_char_array(hello, 6);    //String kuldes tesztelese
  USART_Transmit_string(" Szia ez egy string!\n");
  //USART_Transmit_byte('A'); //Byte kuldo fv tesztelese
  tw_address = EEPROM_address;  //EEPROM cime, innentol az I2C ezzel a cimmel fog beszelni
  for (uint16_t i = 0; i < 10; ++i) { //EEPROM irasa 0x00 cimtol a kovetkezo 10 helyre
    I2C_EEPROM_WriteByte(i, (48 + i));  //Szamok beirasa ASCII-ben 0-9-ig
    //Adatlap alapjan worst case 10ms mire beir egy byte-ot az EEPROM
    _delay_ms(10); //Varakozas, hogy az EEPROM is beirja amit kell, az I2C foglaltsag veget is meg kene varni, de az EEPROM lasabb
  }
  I2C_EEPROM_Read(0x00, 10); //Tobb byte olvasasa I2C-n keresztul EEPROM-bol
  char tmp;
  while(twi_bussy);   //Meg kell varni, hogy az I2C vegezzen a beolvasassal, egyebkent nem lesz mit kiirni!
  while(!TWI_RXbuff.isEmpty()){   // Olvassd a vevo buffert amig van mit
    TWI_RXbuff.writeByte(&tmp);   // Ide jonnek I2C olvasaskor az adatok, ezt olvasom byteonkent
    USART_Transmit_byte(tmp);     // Kuldd ki UART-on az olvasott adatokat
  }
  while (1) {
    /* char tmp = USART_Receive_byte(); //Fogadas tesztelese
    if (tmp != 0) {     // Ha 0 volt a visszateres (nem 0 ASCII !!!)
      USART_Transmit_byte(tmp);   //Kuldd vissza a feladonak */
    TWI_ErrorHandler();
    }
    return(0);
  }

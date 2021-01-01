#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "src/uart_v20.h"
#include "src/twi_v10.h"

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
uint8_t I2C_EEPROM_ReadByte(uint16_t adress, uint8_t lengthOfPackage) {
  if (TWI_TXbuff.isEmpty() && (!twi_bussy))
  {
    twi_mode_flag = TW_WRITE;  //I2C master iras modba valtas
    TWI_TXbuff.readByte(uint8_t(adress << 8));  //Memoriacim elso felenek beirasa
    TWI_TXbuff.readByte(uint8_t(adress));     //Memoriacim masodik felenek beirasa
    TWI_START();
    while (twi_bussy);  //Varok amig az I2C foglalt...
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
  char hello[6] = "Hello";    //Karaktertmb kuldes tesztelese
  USART_Transmit_char_array(hello, 6);    //String kuldes tesztelese
  USART_Transmit_string(" Szia ez egy string!\n");
  //USART_Transmit_byte('A'); //Byte kuldo fv tesztelese
  while (1) {
    char tmp = USART_Receive_byte(); //Fogadas tesztelese
    if (tmp != 0) {     // Ha 0 volt a visszateres (nem 0 ASCII !!!)
      USART_Transmit_byte(tmp);   //Kuldd vissza a feladonak
    }
  }
}

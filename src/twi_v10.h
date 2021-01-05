// ATMega328P I2C kezelese gyuru bufferrel
// 2020.12.27 Hadobas Janos
// V1.0
// Buffer overflow nincs kezelve!!!

#ifndef TWI_H
#define TWI_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "CircularBuffer.h"

#ifndef F_CPU
#define F_CPU  ((uint32_t)16000000)   //CPU orajelenek megadasa, ha meg eddig nem lett volna megadva
#endif  /* F_CPU */

#ifndef TWI_FREQ
#define TWI_FREQ 100000UL //I2C frekvencia definialasa
#endif /* TWI_FREQ */

//TWI interface start es stop conditions kuldese, interrupt beerkezes nyugtazasa, I2C ujraengedelyezes
#define TWI_STOP() (TWCR |= (1 << TWSTO) | (1 <<TWEN) | (1 << TWINT))   //I2C stop kondicio, periferia engedelyezese, interrupt engedelyezes UTOLJARA!
#define TWINT_CLEAR() (TWCR |=  (1 << TWEN) | (1 << TWINT))   //Periferia engedelyezese, interrupt engedelyezes UTOLJARA!

//Fogado es kuldo bufferek meretenek megadasa
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE 16
#endif /* TWI_RX_BUFFER_SIZE */
#ifndef TWI_TX_BUFFER_SIZE
#define TWI_TX_BUFFER_SIZE 16
#endif /* TWI_TX_BUFFER_SIZE */

//Interrupt vezerelt kodhoz bufferek, transcriver
extern volatile CircularBuffer TWI_TXbuff;

//receiver
extern volatile CircularBuffer TWI_RXbuff;

 //Globalis valtozok az I2C kezelesehez
extern volatile bool twi_error;           //I2C hiba jelzo flag
extern volatile bool twi_bussy;  //I2C foglaltsag jelzo flag, a volatile fontos, csak az interruptban modosul!
extern volatile bool twi_mode_flag;    //I2C master iras/olvasas mod valtas TW_WRTIE = 0, TW_READ = 1
extern uint8_t tw_address;        //I2C altal kezelt cim, iras elott meg kell adni egy valosat
extern volatile uint8_t NumberOfRead;         //I2C-n beolvasni kivant byteok szama, olvasas elott atirando valtozo

void TWI_START();          //I2C start condition kuldese
void TWI_INIT();                  //I2C periferia alapbeallitasai

// Megjegyzes: ezek inkabb csak megvalositasi otletadoak egyeb eszkoz specifikus driverekhez
uint8_t TWI_TryWriteArr(const uint8_t* data_arr, uint8_t data_arr_size);        //Megprobal kiirni egy byte tombot (nem sikerul ha a periferia foglalt
uint8_t TWI_TryWriteByte(uint8_t data);                                         //Megprobal kiirni egy byteot (nem sikerul ha a periferia foglalt
uint8_t TWI_Read_Arr(uint8_t lengthOfPackage);                                  //Megbprobal beolvasni egy bytetombot

#endif /* TWI_H */

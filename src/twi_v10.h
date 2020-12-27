// ATMega328P I2C kezelese gyuru bufferrel
// 2020.12.28 Hadobas Janos
// V1.0
// Buffer overflow nincs kezelve!!!

#ifndef TWI_V10_H
#define TWI_V10_H

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
#define TWI_START() (TWCR |=  (1 << TWSTA) | (1 <<TWEN) | (1 << TWINT); twi_bussy = true)  //I2C start kondicio, periferia engedelyezese, interrupt engedelyezes UTOLJARA!, I2C allapogep futasanak jelzese
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
uint8_t twi_t_buffer[TWI_TX_BUFFER_SIZE];
CircularBuffer TWI_TXbuff(twi_t_buffer, TWI_TX_BUFFER_SIZE);

//receiver
uint8_t twi_r_buffer[TWI_RX_BUFFER_SIZE];
CircularBuffer TWI_RXbuff(twi_r_buffer, TWI_RX_BUFFER_SIZE);

 //Globalis valtozok az I2C kezelesehez
bool twi_mode_flag = TW_WRITE;    //I2C master iras/olvasas mod valtas TW_WRTIE = 0, TW_READ = 1
bool twi_error = false;           //I2C hiba jelzo flag
bool volatile twi_bussy = false;  //I2C foglaltsag jelzo flag
uint8_t tw_address = 0x01;        //I2C altal kezelt cim, iras elott meg kell adni egy valosat
uint8_t NumberOfRead = 1;         //I2C-n beolvasni kivant byteok szama, olvasas elott atirando valtozo

void TWI_INIT();                  //I2C periferia alapbeallitasai
// Megjegyzes: ezek inkabb csak megvalositasi otletadoak egyeb eszkoz specifikus driverekhez
uint8_t TWI_TryWriteArr(const uint8_t* data_arr, uint8_t data_arr_size);        //Megprobal kiirni egy byte tombot (nem sikerul ha a periferia foglalt
uint8_t TWI_TryWriteByte(byte data);                                            //Megprobal kiirni egy byteot (nem sikerul ha a periferia foglalt
uint8_t TWI_Read_Arr(uint8_t lengthOfPackage);                                  //Megbprobal beolvasni egy bytetombot

#endif /* TWI_V10_H */

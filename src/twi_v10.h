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
/*#ifndef twi_t_buffer
#define twi_t_buffer
extern uint8_t twi_t_buffer[TWI_TX_BUFFER_SIZE];
#endif /* twi_t_buffer */

//#ifndef TWI_TXbuff
//#define TWI_TXbuff
extern CircularBuffer TWI_TXbuff;
//#endif /* TWI_TXbuff */

//receiver
/*#ifndef twi_r_buffer
#define twi_r_buffer
extern uint8_t twi_r_buffer[TWI_RX_BUFFER_SIZE];
#endif /* twi_r_buffer */

//#ifndef TWI_RXbuff
//#define TWI_RXbuff
extern CircularBuffer TWI_RXbuff;
//#endif /* TWI_RXbuff */

 //Globalis valtozok az I2C kezelesehez

//#ifndef twi_mode_flag
//#define twi_mode_flag
extern bool twi_mode_flag;    //I2C master iras/olvasas mod valtas TW_WRTIE = 0, TW_READ = 1
//#endif

//#ifndef twi_error
//#define twi_error
extern bool twi_error;           //I2C hiba jelzo flag
//#endif

//#ifndef twi_bussy
// #define twi_bussy
extern bool volatile twi_bussy;  //I2C foglaltsag jelzo flag
// #endif

// #ifndef tw_address
// #define tw_address
extern uint8_t tw_address;        //I2C altal kezelt cim, iras elott meg kell adni egy valosat
// #endif

// #ifndef NumberOfRead
// #define NumberOfRead
extern uint8_t NumberOfRead;         //I2C-n beolvasni kivant byteok szama, olvasas elott atirando valtozo
// #endif

inline void TWI_START();
void TWI_INIT();                  //I2C periferia alapbeallitasai

// Megjegyzes: ezek inkabb csak megvalositasi otletadoak egyeb eszkoz specifikus driverekhez
uint8_t TWI_TryWriteArr(const uint8_t* data_arr, uint8_t data_arr_size);        //Megprobal kiirni egy byte tombot (nem sikerul ha a periferia foglalt
uint8_t TWI_TryWriteByte(uint8_t data);                                         //Megprobal kiirni egy byteot (nem sikerul ha a periferia foglalt
uint8_t TWI_Read_Arr(uint8_t lengthOfPackage);                                  //Megbprobal beolvasni egy bytetombot

#endif /* TWI_H */

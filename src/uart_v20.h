// ATMega328P UART kezelese gyuru bufferrel
// 2020.11.29 Hadobas Janos
// V2.0
// Buffer overflow nincs kezelve!!!

#ifndef UART_V20_H
#define UART_V20_H

#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "CircularBuffer.h"

#ifndef F_CPU
#define F_CPU  ((uint32_t)16000000)   //CPU orajelenek megadasa, ha meg eddig nem lett volna megadva
#endif  // F_CPU 

#ifndef BAUD
#define BAUD  ((uint16_t)9600)    //Bitrata amivel kikuldi az adatot
#endif //BAUD

#ifndef TX_BUFFER_SIZE
#define TX_BUFFER_SIZE 32   //Kuldo gyurubuffer merete
#endif

#ifndef RX_BUFFER_SIZE
#define RX_BUFFER_SIZE 32   //Fogado gyurubuffer merete
#endif

#define UBRRN_N ((uint32_t)F_CPU/16/BAUD-1) //Tipus deklaralasa fontos, a nelkul nem mukodik! Itt szamolom ki az UART belso orajelet beallito oszto erteket

void USART_Init();   //USART init a define-ban megadott baudrate-tel
void USART_Transmit_char_array(const char* ch_arr, unsigned short arr_size);  //Karakter tombot (bamilyen 8-bites byte tomb) kikuldese UART-on
void USART_Transmit_string(const char* string); //String kikuldese UART-on
void USART_Transmit_byte(char data);  //Egy bíte kikuldese UART-on
void USART_Tr_UInt16(uint16_t data);  //16 bites int kuldese UART-on ketes hatekonysagu...
uint8_t USART_Receive_byte(); //Egy byte fogadasa UART-on
uint8_t USART_Received();    //USART receive buffer uressegenek tesztelese
#endif /* UART_V20_H*/

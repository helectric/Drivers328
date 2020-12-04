#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "src/uart_v20.h"

int main() {
  DDRB |= (1 << 5);  //LED pin kimenet
  PORTB |= (1 << 5); //LED pin HIGH
  USART_Init();   //USART periferia beallitasa
  char hello[6] = "Hello";    //Karaktertomb kuldes tesztelese
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

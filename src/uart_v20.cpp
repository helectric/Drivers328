// ATMega328P UART kezelese gyuru bufferrel
// 2020.11.29 Hadobas Janos
// V2.0
// Buffer overflow nincs kezelve!!!

#include "uart_v20.h"

//Interrupt vezerelt kodhoz valtozok, transcriver
uint8_t tx_buffer[TX_BUFFER_SIZE];
CircularBuffer TXbuff(tx_buffer, TX_BUFFER_SIZE);

//receiver
uint8_t rx_buffer[RX_BUFFER_SIZE];
CircularBuffer RXbuff(rx_buffer, RX_BUFFER_SIZE);

// ========================================= Interrupt kezelesek =======================================
//ISR-t nem kell deklaralni a header fajlban mert nem hivja meg mas fv oket sosem.

//Interrupt kezles ha kiurult a tx kuldo buffer (1 byte)

ISR(USART_UDRE_vect) {      //Ha kiurult az USART transmit buffere
  uint8_t sreg = SREG;    //Statusz regiszter eltarolasa
  cli();    //Interruptok tiltasa
  if (!TXbuff.isEmpty()) {
    TXbuff.writeByte(&UDR0);
  }
  else {      //Ha egyezik az iras es az olvasas pointer, a buffer ures, nincs mar mit kiirni.
    UCSR0B &= ~(1 << UDRIE0); //Data Register Empty Interrupt Disable. Ha nincs mit beirni a UDR0 regiszterbe akkor kotelezo az interruptot tiltani, mert maskepp ciklikusan be fog lepni az interruptba
  }
  SREG = sreg;    //Status register visszaallitasa
  sei();  //Interruptok ujra engedelyezese
  return;
}

//Interrupt kezeles ha uj byte erkezett a USART periferian
ISR(USART_RX_vect) {
  uint8_t sreg = SREG;    //Statusz regiszter eltarolasa
  cli();    //Interruptok tiltasa
  RXbuff.readByte(UDR0);  //UDR0 regiszter erteket beolvassa a gyurubufferbe
  SREG = sreg;    //Status register visszaallitasa
  sei();  //Interruptok ujra engedelyezese
  return;
}

//======================================== UART kezelo fv.-ek ==========================================
//USART periferia beallitasa
void USART_Init() {
  //Baudrate beallitasa
  UBRR0H = (unsigned char)(UBRRN_N >> 8);
  UBRR0L = (unsigned char) UBRRN_N;
  //Kihasznalom a regiszterek inditaskori alapertekeit!
  //UART frame beallitasa 8 bites keret, paritasbit nelkul, 1 stop bit
  UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
  //reciver es transmitter beallitasa, RX interrupt engedelyezese
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
  sei();    //Globalis interrupt engedelyezes
}

void USART_Transmit_char_array(const char* ch_arr, unsigned short arr_size) {
  TXbuff.read(ch_arr, arr_size);
  UCSR0B |= (1 << UDRIE0); //Data Register Empty Interrupt Enable, Ha ures a TX bffer akkro interrupt erkezik, ennek a kezelese fogja kiuriteni a buffert
  return;
}

void USART_Transmit_string(const char* string) {  //const, mert a char* akar valtozhatna is
  unsigned short str_length = strlen(string);
  USART_Transmit_char_array(string, str_length);
}

void USART_Transmit_byte(char data) {
  TXbuff.readByte(data);
  UCSR0B |= (1 << UDRIE0); //Data Register Empty Interrupt Enable, Ha ures a TX bffer akkor interrupt erkezik, ennek a kezelese fogja kiuriteni a buffert
  return;
}

void USART_Tr_UInt16(uint16_t data) {
  USART_Transmit_byte(data >> 8);
  USART_Transmit_byte(data);
  return;
}

uint8_t USART_Receive_byte() {
  if (!RXbuff.isEmpty()) { //Ha nem ures a receiver buffer
    uint8_t tmp;
    RXbuff.writeByte(&tmp);
    return (tmp);   //Terj vissza tmp ertekevel
  }
  else {
    return (0);
  }
}

uint8_t USART_Received(){
  if(!RXbuff.isEmpty()){
    return(1);
  }
  else{
    return(0);
  }
}

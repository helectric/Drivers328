// Altalaos gyurubuffer kod
// Nincs kezelve a buffer overflow!!!
// Hadobas Janos
// 2020.12.26
// V1.1

//ToDo Inline fv nem mukodik pedig kontrollerre jo lenne !!!
#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(uint8_t* in_buffer, uint8_t in_buffer_size)  //Konstruktor
{
  buffer = in_buffer;   //A tombot nem biztos, hogy jo otlet atadni, lehet bent jobb lenne letrehozni a meret alapjan
  buffer_size = in_buffer_size;   //Buffer merete
  write_ptr = buffer;     //Iro pointer alaphelyzetben a tomb elejen
  read_ptr = buffer;    //Olvaso pointer alaphelyzetben a tomb elejen
}

void CircularBuffer::writeByte(uint8_t* volatile data)  // Gyurubufferbol kifele iras, egyelora ugy dontottem, hogy nem itt kezelem le azt, ha a bufferbol nincs mit kiirni, mert valahol nem elonyos
{
  // if (read_ptr != write_ptr)      //Ha nem egyezik az iras es az olvasa pointer (van meg nem kiolvasott elem a gyuruben) Ez a sor problemakat okoz egy esetleges overflow eseten
  //{
  *data = *read_ptr;        	//Ird ki a kovetkezo byteot az adott cimre
  if (read_ptr < &buffer[buffer_size - 1 ])      //Ha nem eri el a tomb veget az olvasas pointer
  {
    read_ptr++;      	//Adj hozza egyet az olvasas pointerhez
  }
  else                  	//Ha a tomb vegen van az olvasas pointer
  {
    read_ptr = buffer;  //Ugorj a tomb elejere
  }
  // }
  // A kovetkezo sor valoszinuleg nyer a futasidoben a deklaracioban megadott volatile-hoz kepes. (Ezt a generalt kod meretebol kovetkeztettem.)
  //__asm__ __volatile__ ("nop"); //Berak egy ures utasitas assemblyben. E nelkul a fordito kioptimalizalja ezt a fv.-t ha interrupt hivassal toltom fel.
  return;
}

void CircularBuffer::readByte(uint8_t volatile data)  //Gyurubufferbe beolvasas, egy byteot beemel a bufferbe. Tuliras nincs itt kezelve!
{
  *write_ptr = data;    //Legyen a gyurubuffer iro pointere altal mutatott ertek az UDR0 regiszter erteke
  if (write_ptr < &buffer[buffer_size - 1 ])   //Ha nem erte meg el az iro pointer az utolso memoriacimet a tombben
  {
    write_ptr++;     	//leptesd eggyel aziro pointert
  }
  else         				//Ha az iro pointer elerte a gyurubuffer utolso memoriacimet, ugorjon vissza az elsore (ez zarja gyurube)
  {
    write_ptr = buffer;   //Ugorja a tomb elejere, (gyuru bezarasa)
  }
  return;
}

bool CircularBuffer::isEmpty()    //Teszteli, hogy ures-e a gyurubuffer
{
  if (read_ptr == write_ptr)    //Ha az iro es az olvaso pointer ugyanoda mutat akkor a buffer ures
  {
    return true;
  }
  else                //Egyebkent a bufferben van meg kiolvasatlan elem
  {
    return false;
  }
  //Megmondja, hogy ures-e a gyurubuffer
}

uint8_t CircularBuffer::dataLenght()
{
  if(write_ptr > read_ptr){
    return(write_ptr - read_ptr);
  }
  else{
    return((write_ptr - &buffer[0]) + (&buffer[buffer_size] - read_ptr));
  }
}

void CircularBuffer::read(const uint8_t* volatile data_arr, uint8_t arr_size)  //Gyurubufferbe beolvasas, egy tombot tesz be a bufferbe. Tuliras nincs itt kezelve!
{
  for (uint8_t i = 0; i < arr_size; i++)     //masolas a gyuru bufferbe, ciklus amely vegigfut a bemeneti tomb elemein
  {
    *write_ptr = data_arr[i];    //Legyen az iro pointer altal mutatott ertek az i. karaktere a bemeneti tombnek
    if (write_ptr < &buffer[buffer_size - 1 ])   //Ha nem erte meg el az iro pointer az utolso memoriacimet a tombben
    {
      write_ptr++;     //leptesd eggyel a pointer
    }
    else          //Ha az iro pointer elerte a gyurubuffer utolso memoriacimet, ugorjon vissza az elsore (ez zarja gyurube)
    {
      write_ptr = buffer;   //Ugorja a tomb elejere, (gyuru bezarasa)
    }
  }
  return;
}

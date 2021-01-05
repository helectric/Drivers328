// Altalaos gyurubuffer kod
// Nincs kezelve a buffer overflow!!!
// Hadobas Janos
// 2020.12.26
// V1.1

//ToDo Inline fv nem mukodik pedig kontrollerre jo lenne !!!
#ifndef CircularBuffer_h
#define CircularBuffer_h

#include <stdint.h>

class CircularBuffer
{
  public:
    CircularBuffer(uint8_t* buffer, uint8_t buffer_size);	//Konstruktor
    void readByte(uint8_t data);			//Adatok bufferbe torteno beolvasasara szolgalo tagfv.
    void writeByte(uint8_t* data);		//Adatok bufferbol torteno kiirasara szolgalo tagfv.
    bool isEmpty();						//Teszteli ures-e a buffer
    void read(const uint8_t* data_arr, uint8_t arr_size);	// Tomb bufferbe torteno beirasara szolgalo tagfv.
    uint8_t dataLenght();  //Visszaadja a read es a write pointer kulonbseget (byteok szama)

  private:    //Belso valtozok
    uint8_t* buffer;
    uint8_t buffer_size;
    uint8_t* write_ptr;
    uint8_t* read_ptr;
};

#endif

#ifndef PINHANDLING_H_
#define PINHANDLING_H_


//----------------------------------------------------------------------------------------------------------------------------
// MACROS FOR EASY PIN HANDLING FOR ATMEL GCC-AVR
//these macros are used indirectly by other macros , mainly for string concatination
#define _SET(type,name,bit)          type ## name  |= _BV(bit)
#define _CLEAR(type,name,bit)        type ## name  &= ~ _BV(bit)
#define _TOGGLE(type,name,bit)       type ## name  ^= _BV(bit)
#define _GET(type,name,bit)          ((type ## name >> bit) &  1)
#define _PUT(type,name,bit,value)    type ## name = ( type ## name & ( ~ _BV(bit)) ) | ( ( 1 & (unsigned char)value ) << bit )

//these macros are used by end user
#define OUTPUT(pin)         _SET(DDR,pin)
#define INPUT(pin)          _CLEAR(DDR,pin)
#define HIGH(pin)           _SET(PORT,pin)
#define LOW(pin)            _CLEAR(PORT,pin)
#define TOGGLE(pin)         _TOGGLE(PORT,pin)
#define READ(pin)           _GET(PIN,pin)
//----------------------------------------------------------------------------------------------------------------------------


// pinos utilizados no Display
#define RS          B,7      // D13
#define E           B,6      // D12
#define D4          B,5      // D11
#define D5          B,4      // D10
#define D6          H,6      // D9
#define D7          H,5      // D8

// pinos utilizados no Teclado
#define LINHA1      F,0      // A0
#define LINHA2      F,1      // A1
#define LINHA3      F,2      // A2
#define LINHA4      F,3      // A3
#define COLUNA1     F,4      // A4
#define COLUNA2     F,5      // A5
#define COLUNA3     F,6      // A6
#define COLUNA4     F,7      // A7

// pinos utilizados no Buzzer e no LED
#define BUZZER      H,4      // D7
#define LED         H,3      // D6


#endif /* PINHANDLING_H_ */

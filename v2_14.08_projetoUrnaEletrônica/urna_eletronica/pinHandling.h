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
#define RS          D,6      // D6
#define E           D,7      // D7
#define D4          B,0      // D8
#define D5          B,1      // D9
#define D6          B,2      // D10
#define D7          B,3      // D11

// pinos utilizados no Teclado
#define LINHA1      D,5      // D5
#define LINHA2      D,4      // D4
#define LINHA3      D,3      // D3
#define LINHA4      D,2      // D2
#define COLUNA1     C,0      // A0(D14)
#define COLUNA2     C,1      // A1(D15)
#define COLUNA3     C,2      // A2(D16)
#define COLUNA4     C,3      // A3(D17)

// pinos utilizados no Buzzer e no LED
#define BUZZER      B,4      // D12
#define LED         B,5      // D13


#endif /* PINHANDLING_H_ */
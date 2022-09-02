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


// TERMOS DEFINIDOS
#define BLOQUEADA 1
#define OPERACIONAL 2
#define AGUARDANDO 3
#define ENCERRADA 4
#define OPTION_ESTADO 1
#define OPTION_ELEITOR 2
#define OPTION_CONSULTA_HORA 3
#define OPTION_TROCA_HORA 4
#define OPTION_VERIFICA_CORRESPONDENCIA 5
#define OPTION_RELATORIO_VOTACAO 6
#define OPTION_TROCA_SENHA 7
#define OPTION_RESET_SENHA 8
#define OPTION_JUSTIFICA 9
#define RELATORIO_ENVIADO 1

#define CANDIDATO_INEXISTENTE 0xff
#define ERRO_COMUNICACAO 0xfd
#define ELEITOR_VOTOU 2
#define ELEITOR_VALIDO 1
#define ELEITOR_INVALIDO 0
#define TEMPO_ESGOTADO 1
#define VOTACAO_CONCLUIDA 2

#define JUSTIFICATIVA_CONCLUIDA 1
#define JUSTIFICATIVA_NAO_CONCLUIDA 2

// Caracteres especiais LCD
#define DOWN_ARROW_CHARACTER 0x00
#define RIGHT_OPTION_CHARACTER 0x01
#define RETURN_CHARACTER 0x02
#define LOGIN_CHARACTER 0x03
#define UP_AND_DOWN_CARACTERE 0x04
#define LEFT_AND_RIGHT_CARACTERE 0x05
#define UPPERCASE_LOWERCASE_CARACTERE 0x06
#define LEFT_ARROW_CHARACTER 0x7F
#define RIGHT_ARROW_CHARACTER 0x7E



#endif /* PINHANDLING_H_ */
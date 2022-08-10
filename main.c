#include <avr/io.h>


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


// PORTB utilizado nos displays
#define D7			B,0      // PORTB.0
#define D6			B,1      // PORTB.1
#define D5			B,2      // PORTB.2
#define D4			B,3      // PORTB.3
#define E			B,4      // PORTB.4
#define RS			B,5      // PORTB.5

// PORTD utilizado no teclado
#define LINHA1		D,0      // PORTD.0
#define LINHA2      D,1      // PORTD.1
#define LINHA3      D,2      // PORTD.2
#define LINHA4      D,3      // PORTD.3
#define COLUNA1		D,4      // PORTD.4
#define COLUNA2     D,5      // PORTD.5
#define COLUNA3     D,6      // PORTD.6
#define COLUNA4     D,7      // PORTD.7


unsigned char debounce(char, char);
void functionSet();
void entryModeSet(char, char);
void displayOnOffControl(char, char, char);
char getBit(char, char);
void sendChar(char);
void setDdRamAddress(char);
void delay_50us();
void scan_linhas();
char get_tecla();

unsigned long int overflow_counter = 0;

int main(void)
{
	
	// Configuração inicial do Timer0
	TCCR0A = 0;
	TCCR0B = 1;
	
	// Display como output
	// Linhas como outputs e colunas como inputs
	OUTPUT(D4);
	OUTPUT(D5);
	OUTPUT(D6);
	OUTPUT(D7);
	OUTPUT(E);
	OUTPUT(RS);
	
	OUTPUT(LINHA1);
	OUTPUT(LINHA2);
	OUTPUT(LINHA3);
	OUTPUT(LINHA4);
	INPUT(COLUNA1);
	INPUT(COLUNA2);
	INPUT(COLUNA3);
	INPUT(COLUNA4);
	
	
	// Garante que todas as linhas comecem em nível lógico alto
	LOW(LINHA1);
	LOW(LINHA2);
	LOW(LINHA3);
	LOW(LINHA4);
	
	// Inicialização do display
	functionSet();
	entryModeSet(1, 0);
	displayOnOffControl(1, 1, 1);
	
	while (1)
	{
		char tecla_atual = get_tecla();
		
		if(tecla_atual != 0){
			setDdRamAddress(0x00);
			sendChar(tecla_atual);
		}

		scan_linhas();
	}
}

// Retorna o caractere lido
char get_tecla(){
	if(debounce(READ(LINHA1), !READ(COLUNA1))){ return '1';}
	else if(debounce(READ(LINHA1), !READ(COLUNA2))){ return '2';}
	else if(debounce(READ(LINHA1), !READ(COLUNA3))){ return '3';}
	else if(debounce(READ(LINHA1), !READ(COLUNA4))){ return 'A';}
	else if(debounce(READ(LINHA2), !READ(COLUNA1))){ return '4';}
	else if(debounce(READ(LINHA2), !READ(COLUNA2))){ return '5';}
	else if(debounce(READ(LINHA2), !READ(COLUNA3))){ return '6';}
	else if(debounce(READ(LINHA2), !READ(COLUNA4))){ return 'B';}
	else if(debounce(READ(LINHA3), !READ(COLUNA1))){ return '7';}
	else if(debounce(READ(LINHA3), !READ(COLUNA2))){ return '8';}
	else if(debounce(READ(LINHA3), !READ(COLUNA3))){ return '9';}
	else if(debounce(READ(LINHA3), !READ(COLUNA4))){ return 'C';}
	else if(debounce(READ(LINHA4), !READ(COLUNA1))){ return '*';}
	else if(debounce(READ(LINHA4), !READ(COLUNA2))){ return '0';}
	else if(debounce(READ(LINHA4), !READ(COLUNA3))){ return '#';}
	else if(debounce(READ(LINHA4), !READ(COLUNA4))){ return 'D';}
	return 0;
}

// Varre as linhas com o nível lógico baixo
void scan_linhas(){
	static char linha_atual = 1;
	
	if(linha_atual == 1){LOW(LINHA1); HIGH(LINHA4);}
	else if(linha_atual == 2){LOW(LINHA2); HIGH(LINHA1);}
	else if(linha_atual == 3){LOW(LINHA3); HIGH(LINHA2);}
	else if(linha_atual == 4){LOW(LINHA4); HIGH(LINHA3);}
	
	linha_atual++;
	if(linha_atual == 5){linha_atual = 1;}
}

// Realiza o debounce das teclas
unsigned char debounce(char linha, char coluna){
	unsigned char current_key = 0, last_key = 0, counter = 0;
	char const BOUNCE = 7;
	
	// Entra no loop apenas se a linha estiver selecionada
	if(!linha){
		
		// Enquanto contador for diferente de BOUNCE, itera até estabilzar
		while(counter != BOUNCE){
			delay_50us();
			delay_50us();
			current_key = coluna;
			
			if(last_key == current_key){
				counter++;
				} else{
				counter = 0;
			}
			
			last_key = current_key;
		}
	}
	
	return current_key;
}

// Delay de 50us para o display
// Considerando clock = 2 MHz
void delay_50us(void){
	TCNT0 = 156;
	while((TIFR0 & (1 << 0)) == 0);
	overflow_counter++;
}

// Conjunto de funções utilizadas para o Display LCD 16x2
void functionSet(void) {
	LOW(D7);
	LOW(D6);
	HIGH(D5);
	LOW(D4);
	LOW(RS);
	HIGH(E);
	LOW(E);
	delay_50us();
	HIGH(E);
	LOW(E);
	HIGH(D7);
	HIGH(E);
	LOW(E);
	delay_50us();
}
void entryModeSet(char id, char s) {
	LOW(RS);
	LOW(D7);
	LOW(D6);
	LOW(D5);
	LOW(D4);
	HIGH(E);
	LOW(E);
	HIGH(D6);
	if(id == 1){HIGH(D5);} else{LOW(D5);}
	if(s == 1){HIGH(D4);} else{LOW(D4);}
	HIGH(E);
	LOW(E);
	delay_50us();
}
void displayOnOffControl(char display, char cursor, char blinking) {
	LOW(D7);
	LOW(D6);
	LOW(D5);
	LOW(D4);
	HIGH(E);
	LOW(E);
	HIGH(D7);
	if(display == 1){HIGH(D6);} else{LOW(D6);}
	if(cursor == 1){HIGH(D5);} else{LOW(D5);}
	if(blinking == 1){HIGH(D4);} else{LOW(D4);}
	HIGH(E);
	LOW(E);
	delay_50us();
}
char getBit(char c, char bitNumber) {
	return (c >> bitNumber) & 1;
}
void sendChar(char c) {
	if(getBit(c, 7) == 1){HIGH(D7);} else{LOW(D7);}
	if(getBit(c, 6) == 1){HIGH(D6);} else{LOW(D6);}
	if(getBit(c, 5) == 1){HIGH(D5);} else{LOW(D5);}
	if(getBit(c, 4) == 1){HIGH(D4);} else{LOW(D4);}
	HIGH(RS);
	HIGH(E);
	LOW(E);
	if(getBit(c, 3) == 1){HIGH(D7);} else{LOW(D7);}
	if(getBit(c, 2) == 1){HIGH(D6);} else{LOW(D6);}
	if(getBit(c, 1) == 1){HIGH(D5);} else{LOW(D5);}
	if(getBit(c, 0) == 1){HIGH(D4);} else{LOW(D4);}
	HIGH(E);
	LOW(E);
	delay_50us();
}
void setDdRamAddress(char address) {
	LOW(RS);
	HIGH(D7);
	if(getBit(address, 6) == 1){HIGH(D6);} else{LOW(D6);}
	if(getBit(address, 5) == 1){HIGH(D5);} else{LOW(D5);}
	if(getBit(address, 4) == 1){HIGH(D4);} else{LOW(D4);}
	HIGH(E);
	LOW(E);
	if(getBit(address, 3) == 1){HIGH(D7);} else{LOW(D7);}
	if(getBit(address, 2) == 1){HIGH(D6);} else{LOW(D6);}
	if(getBit(address, 1) == 1){HIGH(D5);} else{LOW(D5);}
	if(getBit(address, 0) == 1){HIGH(D4);} else{LOW(D4);}
	HIGH(E);
	LOW(E);
	delay_50us();
}

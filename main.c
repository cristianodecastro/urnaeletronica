#include <avr/io.h>


#define PORTDisplay PORTB         // PORTB utilizado nos displays
#define DDRDisplay  DDRB
#define DB7			(1 << 0)      // PORTB.0
#define DB6			(1 << 1)      // PORTB.1
#define DB5			(1 << 2)      // PORTB.2
#define DB4			(1 << 3)      // PORTB.3
#define E			(1 << 4)      // PORTB.4
#define RS			(1 << 5)      // PORTB.5

#define PORTTeclado PORTD         // PORTD utilizado no teclado
#define DDRTeclado  DDRD
#define PINTeclado  PIND
#define LINHA1		(1 << 0)      // PORTD.0
#define LINHA2      (1 << 1)      // PORTD.1
#define LINHA3      (1 << 2)      // PORTD.2
#define LINHA4      (1 << 3)      // PORTD.3
#define COLUNA1		(1 << 4)      // PORTD.4
#define COLUNA2     (1 << 5)      // PORTD.5
#define COLUNA3     (1 << 6)      // PORTD.6
#define COLUNA4     (1 << 7)      // PORTD.7


unsigned char debounce(char, char);
void functionSet();
void entryModeSet(char, char);
void displayOnOffControl(char, char, char);
char getBit(char, char);
void sendChar(char);
void setDdRamAddress(char);
void delay50us();
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
	DDRDisplay |= DB7;	    DDRDisplay |= DB4;
	DDRDisplay |= DB6;      DDRDisplay |= E;
	DDRDisplay |= DB5;      DDRDisplay |= RS;
	
	DDRTeclado |= LINHA1;	DDRTeclado &= ~COLUNA1;
	DDRTeclado |= LINHA2;	DDRTeclado &= ~COLUNA2;
	DDRTeclado |= LINHA3;	DDRTeclado &= ~COLUNA3;
	DDRTeclado |= LINHA4;	DDRTeclado &= ~COLUNA4;
	
	
	// Garante que todas as linhas comecem em nível lógico alto
	PORTTeclado |= LINHA1;
	PORTTeclado |= LINHA2;
	PORTTeclado |= LINHA3;
	PORTTeclado |= LINHA4;
		
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
	if(debounce((PINTeclado & LINHA1), !(PINTeclado & COLUNA1))){ return '1';}
	else if(debounce((PINTeclado & LINHA1), !(PINTeclado & COLUNA2))){ return '2';}
	else if(debounce((PINTeclado & LINHA1), !(PINTeclado & COLUNA3))){ return '3';}
	else if(debounce((PINTeclado & LINHA1), !(PINTeclado & COLUNA4))){ return 'A';}
	else if(debounce((PINTeclado & LINHA2), !(PINTeclado & COLUNA1))){ return '4';}
	else if(debounce((PINTeclado & LINHA2), !(PINTeclado & COLUNA2))){ return '5';}
	else if(debounce((PINTeclado & LINHA2), !(PINTeclado & COLUNA3))){ return '6';}
	else if(debounce((PINTeclado & LINHA2), !(PINTeclado & COLUNA4))){ return 'B';}
	else if(debounce((PINTeclado & LINHA3), !(PINTeclado & COLUNA1))){ return '7';}
	else if(debounce((PINTeclado & LINHA3), !(PINTeclado & COLUNA2))){ return '8';}
	else if(debounce((PINTeclado & LINHA3), !(PINTeclado & COLUNA3))){ return '9';}
	else if(debounce((PINTeclado & LINHA3), !(PINTeclado & COLUNA4))){ return 'C';}
	else if(debounce((PINTeclado & LINHA4), !(PINTeclado & COLUNA1))){ return '*';}
	else if(debounce((PINTeclado & LINHA4), !(PINTeclado & COLUNA2))){ return '0';}
	else if(debounce((PINTeclado & LINHA4), !(PINTeclado & COLUNA3))){ return '#';}
	else if(debounce((PINTeclado & LINHA4), !(PINTeclado & COLUNA4))){ return 'D';}
	return 0;
}

// Varre as linhas com o nível lógico baixo
void scan_linhas(){
	static char linha_atual = 1;
	
	if(linha_atual == 1){PORTTeclado &= ~LINHA1; PORTTeclado |= LINHA4;} 
	else if(linha_atual == 2){PORTTeclado &= ~LINHA2; PORTTeclado |= LINHA1;}
	else if(linha_atual == 3){PORTTeclado &= ~LINHA3; PORTTeclado |= LINHA2;}
	else if(linha_atual == 4){PORTTeclado &= ~LINHA4; PORTTeclado |= LINHA3;}
	
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
	PORTDisplay &= ~DB7;
	PORTDisplay &= ~DB6;
	PORTDisplay |= DB5;
	PORTDisplay &= ~DB4;
	PORTDisplay &= ~RS;
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	delay_50us();
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	PORTDisplay |= DB7;
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	delay_50us();
}
void entryModeSet(char id, char s) {
	PORTDisplay &= ~RS;
	PORTDisplay &= ~DB7;
	PORTDisplay &= ~DB6;
	PORTDisplay &= ~DB5;
	PORTDisplay &= ~DB4;
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	PORTDisplay |= DB6;
	if(id == 1){PORTDisplay |= DB5;} else{PORTDisplay &= ~DB5;}
	if(s == 1){PORTDisplay |= DB4;} else{PORTDisplay &= ~DB4;}
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	delay_50us();
}
void displayOnOffControl(char display, char cursor, char blinking) {
	PORTDisplay &= ~DB7;
	PORTDisplay &= ~DB6;
	PORTDisplay &= ~DB5;
	PORTDisplay &= ~DB4;
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	PORTDisplay |= DB7;
	if(display == 1){PORTDisplay |= DB6;} else{PORTDisplay &= ~DB6;}
	if(cursor == 1){PORTDisplay |= DB5;} else{PORTDisplay &= ~DB5;}
	if(blinking == 1){PORTDisplay |= DB4;} else{PORTDisplay &= ~DB4;}
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	delay_50us();
}
char getBit(char c, char bitNumber) {
	return (c >> bitNumber) & 1;
}
void sendChar(char c) {
	if(getBit(c, 7) == 1){PORTDisplay |= DB7;} else{PORTDisplay &= ~DB7;}
	if(getBit(c, 6) == 1){PORTDisplay |= DB6;} else{PORTDisplay &= ~DB6;}
	if(getBit(c, 5) == 1){PORTDisplay |= DB5;} else{PORTDisplay &= ~DB5;}
	if(getBit(c, 4) == 1){PORTDisplay |= DB4;} else{PORTDisplay &= ~DB4;}
	PORTDisplay |= RS;
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	if(getBit(c, 3) == 1){PORTDisplay |= DB7;} else{PORTDisplay &= ~DB7;}
	if(getBit(c, 2) == 1){PORTDisplay |= DB6;} else{PORTDisplay &= ~DB6;}
	if(getBit(c, 1) == 1){PORTDisplay |= DB5;} else{PORTDisplay &= ~DB5;}
	if(getBit(c, 0) == 1){PORTDisplay |= DB4;} else{PORTDisplay &= ~DB4;}
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	delay_50us();
}
void setDdRamAddress(char address) {
	PORTDisplay &= ~RS;
	PORTDisplay |= DB7;
	if(getBit(address, 6) == 1){PORTDisplay |= DB6;} else{PORTDisplay &= ~DB6;}
	if(getBit(address, 5) == 1){PORTDisplay |= DB5;} else{PORTDisplay &= ~DB5;}
	if(getBit(address, 4) == 1){PORTDisplay |= DB4;} else{PORTDisplay &= ~DB4;}
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	if(getBit(address, 3) == 1){PORTDisplay |= DB7;} else{PORTDisplay &= ~DB7;}
	if(getBit(address, 2) == 1){PORTDisplay |= DB6;} else{PORTDisplay &= ~DB6;}
	if(getBit(address, 1) == 1){PORTDisplay |= DB5;} else{PORTDisplay &= ~DB5;}
	if(getBit(address, 0) == 1){PORTDisplay |= DB4;} else{PORTDisplay &= ~DB4;}
	PORTDisplay |= E;
	PORTDisplay &= ~E;
	delay_50us();
}
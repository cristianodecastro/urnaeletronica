#include <avr/io.h>
#include "pinHandling.h"
#include "timeControl.h"
#include "lcd.h"
#include "matrix_keypad.h"

int main(void){	
	
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
	HIGH(LINHA1);
	HIGH(LINHA2);
	HIGH(LINHA3);
	HIGH(LINHA4);
	
	// Inicialização do display
	functionSet();
	entryModeSet(1, 0);
	displayOnOffControl(1, 1, 1);
	
	char tecla_atual = 0;
	
	while (1)
	{
		tecla_atual = get_tecla();
		
		if(tecla_atual != 0){
			setDdRamAddress(0x00);
			sendChar(tecla_atual);
		}
	}
}

#include <avr/io.h>
#include "pinHandling.h"
#include "timeControl.h"
#include "lcd.h"
#include "matrix_keypad.h"
#include "serialFunctions.h"

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
	HIGH(COLUNA1);
	HIGH(COLUNA2);
	HIGH(COLUNA3);
	HIGH(COLUNA4);
	
	// Inicialização do display
	functionSet();
	entryModeSet(1, 0);
	displayOnOffControl(1, 1, 1);
	
	timeControlConfig();
	initSerialConfig();

	char tecla_atual = 0;
	char mensagem[] = "A tecla e x";
	
	while (1)
	{
		
		tecla_atual = get_tecla();
		mensagem[10] = tecla_atual;
		
		if(tecla_atual != 0){
			//UDR0 = tecla_atual;
			sendSerialMessage(mensagem);
			/*sendSerialChar(mensagem[0]);
			sendSerialChar(mensagem[1]);
			sendSerialChar(mensagem[2]);
			sendSerialChar(mensagem[3]);
			sendSerialChar(mensagem[4]);
			sendSerialChar(mensagem[5]);
			sendSerialChar(mensagem[6]);
			sendSerialChar(mensagem[7]);
			sendSerialChar(mensagem[8]);
			sendSerialChar(mensagem[9]);
			sendSerialChar(mensagem[10]);*/
			/*setDdRamAddress(0x40);
			sendChar(tecla_atual);*/
		}
	}
}
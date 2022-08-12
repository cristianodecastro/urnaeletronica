#include "timeControl.h"
#include "pinHandling.h"
#include "serialFunctions.h"
#include "matrix_keypad.h"
#include "lcd.h"

#include <avr/io.h>

int main(void)
{

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

	functionSet();
	entryModeSet(1, 0);
	displayOnOffControl(1, 1, 1);

	timeControlConfig();
	initSerialConfig();
	char voto_presidente[5] = {0};
	char resposta[100] = {0};
    /* Replace with your application code */
    while (1){
	    voto_presidente[0] = 'U';
		voto_presidente[1] = 'P';
		voto_presidente[2] = '\0';
		sendString(voto_presidente);
		voto_presidente[2] = 2;
		voto_presidente[3] = get_tecla();
		sendChar(voto_presidente[3]);
		voto_presidente[4] = get_tecla();
		sendChar(voto_presidente[4]);

		do{
			sendSerialMessage(voto_presidente);
			getSerialMessage(resposta);
		}while(resposta[0] == 'M' && resposta[1] == 'P');
		
		sendString_setAdress(&resposta[3], 2, 1);
    }
}


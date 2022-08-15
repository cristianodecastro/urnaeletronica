#include <avr/io.h>
#include "pinHandling.h"
#include "timeControl.h"
#include "lcd.h"
#include "matrix_keypad.h"
#include "serialFunctions.h"
#include "buzzer.h"

int main(void){
	
	// Display como outputs
	OUTPUT(D4);
	OUTPUT(D5);
	OUTPUT(D6);
	OUTPUT(D7);
	OUTPUT(E);
	OUTPUT(RS);
	
	// Linhas como outputs e colunas como inputs
	OUTPUT(LINHA1);
	OUTPUT(LINHA2);
	OUTPUT(LINHA3);
	OUTPUT(LINHA4);
	INPUT(COLUNA1);
	INPUT(COLUNA2);
	INPUT(COLUNA3);
	INPUT(COLUNA4);
	
	// Buzzer e LED como outputs
	OUTPUT(BUZZER);
	OUTPUT(LED);
	
	
	// Garante que todas as linhas comecem em nível lógico alto
	HIGH(LINHA1);
	HIGH(LINHA2);
	HIGH(LINHA3);
	HIGH(LINHA4);
	HIGH(COLUNA1);
	HIGH(COLUNA2);
	HIGH(COLUNA3);
	HIGH(COLUNA4);
	
	// Garante que o buzzer e o LED comecem desligados
	LOW(BUZZER);
	LOW(LED);
	
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
			//sendSerialMessage(mensagem);
			
			//setDdRamAddress(0x00);
			//setDdRamAddress(0x40);
			//clearDisplay();
			/*setDdRamAddress(0x01);
			sendString(mensagem);
			setDdRamAddress(0x43);
			sendString(mensagem);*/
			
  			sendString_setAdress(mensagem, 1, 3);
			//sendString_setAdress(mensagem, 2, 5);
			//char strTime[9] = "12 34 56";
			//sendString_setAdress(strTime, 2, 1);
			
		}
		//votingEndSong();
		/*HIGH(LED);
		for(int i=0; i<1000; i++){
			delay_microsegundos(500);
		}
		LOW(LED);
		for(int i=0; i<1000; i++){
			delay_microsegundos(500);
		}*/
	}
}
#include <avr/io.h>
#include "pinHandling.h"
#include "timeControl.h"
#include "lcd.h"
#include "matrix_keypad.h"
#include "serialFunctions.h"
#include "buzzer.h"

#define BLOQUEADA 0
#define OPERACIONAL 1
#define AGUARDANDO 2
#define ENCERRADA 3
#define OPTION_ESTADO 1
#define OPTION_ELEITOR 2
#define OPTION_CONSULTA_HORA 3
#define OPTION_TROCA_HORA 4
#define OPTION_VERIFICA_CORRESPONDENCIA 5
#define OPTION_RELATORIO_VOTACAO 6

char get_menu_tecla();

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
	displayOnOffControl(1, 0, 0);
	storage_special_characters();
	
	timeControlConfig();
	initSerialConfig();

	char urna_estado = OPERACIONAL;
	char menu_operacional_index = 1;
	char tecla = 0;
	char option_menu_operacional = 0;
	
	while (1){
		
		while(urna_estado == BLOQUEADA){
			// TODO
		}
		
		while(urna_estado == OPERACIONAL){
			
			switch(option_menu_operacional){
				case OPTION_ESTADO:
					// TODO
					break;
				case OPTION_ELEITOR:
					// TODO
					break;
				case OPTION_CONSULTA_HORA:
					sendString_setAdress("HORA ATUAL:   ", 1, 1);
					sendString_setAdress("              ", 2, 1);  sendChar(LEFT_ARROW_CHARACTER); sendChar('B');
					exibe_hora_display();
					if(get_tecla() == 'B'){
						apaga_hora_display();
						option_menu_operacional = 0;
						menu_operacional_index = 3;
					}
					break;
				case OPTION_TROCA_HORA:
					// TODO
					break;
				case OPTION_VERIFICA_CORRESPONDENCIA:
					// TODO
					break;
				case OPTION_RELATORIO_VOTACAO:
					// TODO
					break;
			}
			
			switch(menu_operacional_index){RIGHT_OPTION_CHARACTER
				case 1:
					sendString_setAdress("1. Estado do  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("    Sistema   ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = get_menu_tecla();
					if(tecla == 'B'){ menu_operacional_index++;}
					else if(tecla == 'A'){ option_menu_operacional = OPTION_ESTADO; menu_operacional_index = 0;}
					break;
				case 2:
					sendString_setAdress("2.    Novo    ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("     Eleitor  ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = get_menu_tecla();
					if(tecla == 'B'){ menu_operacional_index++;}
					else if(tecla == 'A'){ option_menu_operacional = OPTION_ELEITOR; menu_operacional_index = 0;}
					break;
				case 3:
					sendString_setAdress("3.  Consulta  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("   de Horario ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = get_menu_tecla();
					if(tecla == 'B'){ menu_operacional_index++;}
					else if(tecla == 'A'){ option_menu_operacional = OPTION_CONSULTA_HORA; menu_operacional_index = 0;}
					break;
				case 4:
					sendString_setAdress("4.  Troca de  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("    Horario   ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = get_menu_tecla();
					if(tecla == 'A'){ menu_operacional_index++;}
					else if(tecla == 'A'){ option_menu_operacional = OPTION_TROCA_HORA; menu_operacional_index = 0;}
					break;
				case 5:
					sendString_setAdress("5.  Verific.  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("de Correspond.", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = get_menu_tecla();
					if(tecla == 'B'){ menu_operacional_index++;}
					else if(tecla == 'A'){ option_menu_operacional = OPTION_VERIFICA_CORRESPONDENCIA; menu_operacional_index = 0;}
					break;
				case 6:
					sendString_setAdress("6. Relatorio  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("  de Votacao  ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = get_menu_tecla();
					if(tecla == 'A'){ menu_operacional_index = 1;}
					else if(tecla == 'A'){ option_menu_operacional = OPTION_RELATORIO_VOTACAO; menu_operacional_index = 0;}
					break;
				default:
					break;
			}
		}
		
		while(urna_estado == AGUARDANDO){
			// TODO
		}
		
		while(urna_estado == ENCERRADA){
			// TODO
		}
	}
}

// Retorna opção 'A' (avançar próximo item)
// ou 'B' (confirmar opção) do menu
char get_menu_tecla(){
	char tecla = 0;
	while(tecla != 'A' && tecla != 'B'){
		tecla = get_tecla();
		if(tecla == 'A' || tecla == 'B'){
			return tecla;
		}
	}
	return 0;
}

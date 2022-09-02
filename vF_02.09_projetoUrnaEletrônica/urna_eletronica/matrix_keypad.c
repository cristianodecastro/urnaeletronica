#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "pinHandling.h"
#include "timeControl.h"
#include "lcd.h"
#include "matrix_keypad.h"
#include "serialFunctions.h"
#include "buzzer.h"
#include "funcoes_candidato.h"
#include "funcoes_eleitor.h"


// Retorna apenas caracteres A ou B
char getCharacter_AB(){
	char tecla = 0;
	while(1){
		tecla = get_tecla();
		if(tecla == 'A' || tecla == 'B' || tecla == OVERTIME_VOTACAO){ return tecla;}
	}
}

// Retorna apenas caractere B
char getCharacter_B(){
	char tecla = 0;
	while(1){
		tecla = get_tecla();
		if(tecla == 'B' || tecla == OVERTIME_VOTACAO){ return tecla;}
	}
}

// Retorna caractere pressionado
char get_tecla(){
	while(1){
		if(votacao_overtime == OVERTIME_VOTACAO){ votacao_overtime = 0; return OVERTIME_VOTACAO;}
		if(!debounce(1, 1)){ delay_ms(250); return '1';}
		if(!debounce(1, 2)){ delay_ms(250); return '2';}
		if(!debounce(1, 3)){ delay_ms(250); return '3';}
		if(!debounce(1, 4)){ delay_ms(250); return 'A';}
		if(!debounce(2, 1)){ delay_ms(250); return '4';}
		if(!debounce(2, 2)){ delay_ms(250); return '5';}
		if(!debounce(2, 3)){ delay_ms(250); return '6';}
		if(!debounce(2, 4)){ delay_ms(250); return 'B';}
		if(!debounce(3, 1)){ delay_ms(250); return '7';}
		if(!debounce(3, 2)){ delay_ms(250); return '8';}
		if(!debounce(3, 3)){ delay_ms(250); return '9';}
		if(!debounce(3, 4)){ delay_ms(250); return 'C';}
		if(!debounce(4, 1)){ delay_ms(250); return '*';}
		if(!debounce(4, 2)){ delay_ms(250); return '0';}
		if(!debounce(4, 3)){ delay_ms(250); return '#';}
		if(!debounce(4, 4)){ delay_ms(250); return 'D';}
	}
	return '\0';
}

// Realiza o debounce das teclas
unsigned char debounce(char linha, char coluna){
	unsigned char current_key = 0, last_key = 0, counter = 0;
	char const BOUNCE = 7;
	
	// ativa a leitura da linha indicada
	switch (linha){
		case 1:
		LOW(LINHA1); HIGH(LINHA2); HIGH(LINHA3); HIGH(LINHA4);
		break;
		case 2:
		HIGH(LINHA1); LOW(LINHA2); HIGH(LINHA3); HIGH(LINHA4);
		break;
		case 3:
		HIGH(LINHA1); HIGH(LINHA2); LOW(LINHA3); HIGH(LINHA4);
		break;
		case 4:
		HIGH(LINHA1); HIGH(LINHA2); HIGH(LINHA3); LOW(LINHA4);
	}
	
	// Enquanto contador for diferente de BOUNCE, itera até estabilzar
	do{
		delay_ms(1);
		
		switch(coluna){
			case 1:
			current_key = READ(COLUNA1);
			break;
			case 2:
			current_key = READ(COLUNA2);
			break;
			case 3:
			current_key = READ(COLUNA3);
			break;
			case 4:
			current_key = READ(COLUNA4);
		}
		
		if(last_key == current_key){
			counter++;
		}else{
			counter = 0;
		}
		
		last_key = current_key;
	}while(counter < BOUNCE);

	switch(coluna){
		case 1:
		while(READ(COLUNA1) == 0);
		break;
		case 2:
		while(READ(COLUNA2) == 0);
		break;
		case 3:
		while(READ(COLUNA3) == 0);
		break;
		case 4:
		while(READ(COLUNA4) == 0);
	}
	return current_key;
}
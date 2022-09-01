#include <avr/io.h>
#include "pinHandling.h"
#include "lcd.h"
#include "timeControl.h"
#include "matrix_keypad.h"

// Conjunto de funções utilizadas para o Display LCD 16x2


void functionSet(void) {
	LOW(D7);
	LOW(D6);
	HIGH(D5);
	LOW(D4);
	LOW(RS);
	HIGH(E);
	LOW(E);
	HIGH(E);
	LOW(E);
	delay_us(40);
	HIGH(E);
	LOW(E);
	HIGH(D7);
	HIGH(E);
	LOW(E);
	delay_us(40);
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
	delay_us(40);
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
	delay_us(40);
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
	delay_us(40);
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
	delay_us(40);
}

void setCgRamAddress(char address) {
	LOW(RS);
	LOW(D7);
	HIGH(D6);
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
	delay_us(40);
}

void cursorHome(){
	LOW(RS);
	LOW(D7);
	LOW(D6);
	LOW(D5);
	LOW(D4);
	HIGH(E);
	LOW(E);
	HIGH(D5);
	HIGH(E);
	LOW(E);
	delay_us(40);
}

void sendString_setAdress(char* str, char linha, char coluna) {
	int index = 0;
	char endereco = 0;
	
	if(linha == 2){
		endereco += 0x40;
	}
	endereco += (coluna - 1);
	if((endereco >= 0 && endereco <= 0x0F) || (endereco >= 0x40 && endereco <= 0x4F)){
		setDdRamAddress(endereco);
	}
	while (str[index] != 0) {
		sendChar(str[index]);
		index++;
	}
}

void sendString(char* str) {
	int index = 0;
	while (str[index] != 0) {
		sendChar(str[index]);
		index++;
	}
}

void clearDisplay(){
	LOW(RS);
	LOW(D7);
	LOW(D6);
	LOW(D5);
	LOW(D4);
	HIGH(E);
	LOW(E);
	HIGH(D4);
	HIGH(E);
	LOW(E);
	delay_us(1640);
}

void storage_special_characters(){
	char down_arrow[8] = {     // Endereço 0x00
		0b00100,
		0b00100,
		0b00100,
		0b00100,
		0b10101,
		0b01110,
		0b00100,
		0b00000
	};
	char right_option[8] = {   // Endereço 0x08
		0b00000,
		0b00001,
		0b00011,
		0b10110,
		0b11100,
		0b01000,
		0b00000,
		0b00000
	};
	char return_option[8] = {   // Endereço 0x10
		0b01110,
		0b10001,
		0b10001,
		0b10100,
		0b10010,
		0b11111,
		0b00010,
		0b00100
	};
	char login_symbol[8] = {   // Endereço 0x18
		0b01110,
		0b10001,
		0b00001,
		0b00001,
		0b11111,
		0b11011,
		0b11011,
		0b11111

	};

	char up_and_down[8] = {
		0b00000,
		0b01000,
		0b10100,
		0b00000,
		0b00101,
		0b00010,
		0b00000,
		0b00000
	};

	char left_and_right[8] = {
		0b00100,
		0b01000,
		0b00100,
		0b00000,
		0b00100,
		0b00010,
		0b00100,
		0b00000
	};

	char uppercase_lowercase[8] = {
// 		0b00100,
// 		0b01010,
// 		0b01110,
// 		0b01010,
// 		0b00000,
// 		0b00110,
// 		0b01010,
// 		0b00110
		
		0b11100,
		0b01000,
		0b01000,
		0b01000,
		0b00010,
		0b00111,
		0b00010,
		0b00011
	};

	setCgRamAddress(0x40);
	int i = 0;
	for(i = 0; i < 8; i++) {
		sendChar(down_arrow[i]);
	}
	for(i = 0; i < 8; i++) {
		sendChar(right_option[i]);
	}
	for(i = 0; i < 8; i++) {
		sendChar(return_option[i]);
	}
	for(i = 0; i < 8; i++) {
		sendChar(login_symbol[i]);
	}
	for(i = 0; i < 8; i++) {
		sendChar(up_and_down[i]);
	}
	for(i = 0; i < 8; i++) {
		sendChar(left_and_right[i]);
	}
	for(i = 0; i < 8; i++) {
		sendChar(uppercase_lowercase[i]);
	}
	setDdRamAddress(0);
}


// Imprime na tela msg de erro de comunicação serial
void print_erroComunicacao(){
	sendString_setAdress("ERRO NA COMUNIC.", 1, 1);
	sendString_setAdress("     SERIAL   ", 2, 1);
}

void print_optionsVotacao(){
	sendString_setAdress("A:  B:BRANCO C: ", 2, 1);
	setDdRamAddress(0x40 + 2);
	sendChar(RIGHT_OPTION_CHARACTER);
	setDdRamAddress(0x40 + 15);
	sendChar(LEFT_ARROW_CHARACTER);
}

void consulta_hora(char* menu_index, char* option_menu_operacional){
	sendString_setAdress("HORA ATUAL:     ", 1, 1);
	sendString_setAdress("              ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
	exibe_hora_display();
	getCharacter_B();
	apaga_hora_display();	
	*option_menu_operacional = 0;
	*menu_index = 3;
}

void troca_hora(char* menu_index, char* option_menu_operacional){
	
	char tecla;
	unsigned char input_caracters_counter = 0;
	sendString_setAdress("INSERIR HORARIO ", 1, 1);
	sendString_setAdress("<  :  > ", 2, 1); sendChar(LEFT_ARROW_CHARACTER); sendChar('C'); sendChar(' '); sendChar(RETURN_CHARACTER); sendChar('B'); sendChar(' '); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
	
	char guarda_novo_horario[4] = {0};
	do{
		tecla = get_tecla();
		// se tecla A e horário com 4 dígitos, prossegue para mudar hora
		if(tecla == 'A' && input_caracters_counter == 4){
			// avisa se conseguiu alterar horário, e se é válido ou não
			if(!setTime(((guarda_novo_horario[0] - 48) * 10) + (guarda_novo_horario[1] - 48), ((guarda_novo_horario[2] - 48) * 10) + (guarda_novo_horario[3] - 48))){
				sendString_setAdress("HORARIO ALTERADO", 1, 1);
				sendString_setAdress("  COM SUCESSO   ", 2, 1);
			}
			else{
				sendString_setAdress("VALOR INVALIDO, ", 1, 1);
				sendString_setAdress("TENTE NOVAMENTE ", 2, 1);
			}
			delay_ms(2000);
			*menu_index = 4;
			*option_menu_operacional = 0;
		}
		// se a tecla for B, volta para o menu
		else if(tecla == 'B') {*menu_index = 4; *option_menu_operacional = 0;}
		// se a tecla for C, apaga o último caractere
		else if(tecla == 'C' && input_caracters_counter > 0){
			input_caracters_counter--;
			if(input_caracters_counter < 2){
				setDdRamAddress(0x41 + input_caracters_counter);
			}
			else{
				setDdRamAddress(0x42 + input_caracters_counter);
			}
			sendChar(' ');
		}
		// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
		else if(input_caracters_counter < 4 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
			if(input_caracters_counter < 2){
				setDdRamAddress(0x41 + input_caracters_counter);
			}
			else{
				setDdRamAddress(0x42 + input_caracters_counter);
			}
			sendChar(tecla);
			guarda_novo_horario[input_caracters_counter] = tecla;
			input_caracters_counter++;
		}
	} while(*option_menu_operacional == OPTION_TROCA_HORA);
}

void ver_estado(char* menu_index, char* option_menu_operacional, char* urna_estado){
	switch (*urna_estado){
		case OPERACIONAL:
			sendString_setAdress("OPERACIONAL   ", 1, 1); sendChar(RETURN_CHARACTER); sendChar('A');
			break;
		case AGUARDANDO:
			sendString_setAdress("AGUARDANDO    ", 1, 1); sendChar(RETURN_CHARACTER); sendChar('A');
			break;
		case ENCERRADA:
			sendString_setAdress("ENCERRADA     ", 1, 1); sendChar(RETURN_CHARACTER); sendChar('A');
			break;
	}
	sendString_setAdress(" Bloquear?    ", 2, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('B');
	char tecla = getCharacter_AB();
	if(tecla == 'B') {
		*urna_estado = BLOQUEADA;
		*menu_index = 1;
		sendString_setAdress("BLOQUEANDO URNA ", 1, 1);
		sendString_setAdress("      ...       ", 2, 1);
		delay_ms(1000);
	}
	else if(tecla == 'A') {*option_menu_operacional = 0; *menu_index = 1;}
}
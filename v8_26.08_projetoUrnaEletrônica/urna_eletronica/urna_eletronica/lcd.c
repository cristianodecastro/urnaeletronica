#include <avr/io.h>
#include "pinHandling.h"
#include "lcd.h"
#include "timeControl.h"

// Conjunto de fun??es utilizadas para o Display LCD 16x2


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
	char down_arrow[8] = {     // Endere?o 0x00
		0b00100,
		0b00100,
		0b00100,
		0b00100,
		0b10101,
		0b01110,
		0b00100,
		0b00000
	};
	char right_option[8] = {   // Endere?o 0x08
		0b00000,
		0b00001,
		0b00011,
		0b10110,
		0b11100,
		0b01000,
		0b00000,
		0b00000
	};
	char return_option[8] = {   // Endere?o 0x08
		0b01110,
		0b10001,
		0b10001,
		0b10100,
		0b10010,
		0b11111,
		0b00010,
		0b00100
	};	
	
	setCgRamAddress(0x40);
	int i = 0;
	for(i = 0; i < 8; i++) {
		sendChar(down_arrow[i]);
	}
	//setCgRamAddress(0x48);
	for(i = 0; i < 8; i++) {
		sendChar(right_option[i]);
	}
	for(i = 0; i < 8; i++) {
		sendChar(return_option[i]);
	}
	setDdRamAddress(0);
}


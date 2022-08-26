#include "serialFunctions.h"
#include "timeControl.h"

#include <avr/io.h>
#include <string.h>

// habilita a transmiss�o serial e define a taxa de transmiss�o
void initSerialConfig(){
	UCSR0B = 16+8; // habilita transmiss�o e recep��o serial
	UBRR0 = 51; // baud rate de 19200 bps
}

// aguarda a chegada de um caractere na comunica��o serial e retorna-o
char getSerialChar(){
	while((UCSR0A & (1<<7)) == 0); // aguarda uma recep��o serial
	return UDR0;
}

// envia um caractere via comunica��o serial
void sendSerialChar(char character){
	UDR0 = character;
	while((UCSR0A & (1<<5)) == 0); // aguarda finalizar a transmiss�o serial para enviar pr�ximo caractere
}

// captura uma mensagem recebida via comunica��o serial
void getSerialMessage(char* message){
	do{
		message[0] = getSerialChar();
	} while(message[0] != 'M');
	
	if(message[0] == 'M'){
		message[1] = getSerialChar();
		if((message[1] == 'I' || message[1] == 'C' || message[1] == 'T' || message[1] == 'R')){
			message[2] = '\0';
			return;
		}
		if(message[1] == 'V'){
			message[2] = getSerialChar();
			if(message[2] == 'O' || message[2] == 'I' || message[2] == 'X'){
				message[3] = '\0';
				return;
			}
		}
		if(message[1] == 'H'){
			message[2] = getSerialChar();
			message[3] = getSerialChar();
			if((message[2] < 24 && message[3] < 60)){
				message[4] = '\0';
				return;
			}
		}
		if((message[1] == 'N' || message[1] == 'P' || /*message[1] == 'G' ||*/ message[1] == 'S' || message[1] == 'F')){
			message[2] = getSerialChar();
			for(char i=0; i<message[2]; i++){
				message[3+i] = getSerialChar();
			}
			return;
		}
	}
	message[0] = '\0';
	return;
}

// envia uma mensagem via comunica��o serial
void sendSerialMessage(char* message){
	int tamanhoStr = strlen(message);
	for(int i=0; i<=tamanhoStr; i++){
		sendSerialChar(message[i]);
	}
}
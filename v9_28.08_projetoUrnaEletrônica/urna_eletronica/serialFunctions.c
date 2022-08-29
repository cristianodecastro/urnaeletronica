#include "serialFunctions.h"
#include "timeControl.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

// habilita a transmissão serial e define a taxa de transmissão
void initSerialConfig(){
	UCSR0B = 16+8; // habilita transmissão e recepção serial
	UBRR0 = 51; // baud rate de 19200 bps
	UCSR0B |= (1<<7); // habilita interrupção por recepção serial
	sei();
}

// aguarda a chegada de um caractere na comunicação serial e retorna-o
char getSerialChar(){
	UCSR0B &= ~(1<<7); // desabilita interrupção por recepção serial
	while((UCSR0A & (1<<7)) == 0); // aguarda uma recepção serial
	return UDR0;
	UCSR0B |= (1<<7); // habilita interrupção por recepção serial
}

// envia um caractere via comunicação serial
void sendSerialChar(char character){
	UDR0 = character;
	while((UCSR0A & (1<<5)) == 0); // aguarda finalizar a transmissão serial para enviar próximo caractere
}

// captura uma mensagem recebida via comunicação serial
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

// envia uma mensagem via comunicação serial
void sendSerialMessage(char* message){
	int tamanhoStr = strlen(message);
	for(int i=0; i<=tamanhoStr; i++){
		sendSerialChar(message[i]);
	}
}

ISR(USART_RX_vect){
	char msg[4];
	msg[0] = UDR0;
	if(msg[0] == 'M'){
		UCSR0B &= ~(1<<7); // desabilita interrupção por recepção serial
		msg[1] = getSerialChar();
		if(msg[1] == 'H'){
			msg[2] = getSerialChar();
			msg[3] = getSerialChar();
			if(!setTime(msg[2], msg[3])){
				sendSerialMessage("UH");
			}
		}
		UCSR0B |= (1<<7); // habilita interrupção por recepção serial
	}
}

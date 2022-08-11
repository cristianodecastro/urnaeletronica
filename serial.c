#include <avr/io.h>
#include <string.h>

// habilita a transmissão serial e define a taxa de transmissão
void inicializaSerial(){
	UCSR0B = 16+8; // habilita transmissão e recepção serial
	UBRR0 = 103; // baud rate de 9600 bps
}

// aguarda a chegada de um caractere na comunicação serial e retorna-o
char getSerialChar(){
	while((UCSR0A & (1<<7)) == 0); // aguarda uma recepção serial
	return UDR0;
}

// envia um caractere via comunicação serial
void sendSerialChar(char character){
	UDR0 = character;
}

// captura uma mensagem recebida via comunicação serial
void getSerialMessage(char* message){
	
}

// envia uma mensagem via comunicação serial
void sendSerialMessage(char* message){
	char strSize = strlen(message);
	for(char i=0; i<strSize; i--){
		sendSerialChar(message[i]);
		while((UCSR0A & (1<<7)) == 0); // aguarda finalizar a transmissão serial para enviar próximo caractere
	}
	
}

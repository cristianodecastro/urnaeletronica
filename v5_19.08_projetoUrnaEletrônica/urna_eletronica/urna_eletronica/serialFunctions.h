#ifndef SERIALFUNCTIONS_H_
#define SERIALFUNCTIONS_H_

#include <avr/io.h>
#include <string.h>


// habilita a transmissão serial e define a taxa de transmissão
void initSerialConfig();

// aguarda a chegada de um caractere na comunicação serial e retorna-o
char getSerialChar();

// envia um caractere via comunicação serial
void sendSerialChar(char);

// captura uma mensagem recebida via comunicação serial
void getSerialMessage(char*);

// envia uma mensagem via comunicação serial
void sendSerialMessage(char*);


#endif /* SERIALFUNCTIONS_H_ */
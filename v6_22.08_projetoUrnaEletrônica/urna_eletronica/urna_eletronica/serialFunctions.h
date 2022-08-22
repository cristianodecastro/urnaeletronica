#ifndef SERIALFUNCTIONS_H_
#define SERIALFUNCTIONS_H_

#include <avr/io.h>
#include <string.h>


// habilita a transmiss�o serial e define a taxa de transmiss�o
void initSerialConfig();

// aguarda a chegada de um caractere na comunica��o serial e retorna-o
char getSerialChar();

// envia um caractere via comunica��o serial
void sendSerialChar(char);

// captura uma mensagem recebida via comunica��o serial
void getSerialMessage(char*);

// envia uma mensagem via comunica��o serial
void sendSerialMessage(char*);


#endif /* SERIALFUNCTIONS_H_ */
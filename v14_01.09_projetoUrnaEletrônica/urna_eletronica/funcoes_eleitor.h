#ifndef FUNCOES_ELEITOR_H_
#define FUNCOES_ELEITOR_H_

// Struct que contem as informações do eleitor
typedef struct eleitor{
	char numero[6];
	char nome[17];
	unsigned char hora_justificativa[3];
}ELEITOR;


char valida_eleitor(char*, char*, unsigned char/*, ELEITOR**/);

#endif /* FUNCOES_ELEITOR_H_ */
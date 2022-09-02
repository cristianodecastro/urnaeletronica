#ifndef FUNCOES_ELEITOR_H_
#define FUNCOES_ELEITOR_H_

// Struct que contem as informações do eleitor
typedef struct eleitor{
	char numero[6];
	char nome[17];
	unsigned char hora_justificativa[3];
}ELEITOR;


char valida_eleitor(char*, char*);
char justifica_voto(ELEITOR*);
void corrige_string(char*);
char get_proximo_caractere(char);
char get_caractere_anterior(char);
char get_uppercase_lowercase(char);
void cleanString(char*);

#endif /* FUNCOES_ELEITOR_H_ */
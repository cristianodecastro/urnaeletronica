#ifndef FUNCOES_CANDIDATO_H_
#define FUNCOES_CANDIDATO_H_

// Struct que contem as informações do candidato
typedef struct dados_candidato{
	char numero[6];
	char nome[17];
	char num_votos;
}DADOS_CANDIDATO;

typedef struct candidato{
	DADOS_CANDIDATO* dados_candidato;
	unsigned char num_candidatos_votados;
}CANDIDATO;

char buscaCandidato(CANDIDATO, char*);
void registraVoto(CANDIDATO*, char*, char*);
unsigned int conta_bytes_candidato(CANDIDATO, char*);
void envia_votos_relatorio(CANDIDATO);


#endif /* FUNCOES_CANDIDATO_H_ */
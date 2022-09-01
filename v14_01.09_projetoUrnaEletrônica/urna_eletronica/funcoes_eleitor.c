#include <avr/io.h>
#include "funcoes_eleitor.h"
#include "serialFunctions.h"
#include "pinHandling.h"


// Valida eleitor
char valida_eleitor(char* eleitor_num, char* nome_eleitor, unsigned char num_eleitores/*, ELEITOR* eleitor*/){
	/*unsigned char eleitor_index;
	for(eleitor_index = 0; eleitor_index < num_eleitores; eleitor_index++){
		if(eleitores_que_votaram[eleitor_index].numero[0] == eleitor[0]){
			if(eleitores_que_votaram[eleitor_index].numero[1] == eleitor[1]){
				if(eleitores_que_votaram[eleitor_index].numero[2] == eleitor[2]){
					if(eleitores_que_votaram[eleitor_index].numero[3] == eleitor[3]){
						if(eleitores_que_votaram[eleitor_index].numero[4] == eleitor[4]){
							return ELEITOR_VOTOU;
						}
					}
				}
			}
		}
	}*/
	
	char message[9] = {'U', 'N', 5};
	char codigo_invalido[] = "Codigo invalido";
	char response[21] = {0};
	
	// Constrói a mensagem a ser enviada serialmente
	unsigned char i;
	for(i = 0; i < 5; i++){
		message[i + 3] = eleitor_num[i];
	}
	message[8] = '\0';
	
	// Envia mensagem e obtém resposta
	sendSerialMessage(message);
	getSerialMessage(response);
	
	// Se não estiver no padrão, ocorreu um erro
	if(response[0] != 'M' || response[1] != 'N'){
		return ERRO_COMUNICACAO;
	}
	
	// Se a resposta conter "Codigo invalido\0", o eleitor não é válido
	char eleitor_invalido = 1;
	// Obtem nome do eleitor
	for(i = 0; i < response[2]; i++){
		nome_eleitor[i] = response[3 + i];
		if(nome_eleitor[i] != codigo_invalido[i]){
			eleitor_invalido = 0;
		}
	}
	
	if(eleitor_invalido){
		return ELEITOR_INVALIDO;
	}
	return ELEITOR_VALIDO;
}
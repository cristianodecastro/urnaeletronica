#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "pinHandling.h"
#include "timeControl.h"
#include "lcd.h"
#include "matrix_keypad.h"
#include "serialFunctions.h"
#include "buzzer.h"
#include "funcoes_candidato.h"
#include "funcoes_eleitor.h"

char buscaCandidato(CANDIDATO candidato, char* numero){
	unsigned char posicao_candidato;
	for(posicao_candidato = 0; posicao_candidato < candidato.num_candidatos_votados; posicao_candidato++){
		// Compara os dois primeiros caracteres do candidato atual
		if(candidato.dados_candidato[posicao_candidato].numero[0] == numero[0] && candidato.dados_candidato[posicao_candidato].numero[1] == numero[1]){
			// Verifica se o candidato é um deputado federal, se não, o candidato já existe e retorna sua posição
			if(candidato.dados_candidato[posicao_candidato].numero[2] == '\0'){
				return posicao_candidato;
			}
			// Sendo deputado federal, compara os demais caracteres
			else if(candidato.dados_candidato[posicao_candidato].numero[2] == numero[2] && candidato.dados_candidato[posicao_candidato].numero[3] == numero[3]){
				return posicao_candidato;
			}
		}
	}
	return CANDIDATO_INEXISTENTE;
}

void registraVoto(CANDIDATO* candidato, char* candidato_numero, char* candidato_nome){
	unsigned char posicao_candidato = buscaCandidato(*candidato, candidato_numero);
	// Se o candidato não existe, o registra e incrementa o numero de votos e de candidatos votados
	if(posicao_candidato == CANDIDATO_INEXISTENTE){
		strcpy(candidato->dados_candidato[candidato->num_candidatos_votados].nome, candidato_nome);
		strcpy(candidato->dados_candidato[candidato->num_candidatos_votados].numero, candidato_numero);
		candidato->dados_candidato[candidato->num_candidatos_votados].num_votos++;
		candidato->num_candidatos_votados++;
	}
	else{
		candidato->dados_candidato[posicao_candidato].num_votos++;
	}
}

unsigned int conta_bytes_candidato(CANDIDATO candidato, char* titulo){
	unsigned int n_caracteres = 0;
	unsigned char candidato_atual;
	n_caracteres += strlen(titulo);
	for(candidato_atual = 0; candidato_atual < candidato.num_candidatos_votados; candidato_atual++){
		n_caracteres += 3; // ("   ")
		n_caracteres += 3; // 2 caracteres + ' '
		n_caracteres += (strlen(candidato.dados_candidato[candidato_atual].nome) + 2); // n caracteres + '\r' + '\n'
	}
	return n_caracteres;
}

void envia_votos_relatorio(CANDIDATO candidato){
	unsigned char candidato_atual;
	for(candidato_atual = 0; candidato_atual < candidato.num_candidatos_votados; candidato_atual++){
		char msa_num_votos = (candidato.dados_candidato[candidato_atual].num_votos / 10) + 48;
		char lsa_num_votos = (candidato.dados_candidato[candidato_atual].num_votos % 10) + 48;
		sendSerialMessage("   ");
		sendSerialChar(msa_num_votos); sendSerialChar(lsa_num_votos); sendSerialChar(' ');
		sendSerialMessage(candidato.dados_candidato[candidato_atual].nome); sendSerialChar('\r'); sendSerialChar('\n');
	}
}
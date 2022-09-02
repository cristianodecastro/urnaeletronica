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


// Valida eleitor
char valida_eleitor(char* eleitor_num, char* nome_eleitor){

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

char justifica_voto(ELEITOR* eleitor){
	// caracteres: ' ', 'A', 'B', ..., 'Z'
	char tecla;
	unsigned char posicao_atual = 0;
	char nome[17] = "                ";
	char numero[6] = {0};


	// Imprime na tela e espera confirmação
	sendString_setAdress("A SEGUIR INSIRA ", 1, 1);
	sendString_setAdress("O NUMERO        ", 2, 1);
	setDdRamAddress(0x40 + 11);
	sendChar('A'); sendChar(RIGHT_OPTION_CHARACTER); sendChar(' ');
	sendChar('B'); sendChar(RETURN_CHARACTER);

	tecla = getCharacter_AB();
	if(tecla == 'B'){
		return JUSTIFICATIVA_NAO_CONCLUIDA;
	}

	// Usuário insere número de eleitor
	sendString_setAdress("Eleitor:        ", 1, 1);
	sendString_setAdress("A:    B:    C:  ", 2, 1);
	setDdRamAddress(0x40 + 2);
	sendChar(RIGHT_OPTION_CHARACTER);
	setDdRamAddress(0x40 + 8);
	sendChar(RETURN_CHARACTER);
	setDdRamAddress(0x40 + 14);
	sendChar(LEFT_ARROW_CHARACTER);
	setDdRamAddress(8);

	posicao_atual = 0;

	do{

		tecla = get_tecla();

		if(tecla == 'B'){
			return JUSTIFICATIVA_NAO_CONCLUIDA;
		}
		else if(tecla == 'C' && posicao_atual > 0){
			posicao_atual--;
			setDdRamAddress(8 + posicao_atual);
			sendChar(' ');
		}
		else if(posicao_atual < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'C' && tecla != 'A'){
			setDdRamAddress(8 + posicao_atual);
			sendChar(tecla);
			numero[posicao_atual] = tecla;
			posicao_atual++;
		}
	} while(tecla != 'A' || posicao_atual != 5);

	// Verifca se o eleitor já existe-------------------------------------------------------------------------------------------------------------------------

	cleanString(nome);

	// Usuário insere o nome
	sendString_setAdress("A SEGUIR INSIRA ", 1, 1);
	sendString_setAdress("O NOME          ", 2, 1);
	setDdRamAddress(0x40 + 11);
	sendChar('A'); sendChar(RIGHT_OPTION_CHARACTER); sendChar(' ');
	sendChar('B'); sendChar(RETURN_CHARACTER);

	tecla = getCharacter_AB();
	if(tecla == 'B'){
		return JUSTIFICATIVA_NAO_CONCLUIDA;
	}

	sendString_setAdress("                ", 1, 1);
	setDdRamAddress(0x40);
	sendChar('2'); sendChar(UP_AND_DOWN_CARACTERE); sendString("8 4");
	sendChar(LEFT_AND_RIGHT_CARACTERE); sendString("6 ");
	sendChar(UPPERCASE_LOWERCASE_CARACTERE); sendString("5 ");
	sendChar(RETURN_CHARACTER); sendString("B ");
	sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');

	posicao_atual = 0;
	
	do{
		tecla = get_tecla();

		if(tecla == '2'){
			nome[posicao_atual] = get_caractere_anterior(nome[posicao_atual]);
			setDdRamAddress(posicao_atual); sendChar(nome[posicao_atual]);
		}
		else if(tecla == '8'){
			nome[posicao_atual] = get_proximo_caractere(nome[posicao_atual]);
			setDdRamAddress(posicao_atual); sendChar(nome[posicao_atual]);
		}
		else if(tecla == '4' && posicao_atual > 0){
			posicao_atual--;
		}
		else if(tecla == '6' && posicao_atual < 15){
			posicao_atual++;
		}
		else if(tecla == '5'){
			nome[posicao_atual] = get_uppercase_lowercase(nome[posicao_atual]);
			setDdRamAddress(posicao_atual); sendChar(nome[posicao_atual]);
		}
		else if(tecla == 'B'){
			return JUSTIFICATIVA_NAO_CONCLUIDA;
		}
	}while(tecla != 'A');

	// Corrige string e guarda informações
	//corrige_string(nome);

	strcpy(eleitor->numero, numero);
	strcpy(eleitor->nome, nome);
	eleitor->hora_justificativa[HOURS] = currentTime[HOURS];
	eleitor->hora_justificativa[MINUTES] = currentTime[MINUTES];
	eleitor->hora_justificativa[SECONDS] = '\0';

	return JUSTIFICATIVA_CONCLUIDA;
}

// Adiciona '\0' caso a string termine com espaços no final
void corrige_string(char* string_sem_fim){
	unsigned char i;
	for(i = sizeof(string_sem_fim); i > 0; i--){
		if(string_sem_fim[i] == ' ' && ((string_sem_fim[i-1] >= 'A' && string_sem_fim[i-1] <= 'Z') || (string_sem_fim[i-1] >= 'a' && string_sem_fim[i-1] <= 'z'))){
			string_sem_fim[i] = '\0';
			i = 0;
		}
	}
}

// Retorna o próxima letra do alfabeto. 'z' -> ' ' e 'Z' -> ' '
char get_proximo_caractere(char caractere){
	if((caractere >= 'A' && caractere < 'Z') || (caractere >= 'a' && caractere < 'z')){
		caractere++;
	}
	else if(caractere == 'Z' || caractere == 'z'){
		caractere = ' ';
	}
	else{
		caractere = 'A';
	}
	return caractere;
}

// Retorna a letra anterior do alfabeto. 'a' -> ' ' e 'A' -> ' '
char get_caractere_anterior(char caractere){
	if((caractere > 'A' && caractere <= 'Z') || (caractere > 'a' && caractere <= 'z')){
		caractere--;
	}
	else if(caractere == 'A' || caractere == 'a'){
		caractere = ' ';
	}
	else{
		caractere = 'Z';
	}
	return caractere;
}

// Retorna o inverso do caractere de entrada, no que diz respeito a ser maisculo ou minusculo
char get_uppercase_lowercase(char caractere){
	if(caractere >= 'A' && caractere <= 'Z'){
		caractere += 32;
	}
	else if(caractere >= 'a' && caractere <= 'z'){
		caractere -= 32;
	}
	return caractere;
}

void cleanString(char* string){
	unsigned char i;
	for(i = 0; i < sizeof(string); i++){
		string[i] = 0;
	}
}
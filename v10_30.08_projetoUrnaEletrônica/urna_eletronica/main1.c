#include <avr/io.h>
#include "pinHandling.h"
#include "timeControl.h"
#include "lcd.h"
#include "matrix_keypad.h"
#include "serialFunctions.h"
#include "buzzer.h"

#define BLOQUEADA 1
#define OPERACIONAL 2
#define AGUARDANDO 3
#define ENCERRADA 4
#define OPTION_ESTADO 1
#define OPTION_ELEITOR 2
#define OPTION_CONSULTA_HORA 3
#define OPTION_TROCA_HORA 4
#define OPTION_VERIFICA_CORRESPONDENCIA 5
#define OPTION_RELATORIO_VOTACAO 6
#define OPTION_TROCA_SENHA 7
#define OPTION_RESET_SENHA 8

#define ERRO_COMUNICACAO 0xff
#define ELEITOR_VALIDO 1
#define ELEITOR_INVALIDO 0
#define TEMPO_ESGOTADO 1
#define VOTACAO_CONCLUIDA 2

// Struct que contem as informações do eleitor
typedef struct eleitor{
	char numero[5];
	char nome[17];
	unsigned char hora_justificativa[3];
}ELEITOR;

// Struct que contem as informações do candidato
typedef struct candidato{
	char codigo[5];
	char nome[17];
	char cargo[17];
	char partido[2];                               // {0, 0} para sem partido
}CANDIDATO;

// Struct que contém o relatório de votação
typedef struct relatorio_votacao{
	CANDIDATO candidato[16];           // 4 presidentes, 8 dep. federais, 3 senadores, 1 voto em branco
	char num_votos[16];
}RELATORIO;

// Variável global que contém o número de eleitores
char num_eleitores = 0;

CANDIDATO candidato[13] = {
	{{'1', '3', 0, 0, 0}, "Lula", "Presidente", {'0', '0'}},
	{{'2', '2', 0, 0, 0}, "Jair Bolsonaro", "Presidente", {0, 0}},
	{{'2', '7', 0, 0, 0}, "Eymael", "Presidente", {0, 0}},
	{{'2', '5', 0, 0, 0}, "Simone Tebet", "Presidente", {0, 0}},
	{{'1', '2', 0, 0, 0}, "Ciro Gomes", "Presidente", {0, 0}},
	{{'1', '3', 0, 0, 0}, "Olivio Dutra", "Senador", {0, 0}},
	{{'1', '0', 0, 0, 0}, "Hamilton Mourao", "Senador", {0, 0}},
	{{'1', '1', 0, 0, 0}, "Ana Amelia", "Senador", {0, 0}},
	{{'1', '9', '0', '1', 0}, "Lasier Martins", "Dep Federal", {0, 0}},
	{{'4', '4', '5', '6', 0}, "Fortunati", "Dep Federal", {0, 0}},
	{{'4', '0', '0', '0', 0}, "Gilvan o Gringo", "Dep Federal", {0, 0}},
	{{'2', '0', '0', '2', 0}, "Sergio Cabeludo", "Dep Federal", {0, 0}},
	{{'2', '0', '0', '2', 0}, "Sergio Cabeludo", "Dep Federal", {0, 0}}
};

// Protótipos
char getCharacter_AB();
char getCharacter_B();
char valida_eleitor(char*, char*);
char votacao(RELATORIO*);
void print_erroComunicacao();
void print_optionsVotacao();
void strCpy(char*, char*);
void cleanString(char*);

int main(void){
	
	// Display como outputs
	OUTPUT(D4);
	OUTPUT(D5);
	OUTPUT(D6);
	OUTPUT(D7);
	OUTPUT(E);
	OUTPUT(RS);
	
	// Linhas como outputs e colunas como inputs
	OUTPUT(LINHA1);
	OUTPUT(LINHA2);
	OUTPUT(LINHA3);
	OUTPUT(LINHA4);
	INPUT(COLUNA1);
	INPUT(COLUNA2);
	INPUT(COLUNA3);
	INPUT(COLUNA4);
	
	// Buzzer e LED como outputs
	OUTPUT(BUZZER);
	OUTPUT(LED);
	
	// Garante que todas as linhas comecem em nível lógico alto
	HIGH(LINHA1);
	HIGH(LINHA2);
	HIGH(LINHA3);
	HIGH(LINHA4);
	HIGH(COLUNA1);
	HIGH(COLUNA2);
	HIGH(COLUNA3);
	HIGH(COLUNA4);
	
	// Garante que o buzzer e o LED comecem desligados
	LOW(BUZZER);
	LOW(LED);
	
	// Inicialização do display
	functionSet();
	entryModeSet(1, 0);
	displayOnOffControl(1, 0, 0);
	storage_special_characters();
	
	// Configurações de timer e serial
	timeControlConfig();
	initSerialConfig();

	RELATORIO relatorio_votacao;		// Relatorio da votacao
	ELEITOR eleitor[33];				// Dados dos eleitores, considerando, por absurdo, que todos justifiquem
	unsigned char num_justificativas = 0; // Número de eleitores que justificaram
	char tecla = 0;						// Guarda tecla pressionada
	char urna_estado = OPERACIONAL;		// Estado da urna
	char menu_index = 1;	// Índice do menu_operacional
	char option_menu_operacional = 1;	// Opção selecionada do menu_operacional
	char numero_eleitor[5] = {0};		// Guarda número do eleitor
	char nome_eleitor[17] = {0};		// Guarda nome do eleitor (max 16 caracteres + \0)
	char guarda_senha_mesario[5] = {0};
	char guarda_login_mesario[5] = {0};
	const char login_mesario[5] = "01237";
	const char senha_inicial[5] = "12378";
	unsigned char input_caracters_counter = 0;	// Contador de daracteres inseridos pelo usuário
	char encerrada_bloqueada = 0;
	char verificacao = 0;
	char consistencia[50] = {0};
	
	while (1){
		
		while(urna_estado == BLOQUEADA){

			switch(menu_index){
				case 1:
					sendString_setAdress("      Urna      ", 1, 1);
					sendString_setAdress("   Bloqueada  ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('A');
					tecla = getCharacter_AB();
					// se a tecla apertada for A, espera o login do mesário
					if(tecla == 'A') {menu_index = 2; /*break;*/}
					else {menu_index = 1; /*break;*/}
					break;
				// login do mesário
				case 2:
					input_caracters_counter = 0;
					sendString_setAdress("Login adm:      ", 1, 1);
					sendString_setAdress("A:    B:    C:  ", 2, 1);
					setDdRamAddress(0x40 + 2);
					sendChar(RIGHT_OPTION_CHARACTER);
					setDdRamAddress(0x40 + 8);
					sendChar(RETURN_CHARACTER);
					setDdRamAddress(0x40 + 14);
					sendChar(LEFT_ARROW_CHARACTER);
					setDdRamAddress(8);

					// char i, a;
					do{
						// a = 0;
						tecla = get_tecla();
						// se tecla A e login com 5 dígitos, prossegue para senha
						if(tecla == 'A' && input_caracters_counter == 5) {menu_index = 3;}
						// se a tecla for B, volta para "urna bloqueada"
						else if(tecla == 'B') {menu_index = 1;}
						// se a tecla for C, apaga o último caractere (não funcionando)
						else if(tecla == 'C' && input_caracters_counter > 0){
							input_caracters_counter--;
							setDdRamAddress(10 + input_caracters_counter);
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
							setDdRamAddress(10 + input_caracters_counter);
							sendChar(tecla);
							guarda_login_mesario[input_caracters_counter] = tecla;
							input_caracters_counter++;
						}
					} while(menu_index == 2);
					break;
				// senha do mesário
				case 3:
					input_caracters_counter = 0;
					sendString_setAdress("Senha adm:      ", 1, 1);
					sendString_setAdress("A:    B:    C:  ", 2, 1);
					setDdRamAddress(0x40 + 2);
					sendChar(RIGHT_OPTION_CHARACTER);
					setDdRamAddress(0x40 + 8);
					sendChar(RETURN_CHARACTER);
					setDdRamAddress(0x40 + 14);
					sendChar(LEFT_ARROW_CHARACTER);
					setDdRamAddress(8);
					
					unsigned char i, a;
					do{
						// a = 0;
						tecla = get_tecla();
						// se tecla A e login com 5 dígitos, prossegue para senha
						if(tecla == 'A' && input_caracters_counter == 5){
							a = 0;
							for(i = 0; i < 5; i++){
								if(guarda_login_mesario[i] == login_mesario[i] && guarda_senha_mesario[i] == senha_inicial[i])
								{a++;}
							}
							menu_index = 1;
							if(a == 5) {urna_estado = OPERACIONAL; /*break;*/}
							else{
								sendString_setAdress("     Mesario    ", 1, 1);
								sendString_setAdress("    invalido  ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
								tecla = getCharacter_B();
							}
							
						}
						// se a tecla for B, volta para "urna bloqueada"
						else if(tecla == 'B') {menu_index = 1;}
						// se a tecla for C, apaga o último caractere (não funcionando)
						else if(tecla == 'C' && input_caracters_counter > 0){
							input_caracters_counter--;
							setDdRamAddress(10 + input_caracters_counter);
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
							setDdRamAddress(10 + input_caracters_counter);
							sendChar('*');
							guarda_senha_mesario[input_caracters_counter] = tecla;
							input_caracters_counter++;
						}
					} while(menu_index == 3);
					break;
			}
			
			if(currentTime[HOURS] > 17 || (currentTime[HOURS] == 17 && currentTime[MINUTES] > 14)){
				urna_estado = ENCERRADA;
				sendString_setAdress("      URNA      ", 1, 1);
				sendString_setAdress("    ENCERRADA   ", 2, 1);
				delay_ms(2000);
				encerrada_bloqueada = 1;
				menu_index = 1;
				option_menu_operacional = 0;
			}

		}
		
		while(urna_estado == OPERACIONAL){
			
			// Menu do sistema
			switch(menu_index){
				case 1:
					sendString_setAdress("1. Estado do  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("    Sistema   ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B'){ menu_index++;}
					else if(tecla == 'A'){ option_menu_operacional = OPTION_ESTADO; menu_index = 0;}
					break;
				case 2:
					sendString_setAdress("2.    Novo    ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("     Eleitor  ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B'){ menu_index++;}
					else if(tecla == 'A'){ option_menu_operacional = OPTION_ELEITOR; menu_index = 0;}
					break;
				case 3:
					sendString_setAdress("3.  Consulta  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("   de Horario ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') { menu_index++;}
					else if(tecla == 'A') { option_menu_operacional = OPTION_CONSULTA_HORA; menu_index = 0;}
					break;
				case 4:
					sendString_setAdress("4.  Troca de  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("    Horario   ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') { menu_index++;}
					else if(tecla == 'A') { option_menu_operacional = OPTION_TROCA_HORA; menu_index = 0;}
					break;
				case 5:
					sendString_setAdress("5.  Verific.  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("de Correspond.", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') { menu_index++;}
					else if(tecla == 'A') { option_menu_operacional = OPTION_VERIFICA_CORRESPONDENCIA; menu_index = 0;}
					break;
				case 6:
					sendString_setAdress("6. Relatorio  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("  de Votacao  ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') {menu_index++;}
					else if(tecla == 'A') {option_menu_operacional = OPTION_RELATORIO_VOTACAO; menu_index = 0;}
					break;
				case 7:
					sendString_setAdress("7.  Troca de  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("     Senha    ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') {menu_index++;}
					else if(tecla == 'A') {option_menu_operacional = OPTION_TROCA_SENHA; menu_index = 0;}
					break;
				case 8:
					sendString_setAdress("8.  Resetar   ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("     Senha    ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') {menu_index = 1;}
					else if(tecla == 'A') {option_menu_operacional = OPTION_RESET_SENHA; menu_index = 0;}
					break;
				default:
					break;
			}
			
			// Opções do menu do sistema
			switch(option_menu_operacional){
				case OPTION_ESTADO:
					sendString_setAdress("OPERACIONAL   ", 1, 1); sendChar(RETURN_CHARACTER); sendChar('A');
					sendString_setAdress(" Bloquear?    ", 2, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') {
						urna_estado = BLOQUEADA;
						menu_index = 1;
						sendString_setAdress("BLOQUEANDO URNA ", 1, 1);
						sendString_setAdress("      ...       ", 2, 1);
						delay_ms(1000);
					}
					else if(tecla == 'A') {option_menu_operacional = 0; menu_index = 1;}
					break;
				
				// Novo eleitor
				case OPTION_ELEITOR:
					input_caracters_counter = 0;
					sendString_setAdress("Eleitor:        ", 1, 1);
					sendString_setAdress("A:    B:    C:  ", 2, 1);
					setDdRamAddress(0x40 + 2);
					sendChar(RIGHT_OPTION_CHARACTER);
					setDdRamAddress(0x40 + 8);
					sendChar(RETURN_CHARACTER);
					setDdRamAddress(0x40 + 14);
					sendChar(LEFT_ARROW_CHARACTER);
					setDdRamAddress(8);
					do{
						tecla = get_tecla();
						// Se a tecla for A, valida o eleitor
						if(tecla == 'A' && input_caracters_counter == 5){
							
							// Obtem validação do eleitor
							char validacao = valida_eleitor(numero_eleitor, nome_eleitor);
							
							// Se o eleitor for valido, exibe no display o nome e aguarda confirmação
							if(validacao == ELEITOR_VALIDO){
								sendString_setAdress(nome_eleitor, 1, 1);
								sendString_setAdress("Confirma?  ", 2, 1);
								sendChar(RIGHT_OPTION_CHARACTER); sendString("A ");
								sendChar(RETURN_CHARACTER); sendChar('B');
								tecla = getCharacter_AB();
								
								// Se a tecla for 'A', confirma o eleitor, guarda nome e numero e inicia processo de votacao
								if(tecla == 'A'){
									char validacao_votacao = 0;						
									// Obtem validacao da votacao
									validacao_votacao = votacao(&relatorio_votacao);
									
									// Se o tempo de 2 minutos foi esgotado
									if(validacao_votacao == TEMPO_ESGOTADO){
										sendString_setAdress("      TEMPO     ", 1, 1);
										sendString_setAdress("     ESGOTADO   ", 2, 1);
										while(1);
									}
									
									// Se houve erro na comunicação serial no processo de votação
									else if(validacao_votacao == ERRO_COMUNICACAO){
										print_erroComunicacao();
										tecla = getCharacter_B();
										option_menu_operacional = 0;
										menu_index = 2;
										while(1);
									}
									
									// Se a votacao foi concluida com sucesso--------------------------- CONCLUIR AQUI
									else if(validacao_votacao == VOTACAO_CONCLUIDA){
										num_eleitores++;
										sendString_setAdress("       FIM      ", 1, 1);
										sendString_setAdress("                ", 2, 1);
										votingEndSong();
										//while(1); 
									}
									/*else{
										sendString_setAdress("       ???      ", 1, 1);
										sendString_setAdress("                ", 2, 1);
										while(1);
									}*/
								}
								
								// Se a tecla for 'B', volta ao menu anterior
								else if(tecla == 'B'){
									option_menu_operacional = 0;
									menu_index = 2;
								}
							
							}
							
							// Se o eleitor for invalido, informa no display e aguarda retorno ao menu
							else if(validacao == ELEITOR_INVALIDO){
								sendString_setAdress("ELEITOR INVALIDO", 1, 1);
								sendString_setAdress("              ", 2, 1);
								sendChar(RETURN_CHARACTER); sendChar('B');
								tecla = getCharacter_B();
								option_menu_operacional = 0;
								menu_index = 2;
							}
							
							// Informa se houve erro na comunicação serial na obtenção do eleitor
							else if(validacao == ERRO_COMUNICACAO){
								print_erroComunicacao();
								sendChar(RETURN_CHARACTER); sendChar('B');
								tecla = getCharacter_B();
								option_menu_operacional = 0;
								menu_index = 2;
							}
						}
						
						// Se a tecla for B, volta para o menu
						else if(tecla == 'B'){
							option_menu_operacional = 0;
							menu_index = 2;
						}
						// Se a tecla for C, corrige último caractere
						else if(tecla == 'C' && input_caracters_counter > 0){
							input_caracters_counter--;
							setDdRamAddress(8 + input_caracters_counter);
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
							setDdRamAddress(8 + input_caracters_counter);
							sendChar(tecla);
							numero_eleitor[input_caracters_counter] = tecla;
							input_caracters_counter++;
						}
					} while(tecla != 'B');
					break;
				
				case OPTION_CONSULTA_HORA:
					sendString_setAdress("HORA ATUAL:     ", 1, 1);
					sendString_setAdress("              ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
					exibe_hora_display();
					if(get_tecla() == 'B'){
						apaga_hora_display();
						option_menu_operacional = 0;
						menu_index = 3;
					}
					break;
				
				case OPTION_TROCA_HORA:
					input_caracters_counter = 0;
					sendString_setAdress("INSERIR HORARIO ", 1, 1);
					sendString_setAdress("<  :  > ", 2, 1); sendChar(LEFT_ARROW_CHARACTER); sendChar('C'); sendChar(' '); sendChar(RETURN_CHARACTER); sendChar('B'); sendChar(' '); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					
					char guarda_novo_horario[4] = {0};
					do{
						tecla = get_tecla();
						// se tecla A e horário com 4 dígitos, prossegue para mudar hora
						if(tecla == 'A' && input_caracters_counter == 4){
							// avisa se conseguiu alterar horário, e se é válido ou não
							if(!setTime(((guarda_novo_horario[0] - 48) * 10) + (guarda_novo_horario[1] - 48), ((guarda_novo_horario[2] - 48) * 10) + (guarda_novo_horario[3] - 48))){
								sendString_setAdress("HORARIO ALTERADO", 1, 1);
								sendString_setAdress("  COM SUCESSO   ", 2, 1);
							}
							else{
								sendString_setAdress("VALOR INVALIDO, ", 1, 1);
								sendString_setAdress("TENTE NOVAMENTE ", 2, 1);
							}
							delay_ms(2000);
							menu_index = 4;
							option_menu_operacional = 0;
						}
						// se a tecla for B, volta para o menu
						else if(tecla == 'B') {menu_index = 4; option_menu_operacional = 0;}
						// se a tecla for C, apaga o último caractere
						else if(tecla == 'C' && input_caracters_counter > 0){
							input_caracters_counter--;
							if(input_caracters_counter < 2){
								setDdRamAddress(0x41 + input_caracters_counter);
							}
							else{
								setDdRamAddress(0x42 + input_caracters_counter);								
							}
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 4 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
							if(input_caracters_counter < 2){
								setDdRamAddress(0x41 + input_caracters_counter);
							}
							else{
								setDdRamAddress(0x42 + input_caracters_counter);
							}
							sendChar(tecla);
							guarda_novo_horario[input_caracters_counter] = tecla;
							input_caracters_counter++;
						}
					} while(option_menu_operacional == OPTION_TROCA_HORA);
					
					break;
				
				case OPTION_VERIFICA_CORRESPONDENCIA:
					do{
						sendSerialChar('U'); sendSerialChar('V');
						sendSerialChar(currentTime[HOURS]); sendSerialChar(currentTime[MINUTES]); sendSerialChar(num_eleitores);
						getSerialMessage(consistencia);
						if(consistencia[0] == 'M' && consistencia[1] == 'V'){
							if(consistencia[2] == 'O'){
								verificacao = 3;
								sendString_setAdress("  CONSISTENCIA  ", 1, 1);
								sendString_setAdress("       OK       ", 2, 1);
								delay_ms(1000);
								option_menu_operacional = 0;
								menu_index = 5;
							}
							else if(consistencia[2] == 'I'){
								verificacao++;
								if(verificacao == 3){
									urna_estado = BLOQUEADA;
									sendString_setAdress("  CONSISTENCIA  ", 1, 1);
									sendString_setAdress("     FALHOU     ", 2, 1);
									delay_ms(1000);
									sendString_setAdress("BLOQUEANDO URNA ", 1, 1);
									sendString_setAdress("      ...       ", 2, 1);
									delay_ms(1000);
									option_menu_operacional = 0;
									menu_index = 1;
								}
								
							}
							else if(consistencia[2] == 'X'){
								verificacao = 3;
								urna_estado = BLOQUEADA;
								sendString_setAdress("  CONSISTENCIA  ", 1, 1);
								sendString_setAdress("     FALHOU     ", 2, 1);
								delay_ms(1000);
								sendString_setAdress("BLOQUEANDO URNA ", 1, 1);
								sendString_setAdress("      ...       ", 2, 1);
								delay_ms(1000);
								option_menu_operacional = 0;
								menu_index = 1;
							}
						}
					} while(verificacao < 3);
					break;
				
				case OPTION_RELATORIO_VOTACAO:
					// TO DO
					break;

				case OPTION_TROCA_SENHA:
					input_caracters_counter = 0;
					char guarda_novasenha1_mesario[5] = {0};
					char guarda_novasenha2_mesario[5] = {0};
					char b = 1;
					// primeiro pedido de nova senha
					sendString_setAdress("Nova Senha:     ", 1, 1);
					sendString_setAdress("A:    B:    C:  ", 2, 1);
					setDdRamAddress(0x40 + 2);
					sendChar(RIGHT_OPTION_CHARACTER);
					setDdRamAddress(0x40 + 8);
					sendChar(RETURN_CHARACTER);
					setDdRamAddress(0x40 + 14);
					sendChar(LEFT_ARROW_CHARACTER);

					do{
						tecla = get_tecla();
						// se tecla A e senha com 5 dígitos, prossegue para repetição de senha
						if(tecla == 'A' && input_caracters_counter == 5){
							b = 2;
						}
						// se a tecla for B, volta para "urna bloqueada"
						else if(tecla == 'B'){
							option_menu_operacional = 0;
							b = 3;
						}
						// se a tecla for C, apaga o último caractere (não funcionando)
						else if(tecla == 'C' /*&& input_caracters_counter > 0*/){
							input_caracters_counter--;
							setDdRamAddress(11 + input_caracters_counter);
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A'){
							setDdRamAddress(11 + input_caracters_counter);
							sendChar('*');
							guarda_novasenha1_mesario[input_caracters_counter] = tecla;
							input_caracters_counter++;
						}
					} while(b == 1);
					
					if(b != 3){
						sendString_setAdress(" Repita a Nova  ", 1, 1);
						sendString_setAdress("    Senha     ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('A');
						tecla = getCharacter_AB();

						// segundo pedido de nova senha
						input_caracters_counter = 0;
						sendString_setAdress("Nova Senha:     ", 1, 1);
						sendString_setAdress("A:    B:    C:  ", 2, 1);
						setDdRamAddress(0x40 + 2);
						sendChar(RIGHT_OPTION_CHARACTER);
						setDdRamAddress(0x40 + 8);
						sendChar(RETURN_CHARACTER);
						setDdRamAddress(0x40 + 14);
						sendChar(LEFT_ARROW_CHARACTER);

						unsigned char a, i;
						do{
							tecla = get_tecla();
							// se tecla A e senha com 5 dígitos, confere senhas
							if(tecla == 'A' && input_caracters_counter == 5){
								a = 0;
								for(i = 0; i < 5; i++){
									if(guarda_novasenha1_mesario[i] == guarda_novasenha2_mesario[i]) {a++;}
								}
								menu_index = OPTION_TROCA_SENHA;
								if(a == 5){
									for(i = 0; i < 5; i++) {guarda_senha_mesario[i] = guarda_novasenha1_mesario[i];}
									sendString_setAdress(" Troca de Senha ", 1, 1);
									sendString_setAdress("    Efetuada  ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
								}
								else{
									sendString_setAdress("   Senhas Nao   ", 1, 1);
									sendString_setAdress("    Conferem  ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
								}
								tecla = getCharacter_B();
								option_menu_operacional = 0;
								b = 3;
							}
							// se a tecla for B, volta para "urna bloqueada"
							else if(tecla == 'B'){
								option_menu_operacional = 0;
								menu_index = OPTION_TROCA_SENHA;
								b = 3;
							}
							// se a tecla for C, apaga o último caractere (não funcionando)
							else if(tecla == 'C' /*&& input_caracters_counter > 0*/){
								input_caracters_counter--;
								setDdRamAddress(11 + input_caracters_counter);
								sendChar(' ');
							}
							// se for um caractere válido e ainda não tiver execedido o limite, é contabilizado
							else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A'){
								setDdRamAddress(11 + input_caracters_counter);
								sendChar('*');
								guarda_novasenha2_mesario[input_caracters_counter] = tecla;
								input_caracters_counter++;
							}
						} while(b == 2);
					}
					else {option_menu_operacional = 0; menu_index = OPTION_TROCA_SENHA;}
					break;
				
				case OPTION_RESET_SENHA:

					break;
			}
			if(currentTime[HOURS] < 8){
				if(urna_estado != BLOQUEADA){
					urna_estado = AGUARDANDO;
				}
			}
			else if(num_eleitores + num_justificativas >= 33 || currentTime[HOURS] > 17 || (currentTime[HOURS] == 17 && currentTime[MINUTES] > 14)){
				urna_estado = ENCERRADA;
				sendString_setAdress("      URNA      ", 1, 1);
				sendString_setAdress("    ENCERRADA   ", 2, 1);
				delay_ms(2000);
				encerrada_bloqueada = 0;
				menu_index = 1;
				option_menu_operacional = 0;
			}
		}
		
		while(urna_estado == AGUARDANDO){
			// Menu do sistema
			switch(menu_index){
				case 1:
					sendString_setAdress("1. Estado do  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("    Sistema   ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B'){ menu_index+=2;}
					else if(tecla == 'A'){ option_menu_operacional = OPTION_ESTADO; menu_index = 0;}
					break;
				case 2:
					menu_index++;
					break;
				case 3:
					sendString_setAdress("3.  Consulta  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("   de Horario ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') { menu_index++;}
					else if(tecla == 'A') { option_menu_operacional = OPTION_CONSULTA_HORA; menu_index = 0;}
					break;
				case 4:
					sendString_setAdress("4.  Troca de  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("    Horario   ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') { menu_index++;}
					else if(tecla == 'A') { option_menu_operacional = OPTION_TROCA_HORA; menu_index = 0;}
					break;
				case 5:
					sendString_setAdress("5.  Verific.  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("de Correspond.", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') { menu_index++;}
					else if(tecla == 'A') { option_menu_operacional = OPTION_VERIFICA_CORRESPONDENCIA; menu_index = 0;}
					break;
				case 6:
					sendString_setAdress("6. Relatorio  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("  de Votacao  ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') {menu_index++;}
					else if(tecla == 'A') {option_menu_operacional = OPTION_RELATORIO_VOTACAO; menu_index = 0;}
					break;
				case 7:
					sendString_setAdress("7.  Troca de  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("     Senha    ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') {menu_index++;}
					else if(tecla == 'A') {option_menu_operacional = OPTION_TROCA_SENHA; menu_index = 0;}
					break;
				case 8:
					sendString_setAdress("8.  Resetar   ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("     Senha    ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') {menu_index = 1;}
					else if(tecla == 'A') {option_menu_operacional = OPTION_RESET_SENHA; menu_index = 0;}
					break;
				default:
					break;
			}
			
			// Opções do menu do sistema
			switch(option_menu_operacional){
				case OPTION_ESTADO:
					sendString_setAdress("AGUARDANDO    ", 1, 1); sendChar(RETURN_CHARACTER); sendChar('A');
					sendString_setAdress(" Bloquear?    ", 2, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') {
						urna_estado = BLOQUEADA;
						menu_index = 1;
						sendString_setAdress("BLOQUEANDO URNA ", 1, 1);
						sendString_setAdress("      ...       ", 2, 1);
						delay_ms(1000);
					}
					else if(tecla == 'A') {option_menu_operacional = 0; menu_index = 1;}
					break;
				case OPTION_ELEITOR:
					break;
				case OPTION_CONSULTA_HORA:
					sendString_setAdress("HORA ATUAL:     ", 1, 1);
					sendString_setAdress("              ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
					exibe_hora_display();
					if(get_tecla() == 'B'){
						apaga_hora_display();
						option_menu_operacional = 0;
						menu_index = 3;
					}
					break;
				
				case OPTION_TROCA_HORA:
					input_caracters_counter = 0;
					sendString_setAdress("INSERIR HORARIO ", 1, 1);
					sendString_setAdress("<  :  > ", 2, 1); sendChar(LEFT_ARROW_CHARACTER); sendChar('C'); sendChar(' '); sendChar(RETURN_CHARACTER); sendChar('B'); sendChar(' '); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					
					char guarda_novo_horario[4] = {0};
					do{
						tecla = get_tecla();
						// se tecla A e horário com 4 dígitos, prossegue para mudar hora
						if(tecla == 'A' && input_caracters_counter == 4){
							// avisa se conseguiu alterar horário, e se é válido ou não
							if(!setTime(((guarda_novo_horario[0] - 48) * 10) + (guarda_novo_horario[1] - 48), ((guarda_novo_horario[2] - 48) * 10) + (guarda_novo_horario[3] - 48))){
								sendString_setAdress("HORARIO ALTERADO", 1, 1);
								sendString_setAdress("  COM SUCESSO   ", 2, 1);
							}
							else{
								sendString_setAdress("VALOR INVALIDO, ", 1, 1);
								sendString_setAdress("TENTE NOVAMENTE ", 2, 1);
							}
							delay_ms(2000);
							menu_index = 4;
							option_menu_operacional = 0;
						}
						// se a tecla for B, volta para o menu
						else if(tecla == 'B') {menu_index = 4; option_menu_operacional = 0;}
						// se a tecla for C, apaga o último caractere
						else if(tecla == 'C' && input_caracters_counter > 0){
							input_caracters_counter--;
							if(input_caracters_counter < 2){
								setDdRamAddress(0x41 + input_caracters_counter);
							}
							else{
								setDdRamAddress(0x42 + input_caracters_counter);								
							}
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 4 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
							if(input_caracters_counter < 2){
								setDdRamAddress(0x41 + input_caracters_counter);
							}
							else{
								setDdRamAddress(0x42 + input_caracters_counter);
							}
							sendChar(tecla);
							guarda_novo_horario[input_caracters_counter] = tecla;
							input_caracters_counter++;
						}
					} while(option_menu_operacional == OPTION_TROCA_HORA);
					
					break;
				
				case OPTION_VERIFICA_CORRESPONDENCIA:
					do{
						sendSerialChar('U'); sendSerialChar('V');
						sendSerialChar(currentTime[HOURS]); sendSerialChar(currentTime[MINUTES]); sendSerialChar(num_eleitores);
						getSerialMessage(consistencia);
						if(consistencia[0] == 'M' && consistencia[1] == 'V'){
							if(consistencia[2] == 'O'){
								verificacao = 3;
								sendString_setAdress("  CONSISTENCIA  ", 1, 1);
								sendString_setAdress("       OK       ", 2, 1);
								delay_ms(1000);
								option_menu_operacional = 0;
								menu_index = 5;
							}
							else if(consistencia[2] == 'I'){
								verificacao++;
								if(verificacao == 3){
									urna_estado = BLOQUEADA;
									sendString_setAdress("  CONSISTENCIA  ", 1, 1);
									sendString_setAdress("     FALHOU     ", 2, 1);
									delay_ms(1000);
									sendString_setAdress("BLOQUEANDO URNA ", 1, 1);
									sendString_setAdress("      ...       ", 2, 1);
									delay_ms(1000);
									option_menu_operacional = 0;
									menu_index = 1;
								}
								
							}
							else if(consistencia[2] == 'X'){
								verificacao = 3;
								urna_estado = BLOQUEADA;
								sendString_setAdress("  CONSISTENCIA  ", 1, 1);
								sendString_setAdress("     FALHOU     ", 2, 1);
								delay_ms(1000);
								sendString_setAdress("BLOQUEANDO URNA ", 1, 1);
								sendString_setAdress("      ...       ", 2, 1);
								delay_ms(1000);
								option_menu_operacional = 0;
								menu_index = 1;
							}
						}
					} while(verificacao < 3);
					break;
				
				case OPTION_RELATORIO_VOTACAO:
					// TO DO
					break;

				case OPTION_TROCA_SENHA:
					input_caracters_counter = 0;
					char guarda_novasenha1_mesario[5] = {0};
					char guarda_novasenha2_mesario[5] = {0};
					char b = 1;
					// primeiro pedido de nova senha
					sendString_setAdress("Nova Senha:     ", 1, 1);
					sendString_setAdress("A:    B:    C:  ", 2, 1);
					setDdRamAddress(0x40 + 2);
					sendChar(RIGHT_OPTION_CHARACTER);
					setDdRamAddress(0x40 + 8);
					sendChar(RETURN_CHARACTER);
					setDdRamAddress(0x40 + 14);
					sendChar(LEFT_ARROW_CHARACTER);

					do{
						tecla = get_tecla();
						// se tecla A e senha com 5 dígitos, prossegue para repetição de senha
						if(tecla == 'A' && input_caracters_counter == 5){
							b = 2;
						}
						// se a tecla for B, volta para "urna bloqueada"
						else if(tecla == 'B'){
							option_menu_operacional = 0;
							b = 3;
						}
						// se a tecla for C, apaga o último caractere (não funcionando)
						else if(tecla == 'C' /*&& input_caracters_counter > 0*/){
							input_caracters_counter--;
							setDdRamAddress(11 + input_caracters_counter);
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A'){
							setDdRamAddress(11 + input_caracters_counter);
							sendChar('*');
							guarda_novasenha1_mesario[input_caracters_counter] = tecla;
							input_caracters_counter++;
						}
					} while(b == 1);
					
					if(b != 3){
						sendString_setAdress(" Repita a Nova  ", 1, 1);
						sendString_setAdress("    Senha     ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('A');
						tecla = getCharacter_AB();

						// segundo pedido de nova senha
						input_caracters_counter = 0;
						sendString_setAdress("Nova Senha:     ", 1, 1);
						sendString_setAdress("A:    B:    C:  ", 2, 1);
						setDdRamAddress(0x40 + 2);
						sendChar(RIGHT_OPTION_CHARACTER);
						setDdRamAddress(0x40 + 8);
						sendChar(RETURN_CHARACTER);
						setDdRamAddress(0x40 + 14);
						sendChar(LEFT_ARROW_CHARACTER);

						unsigned char a, i;
						do{
							tecla = get_tecla();
							// se tecla A e senha com 5 dígitos, confere senhas
							if(tecla == 'A' && input_caracters_counter == 5){
								a = 0;
								for(i = 0; i < 5; i++){
									if(guarda_novasenha1_mesario[i] == guarda_novasenha2_mesario[i]) {a++;}
								}
								menu_index = OPTION_TROCA_SENHA;
								if(a == 5){
									for(i = 0; i < 5; i++) {guarda_senha_mesario[i] = guarda_novasenha1_mesario[i];}
									sendString_setAdress(" Troca de Senha ", 1, 1);
									sendString_setAdress("    Efetuada  ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
								}
								else{
									sendString_setAdress("   Senhas Nao   ", 1, 1);
									sendString_setAdress("    Conferem  ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
								}
								tecla = getCharacter_B();
								option_menu_operacional = 0;
								b = 3;
							}
							// se a tecla for B, volta para "urna bloqueada"
							else if(tecla == 'B'){
								option_menu_operacional = 0;
								menu_index = OPTION_TROCA_SENHA;
								b = 3;
							}
							// se a tecla for C, apaga o último caractere (não funcionando)
							else if(tecla == 'C' /*&& input_caracters_counter > 0*/){
								input_caracters_counter--;
								setDdRamAddress(11 + input_caracters_counter);
								sendChar(' ');
							}
							// se for um caractere válido e ainda não tiver execedido o limite, é contabilizado
							else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A'){
								setDdRamAddress(11 + input_caracters_counter);
								sendChar('*');
								guarda_novasenha2_mesario[input_caracters_counter] = tecla;
								input_caracters_counter++;
							}
						} while(b == 2);
					}
					else {option_menu_operacional = 0; menu_index = OPTION_TROCA_SENHA;}
					break;
				
				case OPTION_RESET_SENHA:

					break;
			}
			if(currentTime[HOURS] >= 8 && (currentTime[HOURS] < 17 || (currentTime[HOURS] == 17 && currentTime[MINUTES] < 15))){
				if(urna_estado != BLOQUEADA){
					urna_estado = OPERACIONAL;
				}
			}
			else if(num_eleitores + num_justificativas >= 33 || currentTime[HOURS] > 17 || (currentTime[HOURS] == 17 && currentTime[MINUTES] > 14)){
				urna_estado = ENCERRADA;
				sendString_setAdress("      URNA      ", 1, 1);
				sendString_setAdress("    ENCERRADA   ", 2, 1);
				delay_ms(2000);
				encerrada_bloqueada = 0;
				menu_index = 1;
				option_menu_operacional = 0;
			}
		}
		
		while(urna_estado == ENCERRADA){
			if(encerrada_bloqueada){
				switch(menu_index){
					case 1:
					sendString_setAdress("      Urna      ", 1, 1);
					sendString_setAdress("   Encerrada  ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('A');
					tecla = getCharacter_AB();
					// se a tecla apertada for A, espera o login do mesário
					if(tecla == 'A') {menu_index = 2; /*break;*/}
					else {menu_index = 1; /*break;*/}
					break;
					// login do mesário
					case 2:
					input_caracters_counter = 0;
					sendString_setAdress("Login adm:      ", 1, 1);
					sendString_setAdress("A:    B:    C:  ", 2, 1);
					setDdRamAddress(0x40 + 2);
					sendChar(RIGHT_OPTION_CHARACTER);
					setDdRamAddress(0x40 + 8);
					sendChar(RETURN_CHARACTER);
					setDdRamAddress(0x40 + 14);
					sendChar(LEFT_ARROW_CHARACTER);
					setDdRamAddress(8);

					do{
						tecla = get_tecla();
						// se tecla A e login com 5 dígitos, prossegue para senha
						if(tecla == 'A' && input_caracters_counter == 5) {menu_index = 3;}
						// se a tecla for B, volta para "urna bloqueada"
						else if(tecla == 'B') {menu_index = 1;}
						// se a tecla for C, apaga o último caractere (não funcionando)
						else if(tecla == 'C' && input_caracters_counter > 0){
							input_caracters_counter--;
							setDdRamAddress(10 + input_caracters_counter);
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
							setDdRamAddress(10 + input_caracters_counter);
							sendChar(tecla);
							guarda_login_mesario[input_caracters_counter] = tecla;
							input_caracters_counter++;
						}
					} while(menu_index == 2);
					break;
					// senha do mesário
					case 3:
					input_caracters_counter = 0;
					sendString_setAdress("Senha adm:      ", 1, 1);
					sendString_setAdress("A:    B:    C:  ", 2, 1);
					setDdRamAddress(0x40 + 2);
					sendChar(RIGHT_OPTION_CHARACTER);
					setDdRamAddress(0x40 + 8);
					sendChar(RETURN_CHARACTER);
					setDdRamAddress(0x40 + 14);
					sendChar(LEFT_ARROW_CHARACTER);
					setDdRamAddress(8);
					
					unsigned char i, a;
					do{
						// a = 0;
						tecla = get_tecla();
						// se tecla A e login com 5 dígitos, prossegue para senha
						if(tecla == 'A' && input_caracters_counter == 5){
							a = 0;
							for(i = 0; i < 5; i++){
								if(guarda_login_mesario[i] == login_mesario[i] && guarda_senha_mesario[i] == senha_inicial[i])
								{a++;}
							}
							menu_index = 1;
							if(a == 5) {encerrada_bloqueada = 0; /*break;*/}
							else{
								sendString_setAdress("     Mesario    ", 1, 1);
								sendString_setAdress("    invalido  ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
								tecla = getCharacter_B();
							}
							
						}
						// se a tecla for B, volta para "urna bloqueada"
						else if(tecla == 'B') {menu_index = 1;}
						// se a tecla for C, apaga o último caractere (não funcionando)
						else if(tecla == 'C' && input_caracters_counter > 0){
							input_caracters_counter--;
							setDdRamAddress(10 + input_caracters_counter);
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
							setDdRamAddress(10 + input_caracters_counter);
							sendChar('*');
							guarda_senha_mesario[input_caracters_counter] = tecla;
							input_caracters_counter++;
						}
					} while(menu_index == 3);
					break;
				}
			}
			else{
				// Menu do sistema
				switch(menu_index){
					case 1:
						sendString_setAdress("1. Estado do  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
						sendString_setAdress("    Sistema   ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
						tecla = getCharacter_AB();
						if(tecla == 'B'){ menu_index+=2;}
						else if(tecla == 'A'){ option_menu_operacional = OPTION_ESTADO; menu_index = 0;}
						break;
					case 2:
						menu_index++;
						break;
					case 3:
						sendString_setAdress("3.  Consulta  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
						sendString_setAdress("   de Horario ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
						tecla = getCharacter_AB();
						if(tecla == 'B') { menu_index++;}
						else if(tecla == 'A') { option_menu_operacional = OPTION_CONSULTA_HORA; menu_index = 0;}
						break;
					case 4:
						sendString_setAdress("4.  Troca de  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
						sendString_setAdress("    Horario   ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
						tecla = getCharacter_AB();
						if(tecla == 'B') { menu_index++;}
						else if(tecla == 'A') { option_menu_operacional = OPTION_TROCA_HORA; menu_index = 0;}
						break;
					case 5:
						sendString_setAdress("5.  Verific.  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
						sendString_setAdress("de Correspond.", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
						tecla = getCharacter_AB();
						if(tecla == 'B') { menu_index++;}
						else if(tecla == 'A') { option_menu_operacional = OPTION_VERIFICA_CORRESPONDENCIA; menu_index = 0;}
						break;
					case 6:
						sendString_setAdress("6. Relatorio  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
						sendString_setAdress("  de Votacao  ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
						tecla = getCharacter_AB();
						if(tecla == 'B') {menu_index++;}
						else if(tecla == 'A') {option_menu_operacional = OPTION_RELATORIO_VOTACAO; menu_index = 0;}
						break;
					case 7:
						sendString_setAdress("7.  Troca de  ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
						sendString_setAdress("     Senha    ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
						tecla = getCharacter_AB();
						if(tecla == 'B') {menu_index++;}
						else if(tecla == 'A') {option_menu_operacional = OPTION_TROCA_SENHA; menu_index = 0;}
						break;
					case 8:
						sendString_setAdress("8.  Resetar   ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
						sendString_setAdress("     Senha    ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
						tecla = getCharacter_AB();
						if(tecla == 'B') {menu_index = 1;}
						else if(tecla == 'A') {option_menu_operacional = OPTION_RESET_SENHA; menu_index = 0;}
						break;
					default:
						break;
				}
				
				// Opções do menu do sistema
				switch(option_menu_operacional){
					case OPTION_ESTADO:
						sendString_setAdress("ENCERRADA     ", 1, 1); sendChar(RETURN_CHARACTER); sendChar('A');
						sendString_setAdress(" Bloquear?    ", 2, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('B');
						tecla = getCharacter_AB();
						if(tecla == 'B') {
							encerrada_bloqueada = 1;
							menu_index = 1;
							sendString_setAdress("BLOQUEANDO URNA ", 1, 1);
							sendString_setAdress("      ...       ", 2, 1);
							delay_ms(1000);
						}
						else if(tecla == 'A') {option_menu_operacional = 0; menu_index = 1;}
						break;
						
					case OPTION_ELEITOR:
						break;
						
					case OPTION_CONSULTA_HORA:
						sendString_setAdress("HORA ATUAL:     ", 1, 1);
						sendString_setAdress("              ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
						exibe_hora_display();
						if(get_tecla() == 'B'){
							apaga_hora_display();
							option_menu_operacional = 0;
							menu_index = 3;
						}
						break;
						
					case OPTION_TROCA_HORA:
						input_caracters_counter = 0;
						sendString_setAdress("INSERIR HORARIO ", 1, 1);
						sendString_setAdress("<  :  > ", 2, 1); sendChar(LEFT_ARROW_CHARACTER); sendChar('C'); sendChar(' '); sendChar(RETURN_CHARACTER); sendChar('B'); sendChar(' '); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					
						char guarda_novo_horario[4] = {0};
						do{
							tecla = get_tecla();
							// se tecla A e horário com 4 dígitos, prossegue para mudar hora
							if(tecla == 'A' && input_caracters_counter == 4){
								// avisa se conseguiu alterar horário, e se é válido ou não
								if(!setTime(((guarda_novo_horario[0] - 48) * 10) + (guarda_novo_horario[1] - 48), ((guarda_novo_horario[2] - 48) * 10) + (guarda_novo_horario[3] - 48))){
									sendString_setAdress("HORARIO ALTERADO", 1, 1);
									sendString_setAdress("  COM SUCESSO   ", 2, 1);
								}
								else{
									sendString_setAdress("VALOR INVALIDO, ", 1, 1);
									sendString_setAdress("TENTE NOVAMENTE ", 2, 1);
								}
								delay_ms(2000);
								menu_index = 4;
								option_menu_operacional = 0;
							}
							// se a tecla for B, volta para o menu
							else if(tecla == 'B') {menu_index = 4; option_menu_operacional = 0;}
							// se a tecla for C, apaga o último caractere
							else if(tecla == 'C' && input_caracters_counter > 0){
								input_caracters_counter--;
								if(input_caracters_counter < 2){
									setDdRamAddress(0x41 + input_caracters_counter);
								}
								else{
									setDdRamAddress(0x42 + input_caracters_counter);
								}
								sendChar(' ');
							}
							// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
							else if(input_caracters_counter < 4 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
								if(input_caracters_counter < 2){
									setDdRamAddress(0x41 + input_caracters_counter);
								}
								else{
									setDdRamAddress(0x42 + input_caracters_counter);
								}
								sendChar(tecla);
								guarda_novo_horario[input_caracters_counter] = tecla;
								input_caracters_counter++;
							}
						} while(option_menu_operacional == OPTION_TROCA_HORA);
					
						break;
					
					case OPTION_VERIFICA_CORRESPONDENCIA:
						do{
							sendSerialChar('U'); sendSerialChar('V');
							sendSerialChar(currentTime[HOURS]); sendSerialChar(currentTime[MINUTES]); sendSerialChar(num_eleitores);
							getSerialMessage(consistencia);
							if(consistencia[0] == 'M' && consistencia[1] == 'V'){
								if(consistencia[2] == 'O'){
									verificacao = 3;
									sendString_setAdress("  CONSISTENCIA  ", 1, 1);
									sendString_setAdress("       OK       ", 2, 1);
									delay_ms(1000);
									option_menu_operacional = 0;
									menu_index = 5;
								}
								else if(consistencia[2] == 'I'){
									verificacao++;
									if(verificacao == 3){
										urna_estado = BLOQUEADA;
										sendString_setAdress("  CONSISTENCIA  ", 1, 1);
										sendString_setAdress("     FALHOU     ", 2, 1);
										delay_ms(1000);
										sendString_setAdress("BLOQUEANDO URNA ", 1, 1);
										sendString_setAdress("      ...       ", 2, 1);
										delay_ms(1000);
										option_menu_operacional = 0;
										menu_index = 1;
									}
								
								}
								else if(consistencia[2] == 'X'){
									verificacao = 3;
									urna_estado = BLOQUEADA;
									sendString_setAdress("  CONSISTENCIA  ", 1, 1);
									sendString_setAdress("     FALHOU     ", 2, 1);
									delay_ms(1000);
									sendString_setAdress("BLOQUEANDO URNA ", 1, 1);
									sendString_setAdress("      ...       ", 2, 1);
									delay_ms(1000);
									option_menu_operacional = 0;
									menu_index = 1;
								}
							}
						} while(verificacao < 3);
						break;
					
					case OPTION_RELATORIO_VOTACAO:
						// TO DO
						break;

					case OPTION_TROCA_SENHA:
						input_caracters_counter = 0;
						char guarda_novasenha1_mesario[5] = {0};
						char guarda_novasenha2_mesario[5] = {0};
						char b = 1;
						// primeiro pedido de nova senha
						sendString_setAdress("Nova Senha:     ", 1, 1);
						sendString_setAdress("A:    B:    C:  ", 2, 1);
						setDdRamAddress(0x40 + 2);
						sendChar(RIGHT_OPTION_CHARACTER);
						setDdRamAddress(0x40 + 8);
						sendChar(RETURN_CHARACTER);
						setDdRamAddress(0x40 + 14);
						sendChar(LEFT_ARROW_CHARACTER);

						do{
							tecla = get_tecla();
							// se tecla A e senha com 5 dígitos, prossegue para repetição de senha
							if(tecla == 'A' && input_caracters_counter == 5){
								b = 2;
							}
							// se a tecla for B, volta para "urna bloqueada"
							else if(tecla == 'B'){
								option_menu_operacional = 0;
								b = 3;
							}
							// se a tecla for C, apaga o último caractere (não funcionando)
							else if(tecla == 'C' /*&& input_caracters_counter > 0*/){
								input_caracters_counter--;
								setDdRamAddress(11 + input_caracters_counter);
								sendChar(' ');
							}
							// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
							else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A'){
								setDdRamAddress(11 + input_caracters_counter);
								sendChar('*');
								guarda_novasenha1_mesario[input_caracters_counter] = tecla;
								input_caracters_counter++;
							}
						} while(b == 1);
					
						if(b != 3){
							sendString_setAdress(" Repita a Nova  ", 1, 1);
							sendString_setAdress("    Senha     ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('A');
							tecla = getCharacter_AB();

							// segundo pedido de nova senha
							input_caracters_counter = 0;
							sendString_setAdress("Nova Senha:     ", 1, 1);
							sendString_setAdress("A:    B:    C:  ", 2, 1);
							setDdRamAddress(0x40 + 2);
							sendChar(RIGHT_OPTION_CHARACTER);
							setDdRamAddress(0x40 + 8);
							sendChar(RETURN_CHARACTER);
							setDdRamAddress(0x40 + 14);
							sendChar(LEFT_ARROW_CHARACTER);

							unsigned char a, i;
							do{
								tecla = get_tecla();
								// se tecla A e senha com 5 dígitos, confere senhas
								if(tecla == 'A' && input_caracters_counter == 5){
									a = 0;
									for(i = 0; i < 5; i++){
										if(guarda_novasenha1_mesario[i] == guarda_novasenha2_mesario[i]) {a++;}
									}
									menu_index = OPTION_TROCA_SENHA;
									if(a == 5){
										for(i = 0; i < 5; i++) {guarda_senha_mesario[i] = guarda_novasenha1_mesario[i];}
										sendString_setAdress(" Troca de Senha ", 1, 1);
										sendString_setAdress("    Efetuada  ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
									}
									else{
										sendString_setAdress("   Senhas Nao   ", 1, 1);
										sendString_setAdress("    Conferem  ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
									}
									tecla = getCharacter_B();
									option_menu_operacional = 0;
									b = 3;
								}
								// se a tecla for B, volta para "urna bloqueada"
								else if(tecla == 'B'){
									option_menu_operacional = 0;
									menu_index = OPTION_TROCA_SENHA;
									b = 3;
								}
								// se a tecla for C, apaga o último caractere (não funcionando)
								else if(tecla == 'C' /*&& input_caracters_counter > 0*/){
									input_caracters_counter--;
									setDdRamAddress(11 + input_caracters_counter);
									sendChar(' ');
								}
								// se for um caractere válido e ainda não tiver execedido o limite, é contabilizado
								else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A'){
									setDdRamAddress(11 + input_caracters_counter);
									sendChar('*');
									guarda_novasenha2_mesario[input_caracters_counter] = tecla;
									input_caracters_counter++;
								}
							} while(b == 2);
						}
						else {option_menu_operacional = 0; menu_index = OPTION_TROCA_SENHA;}
						break;
					
					case OPTION_RESET_SENHA:

						break;
				}
			}
		}
	}
}

// Retorna apenas caracteres A ou B
char getCharacter_AB(){
	char tecla = 0;
	while(tecla != 'A' && tecla != 'B'){
		tecla = get_tecla();
		if(tecla == 'A' || tecla == 'B'){
			return tecla;
		}
	}
	return 0;
}

// Retorna apenas caractere B
char getCharacter_B(){
	char tecla = 0;
	do{
		tecla = get_tecla();
	} while(tecla != 'B');
	return 'B';
}

// Valida eleitor
char valida_eleitor(char* eleitor, char* nome_eleitor){
	char message[8] = {'U', 'N', 5};
	char codigo_invalido[] = "Codigo invalido";
	char response[20] = {0};
	
	// Constrói a mensagem a ser enviada serialmente
	unsigned char i;
	for(i = 0; i < 5; i++){
		message[i + 3] = eleitor[i];
	}
	
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

// Processo de votacao ------------------------------------------------------------------------------------------------------------
// ATRIBUIR NOMES E VALORES PARA A STRUCT RELATÓRIO, ATUALIZAR VOTOS E NUMERO DE VOTOS
// IMPLEMENTAR SAÍDA POR TIMEOUT
char votacao(RELATORIO* relatorio){
	char depFederal_numero[5] = {0,0,0,0,0};
	char depFederal_nome[17] = {0};
	char senador_numero[3] = {0,0,0};
	char senador_nome[17] = {0};
	char presidente_numero[3]  = {0,0,'\0'};
	char presidente_nome[17] = {0};
	char tecla = 0;
	unsigned char input_caracters_counter = 0;
	
	// Início do processo de votação
	sendSerialChar('U'); sendSerialChar('I'); 
	char response[50] = {0};
	getSerialMessage(response);
	
	if(response[0] != 'M' || response[1] != 'I'){
		return ERRO_COMUNICACAO;
	}
	
	// Votação Dep. Federal:
	sendString_setAdress("Dep Federal:    ", 1, 1);
	print_optionsVotacao();
	input_caracters_counter = 0;
	cleanString(response);
	do{
		tecla = get_tecla();
		// Se a tecla for A, ou B, vota
		if(tecla == 'A' || tecla == 'B'){
			// Se não votou em branco
			if(tecla == 'A'){
				// Se o usuário digitou 1 ou 3 caracteres, voto nulo
				if(input_caracters_counter == 3 || input_caracters_counter == 1 || input_caracters_counter == 0){
					input_caracters_counter = 2;
					depFederal_numero[0] = '9';
					depFederal_numero[1] = '9';
					depFederal_numero[2] = 0;
					depFederal_numero[3] = 0;
				}
			}
			// Se votou em branco
			else if(tecla == 'B'){ 
				input_caracters_counter = 2;
				depFederal_numero[0] = '0';
				depFederal_numero[1] = '0';
				depFederal_numero[2] = 0;
				depFederal_numero[3] = 0;
			}

			char depFederal_numero_bkp[3];
			// Realiza comunicação serial
			sendSerialChar('U'); sendSerialChar('F');
			sendSerialChar(input_caracters_counter);
			if(input_caracters_counter == 2){
				depFederal_numero_bkp[0] = depFederal_numero[0];
				depFederal_numero_bkp[1] = depFederal_numero[1];
				depFederal_numero_bkp[2] = 0;
				sendSerialMessage(depFederal_numero_bkp);
			}else {
				sendSerialMessage(depFederal_numero);
			}
			getSerialMessage(response);
			
			// Verifica se houve erro na comunicação serial
			if(response[0] != 'M' || response[1] != 'F'){
				return ERRO_COMUNICACAO;
			}
			
			// Obtém nome do candidato através da reposta serial
			unsigned char i, a = strlen(&response[3]);
			for(i = 0; i < a; i++){
				depFederal_nome[i] = response[3 + i];
			}
			
			// Pede cofirmação do usuário
			sendString_setAdress("                ", 1, 1);
			sendString_setAdress(depFederal_nome, 1, 1);
			sendString_setAdress("Confirma?  ", 2, 1);
			sendChar(RIGHT_OPTION_CHARACTER); sendString("A ");
			sendChar(RETURN_CHARACTER); sendChar('B');
			tecla = getCharacter_AB();
			
			// Se usuário não confirmar, volta a tela anterior para votar novamente
			if(tecla == 'B'){
				input_caracters_counter = 0;
				tecla = 0;
				sendString_setAdress("Dep Federal:    ", 1, 1);
				print_optionsVotacao();
				cleanString(response);
			}
			
		}
		// Se a tecla foi C, corrige caractere
		else if(tecla == 'C' && input_caracters_counter > 0){
			input_caracters_counter--;
			depFederal_numero[input_caracters_counter] = 0;
			setDdRamAddress(12 + input_caracters_counter);
			sendChar(' ');
		}
		// Se foi um caractere numérico, o insere
		else if(tecla != '#' && tecla != '*' && tecla != 'D' && tecla != 'B' && tecla != 'C' && tecla != 'A' && input_caracters_counter < 4){
			setDdRamAddress(12 + input_caracters_counter);
			depFederal_numero[input_caracters_counter] = tecla;
			input_caracters_counter++;
			sendChar(tecla);
		}
	} while(tecla != 'A' && tecla != 'B');
	
	
	// Votação senador
	sendString_setAdress("Senador:        ", 1, 1);
	print_optionsVotacao();
	input_caracters_counter = 0;
	cleanString(response);
	do{
		tecla = get_tecla();
		
		if(tecla == 'A' || tecla == 'B'){
			// Se não votou em branco, e o voto é inválido
			if(tecla == 'A' && (input_caracters_counter == 0 || input_caracters_counter == 1)){
				input_caracters_counter = 2;
				senador_numero[0] = '9';
				senador_numero[1] = '9';
			}
			else if(tecla == 'B'){
				input_caracters_counter = 2;
				senador_numero[0] = '0';
				senador_numero[1] = '0';
			}
			
			// Realiza comunicação serial
			sendSerialChar('U'); sendSerialChar('S');
			sendSerialChar(input_caracters_counter);
			sendSerialMessage(senador_numero);
			getSerialMessage(response);
			
			// Verifica se houve erro na comunicação serial
			if(response[0] != 'M' || response[1] != 'S'){
				return ERRO_COMUNICACAO;
			}
			
			// Obtém nome do candidato através da reposta serial
			unsigned char i, a = strlen(&response[3]);
			for(i = 0; i < a; i++){
				senador_nome[i] = response[3 + i];
			}
			
			// Pede cofirmação do usuário
			sendString_setAdress("                ", 1, 1);
			sendString_setAdress(senador_nome, 1, 1);
			sendString_setAdress("Confirma?  ", 2, 1);
			sendChar(RIGHT_OPTION_CHARACTER); sendString("A ");
			sendChar(RETURN_CHARACTER); sendChar('B');
			tecla = getCharacter_AB();
			
			// Se usuário não confirmar, volta a tela anterior para votar novamente
			if(tecla == 'B'){
				input_caracters_counter = 0;
				tecla = 0;
				sendString_setAdress("Senador:        ", 1, 1);
				print_optionsVotacao();
				cleanString(response);
			}
			
		}
		// Se a tecla foi C, corrige caractere
		else if(tecla == 'C' && input_caracters_counter > 0){
			input_caracters_counter--;
			senador_numero[input_caracters_counter] = 0;
			setDdRamAddress(8 + input_caracters_counter);
			sendChar(' ');
		}
		// Se foi um caractere numérico, o insere
		else if(tecla != '#' && tecla != '*' && tecla != 'D' && tecla != 'B' && tecla != 'C' && tecla != 'A' && input_caracters_counter < 2){
			setDdRamAddress(8 + input_caracters_counter);
			senador_numero[input_caracters_counter] = tecla;
			input_caracters_counter++;
			sendChar(tecla);
		}
		
		
	} while(tecla != 'A' && tecla != 'B');
	
	// Votação para presidente
	sendString_setAdress("Presidente:     ", 1, 1);
	print_optionsVotacao();
	input_caracters_counter = 0;
	cleanString(response);
	
	do{
		tecla = get_tecla();
		if(tecla == 'A' || tecla == 'B'){
			// Se não votou em branco, e o voto é inválido
			if(tecla == 'A' && (input_caracters_counter == 0 || input_caracters_counter == 1)){
				input_caracters_counter = 2;
				presidente_numero[0] = '9';
				presidente_numero[1] = '9';
			}
			else if(tecla == 'B'){
				input_caracters_counter = 2;
				presidente_numero[0] = '0';
				presidente_numero[1] = '0';
			}
			
			// Realiza comunicação serial
			
			sendSerialChar('U'); sendSerialChar('P');
			sendSerialChar(input_caracters_counter);
			sendSerialMessage(presidente_numero);
			getSerialMessage(response);
			
			// Verifica se houve erro na comunicação serial
			if(response[0] != 'M' || response[1] != 'P'){
				return ERRO_COMUNICACAO;
			}
			
			// Obtém nome do candidato através da reposta serial
			unsigned char i, a = strlen(&response[3]);
			for(i = 0; i < a; i++){
				presidente_nome[i] = response[3 + i];
			}
			
			// Pede cofirmação do usuário
			sendString_setAdress("                ", 1, 1);
			sendString_setAdress(presidente_nome, 1, 1);
			sendString_setAdress("Confirma?  ", 2, 1);
			sendChar(RIGHT_OPTION_CHARACTER); sendString("A ");
			sendChar(RETURN_CHARACTER); sendChar('B');
			tecla = getCharacter_AB();
			
			// Se usuário não confirmar, volta a tela anterior para votar novamente
			if(tecla == 'B'){
				input_caracters_counter = 0;
				tecla = 0;
				sendString_setAdress("Presidente:     ", 1, 1);
				print_optionsVotacao();
				cleanString(response);
			}
			
		}
		// Se a tecla foi C, corrige caractere
		else if(tecla == 'C' && input_caracters_counter > 0){
			input_caracters_counter--;
			presidente_numero[input_caracters_counter] = 0;
			setDdRamAddress(11 + input_caracters_counter);
			sendChar(' ');
		}
		// Se foi um caractere numérico, o insere
		else if(tecla != '#' && tecla != '*' && tecla != 'D' && tecla != 'B' && tecla != 'C' && tecla != 'A' && input_caracters_counter < 2){
			setDdRamAddress(11 + input_caracters_counter);
			presidente_numero[input_caracters_counter] = tecla;
			input_caracters_counter++;
			sendChar(tecla);
		}		
	}while(tecla != 'A' && tecla != 'B');
	
	// Fim do processo de votação
	cleanString(response);
	sendSerialChar('U'); sendSerialChar('C');
	getSerialMessage(response);
	
	if(response[0] != 'M' || response[1] != 'C'){
		print_erroComunicacao();
		while(1);
		return ERRO_COMUNICACAO;
	}
	sendString_setAdress("        fim     ", 1, 1);
	return VOTACAO_CONCLUIDA;
}

// Imprime na tela msg de erro de comunicação serial
void print_erroComunicacao(){
	sendString_setAdress("ERRO NA COMUNIC.", 1, 1);
	sendString_setAdress("     SERIAL   ", 2, 1);
}

void print_optionsVotacao(){
	sendString_setAdress("A:  B:BRANCO C: ", 2, 1);
	setDdRamAddress(0x40 + 2);
	sendChar(RIGHT_OPTION_CHARACTER);
	setDdRamAddress(0x40 + 14);
	sendChar(LEFT_ARROW_CHARACTER);
}

// Aloca str2 em str1
void strCpy(char* str1, char* str2){
	unsigned char i;
	for(i = 0; i < sizeof(str1); i++){
		str1[i] = str2[2];
	}
}

void cleanString(char* string){
	unsigned char i;
	for(i = 0; i < sizeof(string); i++){
		string[i] = 0;
	}
}
/* 
IDENTIFICAR ELEITOR QUE JA VOTOU EM "valida_eleitor"
JUSTIFICATIVA DE VOTO
CONSIDERAR JUSTIFICAIVA NO RELATORIO
*/


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



// Protótipos
void reset_senha(char*, char*, char*);
char votacao(CANDIDATO*, CANDIDATO*, CANDIDATO*, CANDIDATO*);
char relatorio_votacao(CANDIDATO, CANDIDATO, CANDIDATO, CANDIDATO);

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

	// Eleitores que ja votaram
	//ELEITOR eleitores_que_votaram[33] = {0};		// Dados dos eleitores, considerando, por absurdo, que todos votem
	char num_eleitores_que_votaram[33][6] = {0};
	unsigned char num_eleitores = 0;				// Variável que contém o número de eleitores
	
	// Estruturas utilizadas no relatório
	ELEITOR eleitores_justificativas[33];			// Dados dos eleitores, considerando, por absurdo, que todos justifiquem
	
	unsigned char num_justificativas = 0;			// Número de eleitores que justificaram
	DADOS_CANDIDATO dados_presidente[7];		// 5 presidentes + Branco e Nulo
	DADOS_CANDIDATO dados_senador[5];			// 3 senadores + Branco e Nulo
	DADOS_CANDIDATO dados_depFederal[6];		// 6 dep. federais + Branco e Nulo
	DADOS_CANDIDATO dados_partido[4];			// 4 partidos
		
	// Inicializa candidatos
// 	memset(dados_presidente, 0, sizeof(dados_presidente));
// 	memset(dados_senador, 0, sizeof(dados_senador));
// 	memset(dados_depFederal, 0, sizeof(dados_depFederal));
// 	memset(dados_partido, 0, sizeof(dados_partido);
	CANDIDATO presidente = {dados_presidente, 0};
	CANDIDATO senador = {dados_senador, 0};
	CANDIDATO depFederal = {dados_depFederal, 0};
	CANDIDATO partido = {dados_partido, 0};
		
	// Outras variáveis
	char tecla = 0;						// Guarda tecla pressionada
	char urna_estado = BLOQUEADA;		// Estado da urna
	char menu_index = 1;				// Índice do menu_operacional
	char option_menu_operacional = 0;	// Opção selecionada do menu_operacional
	char numero_eleitor[6] = {0};		// Guarda número do eleitor
	char nome_eleitor[17] = {0};		// Guarda nome do eleitor (max 16 caracteres + \0)
	char guarda_senha_mesario[6] = {0};
	char guarda_login_mesario[6] = {0};
	const char login_mesario[6] = "01237";
	char senha_mesario[6] = "12378";
	unsigned char input_caracters_counter = 0;	// Contador de daracteres inseridos pelo usuário
	char encerrada_bloqueada = 0;				// Guarda status de bloqueio quando a urna está encerrada
	char validacao_relatorio;
	char validacao_eleitor = 0;
	unsigned char eleitor_index;
	char validacao_justificativa;
	while (1){
		
		while(urna_estado == BLOQUEADA){

			switch(menu_index){
				case 1:
					sendString_setAdress(" URNA BLOQUEADA ", 1, 1);
					sendString_setAdress("A:   B:Rst.Senha", 2, 1); setDdRamAddress(0x42); sendChar(LOGIN_CHARACTER);
					tecla = getCharacter_AB();
					// se a tecla apertada for A, espera o login do mesário
					if(tecla == 'A') {menu_index = 2;}
					else {reset_senha(&menu_index, &option_menu_operacional, senha_mesario); }
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
						tecla = get_tecla();
						// se tecla A e login com 5 dígitos, prossegue para senha
						if(tecla == 'A' && input_caracters_counter == 5){
							a = 0;
							for(i = 0; i < 5; i++){
								if(guarda_login_mesario[i] == login_mesario[i] && guarda_senha_mesario[i] == senha_mesario[i])
								{a++;}
							}
							menu_index = 1;
							if(a == 5) {urna_estado = OPERACIONAL;}
							else{
								sendString_setAdress("     Mesario    ", 1, 1);
								sendString_setAdress("    invalido  ", 2, 1); sendChar(RETURN_CHARACTER); sendChar('B');
								tecla = getCharacter_B();
							}
							
						}
						// se a tecla for B, volta para "urna bloqueada"
						else if(tecla == 'B') {menu_index = 1;}
						// se a tecla for C, apaga o último caractere
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
			
			if(currentTime[HOURS] < 8){
				if(urna_estado != BLOQUEADA){
					urna_estado = AGUARDANDO;
				}
			}
			else if(currentTime[HOURS] > 17 || (currentTime[HOURS] == 17 && currentTime[MINUTES] > 14)){
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
					if(tecla == 'B') {menu_index++;}
					else if(tecla == 'A') {option_menu_operacional = OPTION_RESET_SENHA; menu_index = 0;}
					break;
				case 9:
					sendString_setAdress("9. Justificar ", 1, 1); sendChar(RIGHT_OPTION_CHARACTER); sendChar('A');
					sendString_setAdress("      Voto    ", 2, 1); sendChar(DOWN_ARROW_CHARACTER); sendChar('B');
					tecla = getCharacter_AB();
					if(tecla == 'B') {menu_index = 1;}
					else if(tecla == 'A') {option_menu_operacional = OPTION_JUSTIFICA; menu_index = 0;}
				default:
					//menu_index = 1;
					//option_menu_operacional = 0;
					break;
			}
			
			// Opções do menu do sistema
			switch(option_menu_operacional){
				case OPTION_ESTADO:
					ver_estado(&menu_index, &option_menu_operacional, &urna_estado);
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
							numero_eleitor[5] = '\0';
							
							validacao_eleitor = 0;
							// Obtem validação do eleitor
							validacao_eleitor = valida_eleitor(numero_eleitor, nome_eleitor);
							nome_eleitor[16] = '\0';

							unsigned char aux;
							for(aux = 0; aux < num_eleitores; aux++){
								if(num_eleitores_que_votaram[aux][0] == numero_eleitor[0] && num_eleitores_que_votaram[aux][1] == numero_eleitor[1] && num_eleitores_que_votaram[aux][2] == numero_eleitor[2] && num_eleitores_que_votaram[aux][3] == numero_eleitor[3] && num_eleitores_que_votaram[aux][4] == numero_eleitor[4]){
									validacao_eleitor = ELEITOR_VOTOU;
								}
							}
							
							
							// Se o eleitor for valido, exibe no display o nome e aguarda confirmação
							if(validacao_eleitor == ELEITOR_VALIDO){
								sendString_setAdress(nome_eleitor, 1, 1);
								sendString_setAdress("Confirma?  ", 2, 1);
								sendChar(RIGHT_OPTION_CHARACTER); sendChar('A'); sendChar(' ');
								sendChar(RETURN_CHARACTER); sendChar('B');
								tecla = getCharacter_AB();
								
								// Se a tecla for 'A', confirma o eleitor, guarda nome e numero e inicia processo de votacao
								if(tecla == 'A'){					
									// Obtem validacao da votacao
									char validacao_votacao = votacao(&depFederal, &partido, &senador, &presidente);
									
									// Se o tempo de 2 minutos foi esgotado
									if(validacao_votacao == OVERTIME_VOTACAO){
										sendSerialMessage("UT");
										char resp[2];
										resp[0] = getSerialChar();
										resp[1] = getSerialChar();
										if(resp[0] != 'M' || resp[1] != 'T'){
											print_erroComunicacao();
											while(1);
										}
										sendString_setAdress("      TEMPO     ", 1, 1);
										sendString_setAdress("     ESGOTADO   ", 2, 1);
										delay_ms(2000);
										option_menu_operacional = 0;
										menu_index = 2;
										break;
									}
									
									// Se houve erro na comunicação serial no processo de votação
									else if(validacao_votacao == ERRO_COMUNICACAO){
										print_erroComunicacao();
										tecla = getCharacter_B();
										option_menu_operacional = 0;
										menu_index = 2;
										count_segundos_votacao = 0;
										LOW(LED);
										votacao_em_processo = 0;
										votacao_overtime = 0;
										while(1);
									}
									
									// Se a votacao foi concluida com sucesso--------------------------- CONCLUIR AQUI
									else if(validacao_votacao == VOTACAO_CONCLUIDA){
										unsigned char index;
										for(index = 0; index < 5; index++){
											num_eleitores_que_votaram[num_eleitores][index] = numero_eleitor[index];
										}
										num_eleitores++;
										sendString_setAdress("      FIM       ", 1, 1);
										sendString_setAdress("                ", 2, 1);
										count_segundos_votacao = 0;
										LOW(LED);
										votacao_em_processo = 0;
										votacao_overtime = 0;
										votingEndSong(); // 1020 ms
										delay_ms(980);
										option_menu_operacional = 0;
										menu_index = 2;
										break;
									}
								}
								
								// Se a tecla for 'B', volta ao menu anterior
								else if(tecla == 'B'){
									option_menu_operacional = 0;
									menu_index = 2;
								}
							
							}
							
							// Se o eleitor for invalido, informa no display e aguarda retorno ao menu
							else if(validacao_eleitor == ELEITOR_INVALIDO){
								sendString_setAdress("ELEITOR INVALIDO", 1, 1);
								sendString_setAdress("              ", 2, 1);
								sendChar(RETURN_CHARACTER); sendChar('B');
								tecla = getCharacter_B();
								option_menu_operacional = 0;
								menu_index = 2;
							}
							
							else if(validacao_eleitor == ELEITOR_VOTOU){
								sendString_setAdress("ELEITOR JA VOTOU", 1, 1);
								sendString_setAdress("              ", 2, 1);
								sendChar(RETURN_CHARACTER); sendChar('B');
								tecla = getCharacter_B();
								option_menu_operacional = 0;
								menu_index = 2;
							}
							
							// Informa se houve erro na comunicação serial na obtenção do eleitor
							else if(validacao_eleitor == ERRO_COMUNICACAO){
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
					consulta_hora(&menu_index, &option_menu_operacional);
					break;
				
				case OPTION_TROCA_HORA:
					troca_hora(&menu_index, &option_menu_operacional);
					break;
				
				case OPTION_VERIFICA_CORRESPONDENCIA:
					verifica_correspondencia(&menu_index, &option_menu_operacional, &urna_estado, num_eleitores);
					break;
				
				case OPTION_RELATORIO_VOTACAO:
					sendString_setAdress("    ENVIANDO    ", 1, 1);
					sendString_setAdress("      ...       ", 2, 1);
					validacao_relatorio = relatorio_votacao(depFederal, partido, senador, presidente);
					if(validacao_relatorio == RELATORIO_ENVIADO){
						sendString_setAdress("   RELATORIO    ", 1, 1);
						sendString_setAdress("    ENVIADO     ", 2, 1);
						delay_ms(2000);
					}
					else if(validacao_relatorio == ERRO_COMUNICACAO){
						print_erroComunicacao();
						delay_ms(2000);
					}
					option_menu_operacional = 0;
					menu_index = 6;
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
						else if(tecla == 'C' && input_caracters_counter > 0){
							input_caracters_counter--;
							setDdRamAddress(11 + input_caracters_counter);
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
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
									for(i = 0; i < 5; i++) {senha_mesario[i] = guarda_novasenha1_mesario[i];}
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
							else if(tecla == 'C' && input_caracters_counter > 0){
								input_caracters_counter--;
								setDdRamAddress(11 + input_caracters_counter);
								sendChar(' ');
							}
							// se for um caractere válido e ainda não tiver execedido o limite, é contabilizado
							else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
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
					reset_senha(&menu_index, &option_menu_operacional, senha_mesario);
					break;
				case OPTION_JUSTIFICA:
					validacao_justificativa = justifica_voto(&eleitores_justificativas[num_justificativas]);
					
					if(validacao_justificativa == JUSTIFICATIVA_NAO_CONCLUIDA){
						sendString_setAdress(" JUSTIFICATIVA  ", 1, 1);
						sendString_setAdress(" NAO CONCLUIDA  ", 2, 1);
						delay_ms(3000);
						menu_index = 9;
						option_menu_operacional= 0;
					}
					else if(validacao_justificativa == JUSTIFICATIVA_CONCLUIDA){
						sendString_setAdress(" JUSTIFICATIVA  ", 1, 1);
						sendString_setAdress("   CONCLUIDA    ", 2, 1);
						delay_ms(3000);
						sendString_setAdress("                ", 1, 1);
						sendString_setAdress("                ", 2, 1);
						sendString_setAdress(eleitores_justificativas[num_justificativas].nome, 1, 1);
						sendString_setAdress(eleitores_justificativas[num_justificativas].numero, 2, 1);
						setDdRamAddress(0x40+6);
						sendChar(((eleitores_justificativas[num_justificativas].hora_justificativa[HOURS])/10)+48);
						sendChar(((eleitores_justificativas[num_justificativas].hora_justificativa[HOURS])%10)+48);
						sendChar(':');
						sendChar(((eleitores_justificativas[num_justificativas].hora_justificativa[MINUTES])/10)+48);
						sendChar(((eleitores_justificativas[num_justificativas].hora_justificativa[MINUTES])%10)+48);
						delay_ms(3000);
						num_justificativas++;
						
						menu_index = 9;
						option_menu_operacional= 0;
					}
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
					ver_estado(&menu_index, &option_menu_operacional, &urna_estado);
					break;
				case OPTION_ELEITOR:
					break;
				case OPTION_CONSULTA_HORA:
					consulta_hora(&menu_index, &option_menu_operacional);
					break;
				
				case OPTION_TROCA_HORA:
					troca_hora(&menu_index, &option_menu_operacional);
					break;
				
				case OPTION_VERIFICA_CORRESPONDENCIA:
					verifica_correspondencia(&menu_index, &option_menu_operacional, &urna_estado, num_eleitores);
					break;
				
				case OPTION_RELATORIO_VOTACAO:
					sendString_setAdress("    ENVIANDO    ", 1, 1);
					sendString_setAdress("      ...       ", 2, 1);
					validacao_relatorio = relatorio_votacao(depFederal, partido, senador, presidente);
					if(validacao_relatorio == RELATORIO_ENVIADO){
						sendString_setAdress("   RELATORIO    ", 1, 1);
						sendString_setAdress("    ENVIADO     ", 2, 1);
						delay_ms(2000);
					}
					else if(validacao_relatorio == ERRO_COMUNICACAO){
						print_erroComunicacao();
						delay_ms(2000);
					}
					option_menu_operacional = 0;
					menu_index = 6;
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
						else if(tecla == 'C' && input_caracters_counter > 0){
							input_caracters_counter--;
							setDdRamAddress(11 + input_caracters_counter);
							sendChar(' ');
						}
						// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
						else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
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
									for(i = 0; i < 5; i++) {senha_mesario[i] = guarda_novasenha1_mesario[i];}
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
							else if(tecla == 'C' && input_caracters_counter > 0){
								input_caracters_counter--;
								setDdRamAddress(11 + input_caracters_counter);
								sendChar(' ');
							}
							// se for um caractere válido e ainda não tiver execedido o limite, é contabilizado
							else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
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
					reset_senha(&menu_index, &option_menu_operacional, senha_mesario);
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
						sendString_setAdress(" URNA BLOQUEADA ", 1, 1);
						sendString_setAdress("A:   B:Rst.Senha", 2, 1); setDdRamAddress(0x42); sendChar(LOGIN_CHARACTER);
						tecla = getCharacter_AB();
						// se a tecla apertada for A, espera o login do mesário
						if(tecla == 'A') {menu_index = 2; }
						else {reset_senha(&menu_index, &option_menu_operacional, senha_mesario); }
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
									if(guarda_login_mesario[i] == login_mesario[i] && guarda_senha_mesario[i] == senha_mesario[i])
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
						ver_estado(&menu_index, &option_menu_operacional, &urna_estado);
						break;
						
					case OPTION_ELEITOR:
						break;
						
					case OPTION_CONSULTA_HORA:
						consulta_hora(&menu_index, &option_menu_operacional);
						break;
						
					case OPTION_TROCA_HORA:
						troca_hora(&menu_index, &option_menu_operacional);
						break;
					
					case OPTION_VERIFICA_CORRESPONDENCIA:
						verifica_correspondencia(&menu_index, &option_menu_operacional, &urna_estado, num_eleitores);
						break;
					
					case OPTION_RELATORIO_VOTACAO:
						sendString_setAdress("    ENVIANDO    ", 1, 1);
						sendString_setAdress("      ...       ", 2, 1);
						validacao_relatorio = relatorio_votacao(depFederal, partido, senador, presidente);
						if(validacao_relatorio == RELATORIO_ENVIADO){
							sendString_setAdress("   RELATORIO    ", 1, 1);
							sendString_setAdress("    ENVIADO     ", 2, 1);
							delay_ms(2000);
						}
						else if(validacao_relatorio == ERRO_COMUNICACAO){
							print_erroComunicacao();
							delay_ms(2000);
						}
						option_menu_operacional = 0;
						menu_index = 6;
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
							else if(tecla == 'C' && input_caracters_counter > 0){
								input_caracters_counter--;
								setDdRamAddress(11 + input_caracters_counter);
								sendChar(' ');
							}
							// Se for um caractere válido e ainda não tiver execedido o limite, o contabiliza
							else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
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
										for(i = 0; i < 5; i++) {senha_mesario[i] = guarda_novasenha1_mesario[i];}
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
								else if(tecla == 'C' && input_caracters_counter > 0){
									input_caracters_counter--;
									setDdRamAddress(11 + input_caracters_counter);
									sendChar(' ');
								}
								// se for um caractere válido e ainda não tiver execedido o limite, é contabilizado
								else if(input_caracters_counter < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'A' && tecla != 'C'){
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
						reset_senha(&menu_index, &option_menu_operacional, senha_mesario);
						break;
				}
			}
		}
	}
}

void reset_senha(char* menu_index, char* option_menu_operacional, char* senha_mesario){
	senha_mesario[0] = '1';
	senha_mesario[1] = '2';
	senha_mesario[2] = '3';
	senha_mesario[3] = '7';
	senha_mesario[4] = '8';
	*menu_index = 1;
	*option_menu_operacional = 0;
	sendString_setAdress(" SENHA ALTERADA ", 1, 1);
	sendString_setAdress("  PARA PADRAO   ", 2, 1);
	delay_ms(2000);
}

// Processo de votacao
char votacao(CANDIDATO* depFederal, CANDIDATO* partido, CANDIDATO* senador, CANDIDATO* presidente){
	char depFederal_numero[5] = {0,0,0,0,0};
	char depFederal_nome[17] = {0};
	char senador_numero[3] = {0,0,0};
	char senador_nome[17] = {0};
	char presidente_numero[3]  = {0,0,'\0'};
	char presidente_nome[17] = {0};
	char tecla = 0;
	unsigned char input_caracters_counter = 0;
	votacao_em_processo = 1;
	
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
		if(tecla == OVERTIME_VOTACAO){ return OVERTIME_VOTACAO;}
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

			// Realiza comunicação serial
			sendSerialChar('U'); sendSerialChar('F');
			sendSerialChar(input_caracters_counter);
			sendSerialMessage(depFederal_numero);
			getSerialMessage(response);
			
			// Verifica se houve erro na comunicação serial
			if(response[0] != 'M' || response[1] != 'F'){
				return ERRO_COMUNICACAO;
			}
			
			// Obtém nome do candidato através da reposta serial
			unsigned char i, a = strlen(&response[3]);
			for(i = 0; i <= a; i++){
				depFederal_nome[i] = response[3 + i];
			}
			
			// Pede cofirmação do usuário
			sendString_setAdress("                ", 1, 1);
			sendString_setAdress(depFederal_nome, 1, 1);
			sendString_setAdress("Confirma?  ", 2, 1);
			sendChar(RIGHT_OPTION_CHARACTER); sendString("A ");
			sendChar(RETURN_CHARACTER); sendChar('B');
			tecla = getCharacter_AB();
			
			if(tecla == OVERTIME_VOTACAO){ return OVERTIME_VOTACAO;}
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
		
		if(tecla == OVERTIME_VOTACAO){ return OVERTIME_VOTACAO;}
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
			for(i = 0; i <= a; i++){
				senador_nome[i] = response[3 + i];
			}
			
			// Pede cofirmação do usuário
			sendString_setAdress("                ", 1, 1);
			sendString_setAdress(senador_nome, 1, 1);
			sendString_setAdress("Confirma?  ", 2, 1);
			sendChar(RIGHT_OPTION_CHARACTER); sendString("A ");
			sendChar(RETURN_CHARACTER); sendChar('B');
			tecla = getCharacter_AB();
			
			if(tecla == OVERTIME_VOTACAO){ return OVERTIME_VOTACAO;}
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
		
		if(tecla == OVERTIME_VOTACAO){ return OVERTIME_VOTACAO;}
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
			for(i = 0; i <= a; i++){
				presidente_nome[i] = response[3 + i];
			}
			
			// Pede cofirmação do usuário
			sendString_setAdress("                ", 1, 1);
			sendString_setAdress(presidente_nome, 1, 1);
			sendString_setAdress("Confirma?  ", 2, 1);
			sendChar(RIGHT_OPTION_CHARACTER); sendString("A ");
			sendChar(RETURN_CHARACTER); sendChar('B');
			tecla = getCharacter_AB();
			
			if(tecla == OVERTIME_VOTACAO){ return OVERTIME_VOTACAO;}
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
		return ERRO_COMUNICACAO;
	}
	// Contabiliza votos
	if((strlen(depFederal_numero) == 4) || (depFederal_numero[0] == '0' && depFederal_numero[1] == '0') || (depFederal_numero[0] == '9' && depFederal_numero[1] == '9')){
		registraVoto(depFederal, depFederal_numero, depFederal_nome);
	} else{
		registraVoto(partido, depFederal_numero, depFederal_nome);
	}
	registraVoto(senador, senador_numero, senador_nome);
	registraVoto(presidente, presidente_numero, presidente_nome);
	return VOTACAO_CONCLUIDA;
}

char relatorio_votacao(CANDIDATO depFederal, CANDIDATO partido, CANDIDATO senador, CANDIDATO presidente){
	unsigned int n_caracteres = 0;
	char response[3];
	
	if(presidente.num_candidatos_votados == 0){
		char mensagem[] = "SEM VOTOS REGISTRADOS";
		n_caracteres += strlen(mensagem);
		sendSerialChar('U'); sendSerialChar('R'); sendSerialChar(n_caracteres/256); sendSerialChar(n_caracteres%256);
		sendSerialMessage(mensagem);
		getSerialMessage(response);
		sendString_setAdress("Passou !",1,1);
		if(response[0] != 'M' || response[1] != 'R'){
			return ERRO_COMUNICACAO;
		}
		return RELATORIO_ENVIADO;
	}
	n_caracteres += strlen("DEP FEDERAL:\r\n");
	if(depFederal.num_candidatos_votados > 0){
		n_caracteres += conta_bytes_candidato(depFederal, "-Candidato:\r\n");
	}
	if(partido.num_candidatos_votados > 0){
		n_caracteres += conta_bytes_candidato(partido, "-Partido:\r\n");
	}
	n_caracteres += conta_bytes_candidato(senador, "\r\nSENADOR:\r\n");
	n_caracteres += conta_bytes_candidato(presidente, "\r\nPRESIDENTE:\r\n");
	// CONTAR JUSTIFICATIVAS ------------------------------------------------------------------------------------------------------------
	
	sendSerialMessage("UR"); sendSerialChar(n_caracteres/256); sendSerialChar(n_caracteres%256);
	sendSerialMessage("DEP FEDERAL:\r\n"); 
	if(depFederal.num_candidatos_votados > 0){
		sendSerialMessage("-Candidato:\r\n");
		envia_votos_relatorio(depFederal);
	}
	if(partido.num_candidatos_votados > 0){
		sendSerialMessage("-Partido:\r\n");
		envia_votos_relatorio(partido);
	}
	sendSerialMessage("\r\nSENADOR:\r\n");
	envia_votos_relatorio(senador);
	sendSerialMessage("\r\nPRESIDENTE:\r\n");
	envia_votos_relatorio(presidente);
	// ENVIAR JUSTIFICATIVAS ------------------------------------------------------------------------------------------------------------
	
	getSerialMessage(response);
	if(response[0] != 'M' || response[1] != 'R'){
		return ERRO_COMUNICACAO;
	}
	return RELATORIO_ENVIADO;
}


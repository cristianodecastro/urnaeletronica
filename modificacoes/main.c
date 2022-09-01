#define JUSTIFICATIVA_CONCLUIDA 1
#define JUSTIFICATIVA_NAO_CONCLUIDA 2

typedef struct eleitor{
	char numero[6];
	char nome[17];
	unsigned char hora_justificativa[3];
}ELEITOR;

char justifica_voto();
void corrige_string(char*);
char get_proximo_caractere(char);
char get_caractere_anterior(char);
char get_uppercase_lowercase(char);

char justifica_voto(ELEITOR* eleitor){
    // caracteres: ' ', 'A', 'B', ..., 'Z'
    char tecla;
    char posicao_atual = 0;
    char nome[17] = "                ";
    char numero[6] = {0};


    // Imprime na tela e espera confirmação
    sendString_setAdress("A SEGUIR INSIRA ", 1, 1);
    sendString_setAdress("O NUMERO        ", 2, 1);
    setDdRamAddress(0x40 + 11);
    sendChar('A'); sendChar(RIGHT_OPTION_CHARACTER); sendChar(' ');
    sendChar('B'); sendCar(RETURN_CHARACTER);

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
		else if(posicao_atual < 5 && tecla != '*' && tecla != 'D' && tecla != '#' && tecla != 'C'){
			setDdRamAddress(8 + posicao_atual);
			sendChar(tecla);
			numero[posicao_atual] = tecla;
			posicao_atual++;
		}
	} while(tecla != 'A');

	// Verifca se o eleitor já existe
	if(valida_eleitor(eleitor, nome) == ELEITOR_VALIDO){
		return ELEITOR_CADASTRADO;
	}

	cleanString(nome);

	// Usuário insere o nome
    sendString_setAdress("A SEGUIR INSIRA ", 1, 1);
    sendString_setAdress("O NOME          ", 2, 1);
    setDdRamAddress(0x40 + 11);
    sendChar('A'); sendChar(RIGHT_OPTION_CHARACTER); sendChar(' ');
    sendChar('B'); sendCar(RETURN_CHARACTER);

    tecla = getCharacter_AB();
    if(tecla == 'B'){
        return JUSTIFICATIVA_NAO_CONCLUIDA;
    }

    sendString_setAdress("                ", 1, 1);
    setDdRamAddress(0x40);
    sendChar('A'); sendChar(UP_AND_DOWN_CARACTERE); sendString("B C");
    sendChar(LEFT_AND_RIGHT_CARACTERE); sendString("D ");
    sendChar(UPPERCASE_LOWERCASE_CARACTERE); sendString("# ");
    sendChar(RETURN_CHARACTER); sendString("* ");
    sendChar(RIGHT_OPTION_CHARACTER); sendChar('1');

    do{
        tecla == get_tecla();

        if(tecla == 'A'){
            nome[posicao_atual] = get_caractere_anterior(nome[posicao_atual]);
            setDdRamAddress(posicao_atual); sendChar(nome[posicao_atual]);
        }
        else if(tecla == 'B'){
            nome[posicao_atual] = get_proximo_caractere(nome[posicao_atual]);
            setDdRamAddress(posicao_atual); sendChar(nome[posicao_atual]);
        }
        else if(tecla == 'C'){
            posicao_atual--;
        }
        else if(tecla == 'D'){
            posicao_atual++;
        }
        else if(tecla == '#'){
            nome[posicao_atual] = get_uppercase_lowercase(nome[posicao_atual]);
            setDdRamAddress(posicao_atual); sendChar(nome[posicao_atual]);
        }
        else if(tecla == '*'){
            return JUSTIFICATIVA_NAO_CONCLUIDA;
        }
    }while(tecla != '1');

    // Corrige string e guarda informações
    corrige_string(nome);
    strcpy(eleitor->numero, numero);
    strcpy(eleitor->nome, nome);
    eleitor->hora_justificativa[HOURS] = currentTime[HOURS];
    eleitor->hora_justificativa[MINUTES] = currentTime[MINUTES];
    eleitor->hora_justificativa[SECONDS] = currentTime[SECONDS];

    return JUSTIFICATIVA_CONCLUIDA;
}

// Adiciona '\0' caso a string termine com espaços no final
void corrige_string(char* string_sem_fim){
    char i;
    for(i = sizeof(string_sem_fim); i > 0, i--){
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
    else if(caractere == ' '){
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
    else if(caractere == ' '){
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
}
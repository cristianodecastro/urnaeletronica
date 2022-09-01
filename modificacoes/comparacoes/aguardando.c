// MENU AGUARDANDO  -------------------------------------------------------------------------------------------------------
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


// OPTIONS AGUARDANDO -------------------------------------------------------------------------------------------------------
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
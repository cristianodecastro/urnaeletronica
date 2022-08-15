#include "buzzer.h"
#include "pinHandling.h"
#include "timeControl.h"

#include <avr/io.h>

void tonePino(int frequencia, int tempo_ms){
	int meio_periodo_us = (int)(1000000/(2*frequencia));
	int tempo_us = 1000*tempo_ms;
	for(int i=0; i<tempo_us; i+=(2*meio_periodo_us)){
		HIGH(BUZZER);
		delay_microsegundos(meio_periodo_us);
		LOW(BUZZER);
		delay_microsegundos(meio_periodo_us);
	}
}

void votingEndSong(){
	for(char i=0; i<5; i++){
		tonePino(1337, 90);
		tonePino(1437, 90);
	}
	tonePino(1337, 120);
}




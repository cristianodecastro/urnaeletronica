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

void votingEndSong(){
	char j = 0;
	long int i;
	for(j = 0; j < 5; j++){
		for(i=0; i<90000; i+=(2*MEIO_PERIODO_1)){
			HIGH(BUZZER);
			delay_us(MEIO_PERIODO_1);
			LOW(BUZZER);
			delay_us(MEIO_PERIODO_1);
		}
		for(i=0; i<90000; i+=(2*MEIO_PERIODO_2)){
			HIGH(BUZZER);
			delay_us(MEIO_PERIODO_2);
			LOW(BUZZER);
			delay_us(MEIO_PERIODO_2);
		}
	}
	for(i=0; i<120000; i+=(2*MEIO_PERIODO_1)){
		HIGH(BUZZER);
		delay_us(MEIO_PERIODO_1);
		LOW(BUZZER);
		delay_us(MEIO_PERIODO_1);
	}
}




#include "buzzer.h"
#include "pinHandling.h"
#include "timeControl.h"

#include <avr/io.h>

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




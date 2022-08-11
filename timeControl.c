#define HOURS    0
#define MINUTES  1
#define SECONDS  2

#include "timeControl.h"
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char currentTime[3]={0, 0, 0}; // guarda o horário atual em horas, minutos e segundos

void timeControlConfig(){
	TCCR1A = 0b00000000; // modo de contagem contínua até o valor do comparador A
	TCCR1B = 0b00001100; // prescaler = 256 => frequencia do timer = 16MHz / 256 = 62.5Hz => T = 16us
	OCR1A = 62500-1; // teto de contagem; vai contar de 0 até [(pulsos em um segundo) - 1] = contagem de um segundo
	TCNT1 = 0;

	TIMSK1 |= ( 1 << 1 ); // habilita interrupcao por comparador A do timer1
	sei();
}

// redefine o horário se os valores passados nos argumentos forem coerentes
unsigned char setTime(unsigned char hours, unsigned char minutes){
	if((hours < 24) && (minutes < 60)){
		currentTime[HOURS] = hours;
		currentTime[MINUTES] = minutes;
		return 0;
		}else{
		return 1;
	}
}

// incrementa horário em um segundo
void incTime(){
	if(currentTime[SECONDS] == 59){
		currentTime[SECONDS] = 0;
		if(currentTime[MINUTES] == 59){
			currentTime[MINUTES] = 0;
			if(currentTime[HOURS] == 23){
				currentTime[HOURS] = 0;
				}else{
				currentTime[HOURS]++;
			}
			}else{
			currentTime[MINUTES]++;
		}
		}else{
		currentTime[SECONDS]++;
	}
}

ISR(TIMER1_OVF_vect)
{
	incTime();
	TIFR1 = ( 1 << 1 ); // limpa flag de estouro
}
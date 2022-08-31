#include "timeControl.h"
#include "serialFunctions.h"
#include "lcd.h"
#include "pinHandling.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

unsigned char currentTime[3]={0, 0, 0}; // guarda o hor�rio atual em horas, minutos e segundos
char exibindo_hora_display = 0;
char count_ciclos = 0;
char count_segundos_votacao = 0;
char votacao_em_processo = 0;
char votacao_overtime = 0;

// tempo em microssegundos
void delay_us(long int t_us){
	TCCR0A = 0b00000010;
	TCCR0B = 0b00000001;
	OCR0A = 160-1;
	TCNT0 = 0;
	TIFR0 = (1<<1);
	long int count = 0;
	t_us = t_us/10;
	do{
		while(!(TIFR0 & (1 << 1)));
		TIFR0 = (1<<1);
		count+=1;
	} while(count<t_us);
}

void delay_ms(long int t_ms){
	long int count = 0;
	TCCR0A = 0b00000010;
	TCCR0B = 0b00000011;
	OCR0A = 250-1;
	TCNT0 = 0;
	TIFR0 |= (1<<1);
	do{
		while((TIFR0 & (1 << 1)) == 0);
		TIFR0 |= (1<<1);
		count+=1;
	} while(count<t_ms);
}

void timeControlConfig(){
	TCCR1A = 0b00000000; // modo de contagem cont�nua at� o valor do comparador A
	TCCR1B = 0b00001011; // prescaler = 64 => frequencia do timer = 16MHz / 64 = 250kHz => T = 4us
	OCR1A = 62500-1; // teto de contagem; vai contar de 0 at� [(pulsos em 1/4 de segundo) - 1] = contagem de 1/4 de segundo
	TCNT1 = 0;

	TIMSK1 |= ( 1 << 1 ); // habilita interrupcao por comparador A do timer1
	sei();
}

// redefine o hor�rio se os valores passados nos argumentos forem coerentes
unsigned char setTime(unsigned char hours, unsigned char minutes){
	if((hours < 24) && (minutes < 60)){
		currentTime[HOURS] = hours;
		currentTime[MINUTES] = minutes;
		return 0;
	}
	else{
		return 1;
	}
}

// incrementa hor�rio em um segundo
void incTime(){
	if(currentTime[SECONDS] == 59){
		currentTime[SECONDS] = 0;
		if(currentTime[MINUTES] == 59){
			currentTime[MINUTES] = 0;
			if(currentTime[HOURS] == 23){
				currentTime[HOURS] = 0;
			}
			else{
				currentTime[HOURS]++;
			}
		}
		else{
			currentTime[MINUTES]++;
		}
	}
	else{
		currentTime[SECONDS]++;
	}
}

void exibe_hora_display(){
	exibindo_hora_display = 1;
}

void apaga_hora_display(){
	exibindo_hora_display = 0;
}


ISR(TIMER1_COMPA_vect)
{
	if(count_ciclos < 3){
		count_ciclos++;
	}
	else{
		if(votacao_em_processo){
			count_segundos_votacao++;
			if(count_segundos_votacao > 119){
				count_segundos_votacao = 0;
				LOW(LED);
				votacao_em_processo = 0;
				votacao_overtime = 1;
			}
		}
		incTime();
		if(exibindo_hora_display){
			char strTime[9] = {(currentTime[HOURS]/10)+0x30, (currentTime[HOURS]%10)+0x30, ':', (currentTime[MINUTES]/10)+0x30, (currentTime[MINUTES]%10)+0x30, ':', (currentTime[SECONDS]/10)+0x30, (currentTime[SECONDS]%10)+0x30};
			sendString_setAdress(strTime, 2, 1);
		}
		count_ciclos = 0;
	}
	if(count_segundos_votacao > 99){
		TOGGLE(LED);
	}
	
	TIFR1 = ( 1 << 1 ); // limpa flag de estouro
}



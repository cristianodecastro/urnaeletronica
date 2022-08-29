
#include "timeControl.h"
#include "serialFunctions.h"
#include "lcd.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

unsigned char currentTime[3]={0, 0, 0}; // guarda o horário atual em horas, minutos e segundos
char exibindo_hora_display = 0;

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
	}
	else{
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
	incTime();
	if(exibindo_hora_display){
		char strTime[9] = {(currentTime[HOURS]/10)+0x30, (currentTime[HOURS]%10)+0x30, ':', (currentTime[MINUTES]/10)+0x30, (currentTime[MINUTES]%10)+0x30, ':', (currentTime[SECONDS]/10)+0x30, (currentTime[SECONDS]%10)+0x30};
		sendString_setAdress(strTime, 2, 1);
	}
	TIFR1 = ( 1 << 1 ); // limpa flag de estouro
}



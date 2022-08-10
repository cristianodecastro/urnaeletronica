#define HORAS    0
#define MINUTOS  1
#define SEGUNDOS 2

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char horario[3]={0, 0, 0};
	
unsigned char setHorario(unsigned char horas, unsigned char minutos){
	if((horas < 24) && (minutos < 60)){
		horario[HORAS] = horas;
		horario[MINUTOS] = minutos;
		return 0;
	}else{
		return 1;
	}
}

void incHorario(){
	if(horario[SEGUNDOS] == 59){
		horario[SEGUNDOS] = 0;
		if(horario[MINUTOS] == 59){
			horario[MINUTOS] = 0;
			if(horario[HORAS] == 23){
				horario[HORAS] = 0;
			}else{
				horario[HORAS]++;
			}
		}else{
			horario[MINUTOS]++;
		}
	}else{
		horario[SEGUNDOS]++;
	}
}

ISR(TIMER1_OVF_vect)
{
	incHorario();
	TIFR1 = ( 1 << 1 ); // limpa flag de estouro
}

int main(void){
	TCCR1A = 0b00000000; // modo de contagem contínua até o valor do comparador A
	TCCR1B = 0b00001100; // prescaler = 256 => frequencia do timer = 16MHz / 256 = 62.5Hz => T = 16us
	OCR1A = 62500-1; // teto de contagem; vai contar de 0 até [(pulsos em um segundo) - 1] = contagem de um segundo
	TCNT1 = 0;

	TIMSK1 |= ( 1 << 1 ); // habilita interrupcao por comparador A do timer1
	sei();

}
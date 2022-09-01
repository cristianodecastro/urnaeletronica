#ifndef TIMECONTROL_H_
#define TIMECONTROL_H_

#define HOURS    0
#define MINUTES  1
#define SECONDS  2

#define CLK 16000000

#define OVERTIME_VOTACAO 8

unsigned char currentTime[3];
char exibindo_hora_display;
unsigned char count_ciclos;
unsigned char count_segundos_votacao;
unsigned char votacao_em_processo;
char votacao_overtime;

void delay_us(long int);
void delay_ms(long int);
void delay_microsegundos(unsigned int);
void timeControlConfig();
unsigned char setTime(unsigned char, unsigned char);
void incTime();
void exibe_hora_display();
void apaga_hora_display();


#endif /* TIMECONTROL_H_ */
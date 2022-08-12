#ifndef TIMECONTROL_H_
#define TIMECONTROL_H_


unsigned char currentTime[3];

void delay_microsegundos(int);

void timeControlConfig();

unsigned char setHorario(unsigned char horas, unsigned char minutos);

void incHorario();

int ISR(int);


#endif /* TIMECONTROL_H_ */
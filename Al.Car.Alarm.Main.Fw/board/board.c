#include "board.h"

void relay_starter_set_state(byte state)
{
	setbit(DDRA,4,1);
	setbit(PORTA,4,state);
}

void relay_ignition_set_state(byte state)
{
	setbit(DDRA,3,1);
	setbit(PORTA,3,state);
}

void button_start_engine_enable()
{
	setbit(DDRB,4,0);
	setbit(PORTB,4,1);
}

byte button_start_engine_is_pressed()
{
	if (getbit(PINB,4)) {return 0;}
	return 1;
}

void indicator_set_state(byte state)
{
	setbit(DDRB,5,1);
	setbit(PORTB,5,state);
}

void var_delay_ms(int ms)
{
	for (int i=0; i<ms; i++)_delay_ms(1.0);
}
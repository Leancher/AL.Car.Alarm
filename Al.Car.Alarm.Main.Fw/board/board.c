#include "board.h"

void adc_init_voltage_input()
{
	adc_init(0, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
}

void adc_init_voltage_acc()
{
	adc_init(2, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
}

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

//0 - on, 1 - off
void gsm_relay_set_state(byte state)
{
	setbit(DDRC,7,1);
	setbit(PORTC,7,state);
}

void bluetooth_data_in_enable()
{
	setbit(DDRD,4,0);
	setbit(PORTD,4,1);	
}

byte bluetooth_data_in()
{
	if (getbit(PIND,4)) {return 0;}
	return 1;	
}

void var_delay_ms(int ms)
{
	for (int i=0; i<ms; i++)_delay_ms(1.0);
}
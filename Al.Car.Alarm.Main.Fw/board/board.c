#include "board.h"

void set_unused_pin()
{
	setbit(DDRA,0,0);
	setbit(PORTA,0,1);
	setbit(DDRA,5,0);
	setbit(PORTA,5,1);
	setbit(DDRA,6,0);
	setbit(PORTA,6,1);
	setbit(DDRA,7,0);
	setbit(PORTA,7,1);
	
	setbit(DDRB,0,0);
	setbit(PORTB,0,1);
	setbit(DDRB,1,0);
	setbit(PORTB,1,1);
	setbit(DDRB,2,0);
	setbit(PORTB,2,1);
	setbit(DDRB,3,0);
	setbit(PORTB,3,1);
	setbit(DDRB,4,0);
	setbit(PORTB,4,1);
	setbit(DDRB,5,1);
	setbit(PORTB,5,0);
	setbit(DDRB,6,1);
	setbit(PORTB,6,0);
	setbit(DDRB,7,1);
	setbit(PORTB,7,0);
	
	setbit(DDRC,0,0);
	setbit(DDRC,1,0);
	setbit(DDRC,6,0);
	setbit(DDRC,7,0);

	setbit(PORTC,0,1);
	setbit(PORTC,1,1);
	setbit(PORTC,6,1);
	setbit(PORTC,7,1);

	setbit(DDRD,4,0);
	setbit(PORTD,4,1);
	setbit(DDRD,5,0);
	setbit(PORTD,5,1);
	setbit(DDRD,6,1);
	setbit(PORTD,6,0);
	setbit(DDRD,7,1);
	setbit(PORTD,7,0);
}

void adc_init_voltage_input()
{
	adc_init(1, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
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
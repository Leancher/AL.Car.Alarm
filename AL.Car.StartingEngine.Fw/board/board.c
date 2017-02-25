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

void adc_init_voltage_generator()
{
	adc_init(ADC_IN_GENERATOR_VOLTAGE, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
}

void adc_init_vol_power_in()
{
	adc_init(0, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
}

void adc_init_vol_dig_in()
{
	adc_init(1, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
}

void relay_ignition_set_state(byte state)
{
	setbit(DDRB,0,1);
	setbit(PORTB,0,state);
}

void relay_starter_set_state(byte state)
{
	setbit(DDRB,1,1);
	setbit(PORTB,1,state);
}

void relay_starter_acc_state(byte state)
{
	setbit(DDRB,2,1);
	setbit(PORTB,2,state);
}

void board_led_set_state(byte state)
{
	setbit(DDRD,7,1);
	setbit(PORTD,7,state);
}

void board_button_enable()
{
	setbit(DDRD,6,0);
	setbit(PORTD,6,1);
}

byte board_button_is_pressed()
{
	if (getbit(PIND,6)) {return 0;}
	return 1;	
}

void sensor_ignition_key_enable()
{
	setbit(DDRB,5,0);
	setbit(PORTB,5,1);	
}

byte sensor_ignition_key_is_pressed()
{
	if (getbit(PINB,5)) {return 0;}
	return 1;	
}

void var_delay_ms(int ms)
{
	for (int i=0; i<ms; i++)_delay_ms(1.0);
}

void rs485_send_start()
{
	setbit(DDRD,5,1);
	setbit(PORTD,5,1);
}

void rs485_send_end()
{
	setbit(DDRD,5,1);
	setbit(PORTD,5,0);
}
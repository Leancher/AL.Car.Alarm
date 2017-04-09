#include "board.h"

void adc_init_vol_power_in()
{
	adc_init(0, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
}

void adc_init_vol_dig_in()
{
	adc_init(1, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
}

void adc_init_voltage_generator()
{
	adc_init(2, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
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

void sserial_send_start(unsigned char portindex)
{
	if (portindex==UART_485)	{
		DDRD|=(1<<5);
		PORTD|=(1<<5);
	}
}
void sserial_send_end(unsigned char portindex)
{
	if (portindex==UART_485)	{
		DDRD|=(1<<5);
		PORTD&=(~(1<<5));
	}
}
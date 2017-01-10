#include "board.h"

void ir_led_set(char state)
{
	setbit(DDRC,3,1);
	setbit(PORTC,3,state);
}

void ir_modulation_delay()
{
	_delay_us(10.5);
}

void board_led_set (byte state)
{
	setbit(DDRC,5,1);
	setbit(PORTC,5,state);
}

void board_button_enable()
{
	setbit(DDRC,6,0);
	setbit(PORTC,6,1);
}

byte board_button_get()
{
	if (getbit(PINC,6)) {return 0;}
	return 1;
}

void set_ow_pin(byte mode, byte state)
{
	if (state){PORTJ|=1<<2;	}else{PORTJ&=(~(1<<2));}
	if (mode){DDRJ|=1<<2;	}else{DDRJ&=(~(1<<2));}
}

unsigned char get_ow_pin()
{
	return ((PINJ&(1<<2)));
}

void var_delay_ms(int ms)
{
	for (int i=0; i<ms; i++)_delay_ms(1.0);
}

char gsm_statusled_get()
{
	setbit(DDRH,4,0);
	setbit(PORTH,4,0);
	var_delay_ms(1);
	return getbit(PINH,4);
}

void gsm_powerkey()
{
	setbit(DDRH,2,1);
	setbit(PORTH,2,0);
	var_delay_ms(1000);
	setbit(DDRH,2,0);
	setbit(PORTH,2,0);
}

void watchdog_power_on()
{
	wdt_reset();
	setbit(DDRB,7,1);
	setbit(PORTB,7,1);
	_delay_ms(300);
	setbit(PORTB,7,0);
	wdt_reset();
}

void board_init()
{

}

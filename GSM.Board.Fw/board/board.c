#include "board.h"

void set_unused_pin()
{
	setbit(DDRA,0,0);
	setbit(DDRA,1,0);
	setbit(DDRA,2,0);
	setbit(DDRA,3,0);
	setbit(DDRA,4,0);
	setbit(DDRA,5,0);
	setbit(DDRA,6,0);
	setbit(DDRA,7,0);

	setbit(PORTA,0,1);
	setbit(PORTA,1,1);
	setbit(PORTA,2,1);
	setbit(PORTA,3,1);
	setbit(PORTA,4,1);
	setbit(PORTA,5,1);
	setbit(PORTA,6,1);
	setbit(PORTA,7,1);

	setbit(DDRB,0,0);
	setbit(DDRB,1,0);
	setbit(DDRB,2,0);
	setbit(DDRB,3,0);
	setbit(DDRB,4,0);
	setbit(DDRB,5,0);
	setbit(DDRB,6,0);
	setbit(DDRB,7,0);

	setbit(PORTB,0,1);
	setbit(PORTB,1,1);
	setbit(PORTB,2,1);
	setbit(PORTB,3,1);
	setbit(PORTB,4,1);
	setbit(PORTB,5,1);
	setbit(PORTB,6,1);
	setbit(PORTB,7,1);

	setbit(DDRC,0,0);
	setbit(DDRC,1,0);
	setbit(DDRC,2,0);
	setbit(DDRC,3,0);
	setbit(DDRC,7,0);

	setbit(PORTC,0,1);
	setbit(PORTC,1,1);
	setbit(PORTC,2,1);
	setbit(PORTC,3,1);
	setbit(PORTC,7,1);

	setbit(DDRD,0,0);
	setbit(DDRD,1,0);
	setbit(DDRD,4,0);
	setbit(DDRD,5,0);
	setbit(DDRD,6,0);
	setbit(DDRD,7,0);

	setbit(PORTD,0,1);
	setbit(PORTD,1,1);
	setbit(PORTD,4,1);
	setbit(PORTD,5,1);
	setbit(PORTD,6,1);
	setbit(PORTD,7,1);

	setbit(DDRE,2,0);
	setbit(DDRE,3,0);
	setbit(DDRE,4,0);
	setbit(DDRE,5,0);
	setbit(DDRE,6,0);
	setbit(DDRE,7,0);

	setbit(PORTE,2,1);
	setbit(PORTE,3,1);
	setbit(PORTE,4,1);
	setbit(PORTE,5,1);
	setbit(PORTE,6,1);
	setbit(PORTE,7,1);

	setbit(DDRG,0,0);
	setbit(DDRG,1,0);
	setbit(DDRG,2,0);
	setbit(DDRG,3,0);
	setbit(DDRG,4,0);
	setbit(DDRG,5,0);

	setbit(PORTG,0,1);
	setbit(PORTG,1,1);
	setbit(PORTG,2,1);
	setbit(PORTG,3,1);
	setbit(PORTG,4,1);
	setbit(PORTG,5,1);

	setbit(DDRJ,0,0);
	setbit(DDRJ,1,0);
	setbit(DDRJ,3,0);
	setbit(DDRJ,4,0);
	setbit(DDRJ,5,0);
	setbit(DDRJ,6,0);
	setbit(DDRJ,7,0);

	setbit(PORTJ,0,1);
	setbit(PORTJ,1,1);
	setbit(PORTJ,3,1);
	setbit(PORTJ,4,1);
	setbit(PORTJ,5,1);
	setbit(PORTJ,6,1);
	setbit(PORTJ,7,1);

	setbit(DDRK,0,0);
	setbit(DDRK,1,0);
	setbit(DDRK,2,0);
	setbit(DDRK,3,0);
	setbit(DDRK,4,0);
	setbit(DDRK,5,0);
	setbit(DDRK,6,0);
	setbit(DDRK,7,0);

	setbit(PORTK,0,1);
	setbit(PORTK,1,1);
	setbit(PORTK,2,1);
	setbit(PORTK,3,1);
	setbit(PORTK,4,1);
	setbit(PORTK,5,1);
	setbit(PORTK,6,1);
	setbit(PORTK,7,1);

	setbit(DDRL,0,0);
	setbit(DDRL,1,0);
	setbit(DDRL,2,0);
	setbit(DDRL,3,0);
	setbit(DDRL,4,0);
	setbit(DDRL,5,0);
	setbit(DDRL,6,0);
	setbit(DDRL,7,0);

	setbit(PORTL,0,1);
	setbit(PORTL,1,1);
	setbit(PORTL,2,1);
	setbit(PORTL,3,1);
	setbit(PORTL,4,1);
	setbit(PORTL,5,1);
	setbit(PORTL,6,1);
	setbit(PORTL,7,1);
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

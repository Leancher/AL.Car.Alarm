#ifndef BOARD_H
#define BOARD_H

#define F_CPU 8000000UL

#define UART_485 0

#define UART_GSM 2
#define UART_GPS 3

#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <string.h>

#define getbit(port, bit)		((port) &   (1 << (bit)))
#define setbit(port,bit,val)	{if ((val)) {(port)|= (1 << (bit));} else {(port) &= ~(1 << (bit));}}

typedef unsigned char byte;
void set_unused_pin();
void board_led_set (byte _state);
void board_button_enable();
byte board_button_get();
void var_delay_ms(int ms);

#endif /* BOARD_H_ */
#ifndef BOARD_H
#define BOARD_H

#define F_CPU 8000000UL

#define UART_485 1

#define UART_GSM 2
#define UART_GPS 3

#define ALARM_CH1_LEVEL 2.8
#define ALARM_CH2_LEVEL 4.8
#define ALARM_CH3_LEVEL 10.1
#define ALARM_CH4_LEVEL 4.7
#define ALARM_CH5_LEVEL 7.5

#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <string.h>

#define getbit(port, bit)		((port) &   (1 << (bit)))
#define setbit(port,bit,val)	{if ((val)) {(port)|= (1 << (bit));} else {(port) &= ~(1 << (bit));}}

typedef unsigned char byte;

void board_led_set (byte _state);
void board_button_enable();
byte board_button_get();
void var_delay_ms(int ms);

#endif /* BOARD_H_ */
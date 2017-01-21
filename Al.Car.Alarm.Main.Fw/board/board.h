#ifndef BOARD_H_
#define BOARD_H_

#define F_CPU 8000000UL
#define BAUD 9600

//Atmel Libraries
#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <util/setbaud.h>

#define UART_USB 0
#define UART_485 1

#define getbit(port, bit)		((port) &   (1 << (bit)))
#define setbit(port,bit,val)	{if ((val)) {(port)|= (1 << (bit));} else {(port) &= ~(1 << (bit));}}

//User Libraries
#include "../libs/bwl_uart.h"
#include "../libs/bwl_adc.h"
#include "../libs/bwl_strings.h"
#include "../libs/bwl_simplserial.h"

typedef unsigned char byte;
void var_delay_ms(int ms);

void set_unused_pin();

void adc_init_voltage_input();
void adc_init_voltage_acc();

void relay_starter_set_state(byte state);
void relay_ignition_set_state(byte state);
void button_start_engine_enable();
byte button_start_engine_is_pressed();
void indicator_set_state(byte state);

void gsm_relay_set_state(byte state);

void bluetooth_data_in_enable();
byte bluetooth_data_in();

#endif

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

#define UART_BT 0
#define UART_485 1

#define getbit(port, bit)		((port) &   (1 << (bit)))
#define setbit(port,bit,val)	{if ((val)) {(port)|= (1 << (bit));} else {(port) &= ~(1 << (bit));}}

//User Libraries
#include "../../refs-avr/bwl_uart.h"
#include "../../refs-avr/bwl_adc.h"
#include "../../refs-avr/bwl_strings.h"
#include "../../refs-avr/bwl_simplserial.h"

typedef unsigned char byte;
void var_delay_ms(int ms);

void set_unused_pin();
void adc_init_voltage_generator();
void adc_init_vol_power_in();
void adc_init_vol_dig_in();

void relay_ignition_set_state(byte state);
void relay_starter_set_state(byte state);
void relay_starter_acc_state(byte state);

void sensor_ignition_key_enable();
byte sensor_ignition_key_is_pressed();

void board_button_enable();
byte board_button_is_pressed();
void board_led_set_state(byte state);

void sserial_send_start(unsigned char portindex);
void sserial_send_end(unsigned char portindex);

#endif

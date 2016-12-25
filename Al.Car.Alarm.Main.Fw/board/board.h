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


//User Libraries
#include "../libs/bwl_uart.h"
#include "../libs/bwl_adc.h"
#include "../libs/bwl_strings.h"
#include "../libs/bwl_tools.h"

typedef unsigned char byte;
void var_delay_ms(int ms);

void relay_starter_set_state(byte state);
void relay_ignition_set_state(byte state);
void button_start_engine_enable();
byte button_start_engine_is_pressed();
void indicator_set_state(byte state);

#endif

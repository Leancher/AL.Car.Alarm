#ifndef BOARD_H_
#define BOARD_H_

#define F_CPU 8000000UL

//Atmel Libraries
#include <util/delay.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <string.h>

//User Libraries
#include "../libs/bwl_uart.h"
#include "../libs/bwl_adc.h"
#include "../libs/bwl_strings.h"
#include "../libs/bwl_tools.h"


typedef unsigned char byte;
void var_delay_ms(int ms);

#endif

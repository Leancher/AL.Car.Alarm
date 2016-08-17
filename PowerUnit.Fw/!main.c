/*
* al.c
*
* Created: 13.08.2013 11:16:48
*  Author: Андрей
*/

#include <avr/io.h>

# define F_CPU 16000000UL
#include <util/delay.h>
#include <stdlib.h>


#include "refs/bwl_gap_lib.h"
#include "refs/bwl_uart.h"
#include "lib_soft_uart.c"
#include "hal_gap.c"
#include "work.c"

int main ()
{
	
	uart_init_withbaud(16000.0,9600.0);
	uart_send_string("Power block init.\r\n");
	relayPortInit();
	while (1)
	{
		startWork();
	}
}
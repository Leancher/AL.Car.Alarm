/*
* AL.Car.Alarm.MainBlock.Firmware
*
* Created: 13.08.2013 11:16:48
*  Author: Андрей
*/

#include <avr/io.h>
//Переменные, которые не изменяются при сбросе МК встроенным вотчдогом.
#define noinit __attribute__((section (".noinit")))
int stateLED noinit;
int stateAlarm noinit;
int presenceDriver noinit;
int cycleAlarm noinit;

# define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/wdt.h>
#include <stdlib.h>

#include "rfm69/spi.h"
#include "rfm69/rfm69.h"

#include "refs/bwl_uart.h"
#include "refs/lib_soft_uart.c"
#include "hal_gap.c"
#include "relayControl.c"
#include "work.c"

int main ()
{
	wdt_enable(WDTO_8S);
	setStateIndicator(1);
	uart_init_withbaud(16000.0,9600.0);
	uart_send_string ("Device init.\n\r");
	
	uart_send_only_int (stateAlarm);
	
	rfm69_normal_init();
	rfm69_set_params(4,10,4,31);
	rfm69_set_syntfreq(869.05*rfm69_freq_koeff);
	
	rfpacket_params.address=0;
	rfpacket_params.promiscuos=1;
	
	//Определением порта для использования программного УАРТ
 	uart_software_init();
	
	while (1)
	{
		startWork();
	}
}
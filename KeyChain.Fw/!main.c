/*
 * Cf.cpp
 *
 * Created: 04.10.2014 10:37:59
 *  Author: Andrew
 */ 

#include <avr/io.h>

# define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/wdt.h>
#include "rfm69/spi.h"
#include "rfm69/rfm69.h"

#include "libs/bwl_gap_lib.h"
#include "libs/bwl_uart.h"
#include "hal_gap.c"
#include "work.c"

int main ()
{
	wdt_enable(WDTO_8S);
	rfm69_normal_init();
	rfm69_set_params(4,10,4,31);
	rfm69_set_syntfreq(869.05*rfm69_freq_koeff);
	
	rfpacket_params.address=0;
	rfpacket_params.promiscuos=1;
	
	//setStateLED(1);
	while (1)
	{
		work();
	}
}

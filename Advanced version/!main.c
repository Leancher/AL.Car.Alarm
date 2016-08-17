/*
* AL.Car.Alarm.MainBlock.Firmware
*
* Created: 13.08.2013 11:16:48
*  Author: Андрей
*/

#include <avr/io.h>

#define noinit __attribute__((section (".noinit")))
int stateLED noinit;

# define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/wdt.h>
#include <stdlib.h>

int stateRelay=0;

#include "rfm69/spi.h"
#include "rfm69/rfm69.h"

#include "refs/bwl_gap_lib.h"
#include "refs/bwl_uart.h"
// #include "refs/lib_soft_uart.c"
#include "hal_gap.c"
// 
// #include "relayControl.c"
// #include "RPM.c"
// #include "work.c"



void rfm69_send_data(byte data)
{
	rfpacket_response.data_length=2;
	rfpacket_response.response=data;
	rfpacket_response.data[0]=1;
	rfpacket_response.data[1]=2;
	rfpacket_send();
}

int getData()
{
	int data=0;
	do 
	{
		if (rfpacket_receive())
		{	
			if (rfpacket_request.command!=99) data=rfpacket_request.command; 
			//uart_send_int("Data: ",data);
			_delay_ms(50);
			rfm69_send_data(99);
		}		
	} while (rfpacket_request.command!=99);

	return data;
}

int main ()
{
	setStateLED(stateLED);
	wdt_enable(WDTO_4S);
	uart_init_withbaud(16000.0,9600.0);
	//uart_send_string("Power block init.\r\n");
	
	rfm69_normal_init();
	rfm69_set_params(4,10,4,31);
	rfm69_set_syntfreq(869.05*rfm69_freq_koeff);
	
	rfpacket_params.address=0;
	rfpacket_params.promiscuos=1;
	
	//Определением порта для использования программного УАРТ
// 	uart_software_init();
	//stateLED = 0;
	
	while (1)
	{
		//wdt_reset();
		int data=getData();
		if (data!=0) uart_send_int("data",data);

		if (data==10) stateLED = 1;
		if (data==20) stateLED = 0;
		setStateLED(stateLED);
	}
}
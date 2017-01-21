#include "../refs-avr/sim900.h"
#include "../refs-avr/bwl_simplserial.h"

byte		gsm_present=0;
byte		gsm_connected=0;

void sserial_process_gsm()
{
	if (sserial_request.command==60)
	{
		sserial_response.result=128+sserial_request.command;
		
		sserial_response.data[0]=gsm_present;
		sserial_response.data[1]=gsm_connected;

		//sserial_response.data[10]=watchdog_sec_to_reset>>8;
		//sserial_response.data[11]=watchdog_sec_to_reset&255;
		
		sserial_response.datalength=30;
		sserial_send_response();
	}
	
	if (sserial_request.command==39)
	{
		sserial_response.result=128+sserial_request.command;
		sserial_send_response();
	}
}

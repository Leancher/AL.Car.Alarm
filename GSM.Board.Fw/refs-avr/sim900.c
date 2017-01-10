#include "SIM900.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <string.h>
#include "bwl_strings.h"

unsigned char gsm_uart_receive_buffer_pos=0;
unsigned char gsm_last_line_lenght = 0;
char gsm_uart_receive_buffer[GSM_RECEIVED_BUFFER_LENGTH]={};
char gsm_working=0;
char gsm_error_code=GSM_ERROR_NONE;

void gsm_uart_send_string(char *string)
{
	unsigned	char  i=0;
	while (string[i]>0)
	{
		uart_send(UART_GSM,string[i]);
		i++;
	}
}

void gsm_uart_send_line(char *string)
{
	gsm_uart_send_string(string);
	gsm_uart_send_string("\r\n");
}

char gsm_uart_receive_line()
{
	if (uart_received(UART_GSM))
	{
		unsigned char byte=uart_get(UART_GSM);
		if ((byte==13)||(byte==10))
		{
			if (gsm_uart_receive_buffer_pos>0)
			{
				gsm_uart_receive_buffer[gsm_uart_receive_buffer_pos++]=0;
				gsm_last_line_lenght = gsm_uart_receive_buffer_pos;
				gsm_uart_receive_buffer_pos=0;
				return 1;
			}
		}else
		{
			if (gsm_uart_receive_buffer_pos<GSM_RECEIVED_BUFFER_LENGTH)
			{
				gsm_uart_receive_buffer[gsm_uart_receive_buffer_pos++]=byte;
			}
		}
	}
	return 0;
}

char gsm_uart_wait_line(int time_ms)
{
	long timer_limit=(long)time_ms*100;
	long timer=0;
	do
	{
		if (gsm_uart_receive_line()>0)
		{
			return 1;
		}
		timer+=1;
	}while(timer<timer_limit);
	return 0;
}

void gsm_powerkey_on()
{
	if (gsm_statusled_get()==0)	{gsm_powerkey();}
	if (gsm_statusled_get()==0)	{gsm_powerkey();}
	if (gsm_statusled_get()==0)	{gsm_powerkey();}
}

void gsm_powerkey_off()
{
	if (gsm_statusled_get()>0)	{gsm_powerkey();}
	if (gsm_statusled_get()>0)	{gsm_powerkey();}
	if (gsm_statusled_get()>0)	{gsm_powerkey();}
}

char gsm_send_wait(char *line, char* wait, int wait_ms, char repeats)
{
	for (char i=0; i<repeats; i++)
	{
		wdt_reset();
		gsm_uart_send_line(line);
		if (gsm_uart_wait_line(wait_ms))
		{
			if (strstr(gsm_uart_receive_buffer,wait)>0){return 1;}
		}
		if (gsm_uart_wait_line(wait_ms))
		{
			if (strstr(gsm_uart_receive_buffer,wait)>0){return 1;}
		}		
	}
	return 0;
}
char gsm_send_wait_ok(char *line, int wait_ms, char repeats)
{
	return gsm_send_wait(line,"OK",wait_ms,repeats);
}

void gsm_init()
{
	gsm_working=0;
	gsm_error_code=GSM_ERROR_NONE;
	gsm_powerkey_off();wdt_reset();
	var_delay_ms(500);
	gsm_powerkey_on();wdt_reset();

	if (gsm_send_wait_ok("AT"				,200,9)==0){gsm_error_code=GSM_ERROR_NOT_RESPOND;		return;}	
	if (gsm_send_wait_ok("ATE0"				,300,3)==0){gsm_error_code=GSM_ERROR_NOT_CONFIGURED;	return;}

	if (gsm_send_wait	("ATI","SIM900"		,500,3)==0){gsm_error_code=GSM_ERROR_WRONG_MODULE;	return;}
	
	if (gsm_send_wait_ok("AT+CMGF=1"		,500,3)==0){gsm_error_code=GSM_ERROR_NOT_CONFIGURED;	return;}
	if (gsm_send_wait_ok("AT+CTZU=1"		,500,3)==0){gsm_error_code=GSM_ERROR_NOT_CONFIGURED;	return;}
	if (gsm_send_wait_ok("AT+IFC=1, 1"		,500,3)==0){gsm_error_code=GSM_ERROR_NOT_CONFIGURED;	return;}
	if (gsm_send_wait_ok("AT+CPBS=\"SM\""	,500,3)==0){gsm_error_code=GSM_ERROR_NOT_CONFIGURED;	return;}
	if (gsm_send_wait_ok("AT+CNMI=1,2,2,1,0",500,3)==0){gsm_error_code=GSM_ERROR_NOT_CONFIGURED;	return;}
	gsm_error_code=GSM_ERROR_NONE;
}

void gsm_checkstate()
{
	for (byte i=0; i<20; i++)
	{
		wdt_reset();
		if (gsm_send_wait("AT+CREG?","+CREG: 0,1",500,1)!=0)
		{
			gsm_error_code=GSM_ERROR_NONE;
			gsm_working=1;
			return;		
		}
		var_delay_ms(500);
	}
	gsm_error_code=GSM_ERROR_NOT_OPERATIONAL;
	gsm_working=0;	
}

void gsm_poll()
{
	if (gsm_uart_receive_line())
	{
		if (strstr(gsm_uart_receive_buffer,"RING")>0)
		{
			gsm_uart_send_line("ATA");
		}
		
		if (strstr(gsm_uart_receive_buffer,"+CMT")>0)
		{
			for (byte i=0; i<12; i++)
			{
				gsm_received_sms_phone[i]=gsm_uart_receive_buffer[i+7];
			}
			gsm_received_sms_phone[12]=0;
			char line=gsm_uart_wait_line(100);
			if (line)
			{
				for (byte i=0; i<64; i++)
				{
					gsm_received_sms_text[i]=gsm_uart_receive_buffer[i];
				}
				gsm_received_sms();
			}
		}else
		{
			//Debug line
			//uart_send_line(UART_USB,uart_receive_buffer_2);
		}
	}
}

// char gsm_check_gprs_state()
// {
// 	gsm_uart_send_line("AT+CGATT?");
// 	gsm_uart_wait_line(1000);
// 	wdt_reset();
// 	if(gsm_uart_receive_buffer[8]==49)return 1;
// 	return 1;
// }

void gsm_send_sms(char* phone,char* message)
{
	gsm_checkstate();
	var_delay_ms(100);
	gsm_uart_send_string("AT+CMGS=\"");
	gsm_uart_send_string(phone);
	gsm_uart_send_string("\"");
	gsm_uart_send_string("\r\n");
	var_delay_ms(500);
	gsm_uart_send_string(message);
	var_delay_ms(100);
	uart_send(UART_GSM,26);
	gsm_checkstate();
}

// void gsm_get_time_line(char *buffer)
// {	
// 	gsm_checkstate();
// 	var_delay_ms(100);
// 	gsm_send_wait("AT+CCLK?","+CCLK:",500,3);
// 	char data[gsm_last_line_lenght-5];
// 	for(unsigned char i = 0;i<29-6;i++){
// 		*buffer++ = gsm_uart_receive_buffer[i+6];
// 	}
// }

// char gsm_gprs_connect()
// {
// 	return 1;		
// }

char gsm_http_get_request(char *url)
{
	char data = 0;	
	if(gsm_check_gprs_state()){ //TODO check state!!!!!

		gsm_send_wait_ok("AT+CIPATS=0",1500, 1);
		gsm_send_wait_ok("at+cipcsgp=1,\"internet\",\"megafon\",\"megafon\"",1500,1);
		gsm_send_wait("AT+CIPSTART=\"TCP\",\"194.87.144.245\",\"8795\"","CONNECT OK", 3500, 1);	
		gsm_uart_send_line("AT+CIPSEND");	
		var_delay_ms(1000);
		gsm_uart_send_string(url);
		gsm_uart_send_string("\n");
		var_delay_ms(100);
		uart_send(UART_GSM,26);	
		gsm_send_wait_ok("AT+CIPCLOSE=1", 1500, 1);
		//gsm_send_wait_ok("AT+CIPSHUT",1500, 1);
	}
	
	return data;
}

void gsm_get_location()
{
	gsm_send_wait_ok("AT+SAPBR=3,1,\"Contype\",\"GPRS\"",5500, 1);
	wdt_reset();
	gsm_send_wait_ok("AT+SAPBR=3,1,\"APN\",\"Internet\"",5500, 1);
	wdt_reset();
	gsm_send_wait_ok("AT+SAPBR=1,1",5500, 1);
	wdt_reset();
	gsm_send_wait("AT+CIPGSMLOC=1,1","+CIPGSMLOC",6000,3);
	wdt_reset();
	gsm_send_wait_ok("AT+SAPBR=0,1",5500, 1);
	wdt_reset();
}




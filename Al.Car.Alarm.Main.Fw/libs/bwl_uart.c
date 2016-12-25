/*
 * Bwl UART lib for AVR: mega48/88/168/328/324/164/2560/1280
 *
 * Author: Igor Koshelev and others
 * Licensed: open-source Apache license
 *
 * Version: 01.07.2016
 */ 

#include <avr/io.h>
#include <stdlib.h>

#include "bwl_uart.h"

char uart_send_buffer[16]={};

void uart_init_withdivider(unsigned char port, unsigned int ubrr)
{
	if (port==0)
	{
		UBRR0H = (unsigned char)(ubrr>>8);
		UBRR0L = (unsigned char)ubrr;
		UCSR0A = (0<<U2X0);
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);
		UCSR0C = (0<<USBS0)|(3<<UCSZ00);
	}
	#ifdef UDR1
	if (port==1)
	{
		UBRR1H = (unsigned char)(ubrr>>8);
		UBRR1L = (unsigned char)ubrr;
		UCSR1A = (0<<U2X1);
		UCSR1B = (1<<RXEN1)|(1<<TXEN1);
		UCSR1C = (1<<USBS1)|(3<<UCSZ10);
	}	
	#endif
}

void uart_disable(unsigned char port)
{
	if (port==0){UCSR0B = (0<<RXEN0)|(0<<TXEN0);}
	#ifdef UDR1
	if (port==1){UCSR1B = (0<<RXEN1)|(0<<TXEN1);}	
	#endif
}

void uart_send(const unsigned char port, unsigned char byte )
{
	if (port==0)
	{	
		while ( !( UCSR0A & (1<<UDRE0)) );
		UDR0 = byte;
	}
	#ifdef UDR1
	if (port==1)
	{
		while ( !( UCSR1A & (1<<UDRE1)) );
		UDR1 = byte;
	}
	#endif
}

 unsigned char uart_received(unsigned char port)
{
	if (port==0){	return (UCSR0A & (1<<RXC0));}
	#ifdef UDR1
	if (port==1){	return (UCSR1A & (1<<RXC1));}
	#endif
	return 0;
}

unsigned char uart_get(unsigned char port)
{
	if (port==0){	return UDR0;}
	#ifdef UDR1
	if (port==1){	return UDR1;}
	#endif
	return 0;
}

void uart_send_string (unsigned char port,char *string)
{
	unsigned	char  i=0;
	while (string[i]>0)
	{
		uart_send(port,string[i]);
		i++;
	}
}

void uart_send_int(unsigned char port,int val)
{
	itoa(val,uart_send_buffer,10);
	uart_send_string(port,uart_send_buffer);
}

void uart_send_float(unsigned char port,float val, char precision)
{
	char* buffer="1234567890";
	dtostrf(val,1,precision,buffer);
	uart_send_string(port,buffer);
}

void uart_send_line(unsigned char port,char *string)
{
	uart_send_string(port,string);
	uart_send_string(port,"\r\n");
}
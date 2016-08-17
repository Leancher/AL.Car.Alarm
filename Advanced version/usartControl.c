/*
 * USART_ctrl_mX.c
 * UART init. and control fo Atmega 48
 * Created: 13.06.2013 11:59:15
 *  Author: Андрей
 */ 
	
	#include <stdio.h>

	void USART_Init( unsigned int ubrr){
		UBRR0H = (unsigned char)(ubrr>>8);			//Set baud rate
		UBRR0L = (unsigned char)ubrr;
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);				//Enable receiver and transmitter
		UCSR0C = (1<<USBS0)|(3<<UCSZ00);			// Set frame format: 8data, 2stop bit
	}
	
	void USART_Trans (unsigned char data){
		while (!(UCSR0A & (1<<UDRE0)));				//Wait for empty transmit buffer
		UDR0 = data;								//Put data into buffer, sends the data
	}
		
	unsigned char USART_Rec (void){
		while (!(UCSR0A & (1<<RXC0)));			//Wait for data to be received
		return UDR0;								//Get and return received data from buffer
	}
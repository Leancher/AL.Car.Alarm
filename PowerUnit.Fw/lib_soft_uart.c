#include <avr/io.h>

#define PIN 5
#define PORT PORTD

void uartSendPause()
{
	//26-28 - 9600 @ 4MHz
	volatile unsigned char v;
	for (v=0; v<108;v++)
	{
		v=v;
	}
}
void uartSendZero()
{
	//0
	PORT|=(1<<PIN);
}
void uartSendOne()
{
	PORT&=(~(1<<PIN));	
}


void uartSendSw( unsigned char byte)
{
	uartSendZero();
	uartSendPause();
	for (unsigned char i=0; i<8; i++)
	{
		if ((byte&1)==0)
		{
			uartSendZero();
			uartSendPause();
		}else
		{
			//1
			uartSendOne();
			uartSendPause();
		}	
		byte=byte>>1;
	}
	uartSendOne();
	uartSendPause();
	uartSendPause();
	//uartSendPause();
	//uartSendPause();	
}

int uart_peek_sw()
{
	unsigned char byte=0;
	byte=uart_peek();
	if (byte!=0xFF)
	{
		byte=byte>>1;
		return byte;
	}
	return byte;
}
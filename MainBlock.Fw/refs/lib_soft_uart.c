#define PIN 5
#define PORT PORTD
#define DDR DDRD
#define DELAY 104.1666666666667 //задержка для УАРТ T=1/Baudrate

void uart_software_init()
{
	DDR|=1<<PIN;
	PORT|=1<<PIN;
}

void uart_software_send_bit(int state)
{
	if (state){PORT|=1<<PIN;}else{PORT&=(~(1<<PIN));}
}

void uart_software_send_byte( unsigned char byte)
{
	//Режим ожидания УАРТ - 1, для начала отправки отправляем 0
	uart_software_send_bit(0);
	_delay_us(DELAY);
	for (unsigned char i=0; i<8; i++)
	{
		if ((byte&1)==0)
		{
			//0
			uart_software_send_bit(0);
			_delay_us(DELAY);
		}else
		{
			//1
			uart_software_send_bit(1);
			_delay_us(DELAY);
		}	
		byte=byte>>1;
	}
	//Перевод в режим ожидания
	uart_software_send_bit(1);
}
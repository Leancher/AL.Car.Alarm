
typedef unsigned char byte;

void gap_ddrb(unsigned char pin, unsigned char val)
{
	if (val){DDRB|=1<<pin;}else{DDRB&=(~(1<<pin));}
}

void gap_portb(unsigned char pin, unsigned char val)
{
	if (val){PORTB|=1<<pin;	}else{PORTB&=(~(1<<pin));}
}

unsigned char gap_pinb(unsigned char pin)
{
	return ((PINB&(1<<pin))==0);
}

void gap_ddrc(unsigned char pin, unsigned char val)
{
	if (val){DDRC|=1<<pin;}else{DDRC&=(~(1<<pin));}
}

void gap_portc(unsigned char pin, unsigned char val)
{
	if (val){PORTC|=1<<pin;	}else{PORTC&=(~(1<<pin));}
}

unsigned char gap_pinc(unsigned char pin)
{
	return ((PINC&(1<<pin))==0);
}

void gap_ddrd(unsigned char pin, unsigned char val)
{
	if (val){DDRD|=1<<pin;}else{DDRD&=(~(1<<pin));}
}

void gap_portd(unsigned char pin, unsigned char val)
{
	if (val){PORTD|=1<<pin;	}else{PORTD&=(~(1<<pin));}
}

unsigned char gap_pind(unsigned char pin)
{
	return ((PIND&(1<<pin))==0);
}

unsigned int getStatePIND(unsigned char pin)
{
	if (gap_pind(pin))
	{
		volatile int wait=0;
		do
		{
			wait++;
			_delay_ms(300);
		} while (gap_pind(pin));
		return wait;
	}
	return 0;
}

unsigned int getStatePINC(unsigned char pin)
{
	if (gap_pinc(pin))
	{
		volatile int wait=0;
		do
		{
			wait++;
			_delay_ms(200);
		} while (gap_pinc(pin));
		return wait;
	}
	return 0;
}

unsigned int getStatePINB(unsigned char pin)
{
	if (gap_pinb(pin))
	{
		volatile int wait=0;
		do
		{
			wait++;
			_delay_ms(200);
		} while (gap_pinb(pin));
		return wait;
	}
	return 0;
}

void setStateLED (byte state)
{
	gap_ddrb(0,1);
	gap_portb(0,state);
}

int getStateDoorTerminalSwitch()
{
	gap_ddrd(4,0);
	gap_portd(4,1);
	//Без ожидания отпускания контактов.
	return gap_pind(4);
}

int getStateDoorSwitch()
{
	gap_ddrc(2,0);
	gap_portc(2,1);
	return getStatePINC(2);
}

int getStateServiceButton()
{
	gap_ddrc(1,0);
	gap_portc(1,1);
	return getStatePINC(1);
}

void setStateIndicator(int state)
{
	gap_ddrc(0,1);
	gap_portc(0,state);
}
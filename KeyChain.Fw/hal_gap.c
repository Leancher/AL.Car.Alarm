unsigned int gap_button_portd(unsigned char pin)
{
	if (gap_pind(pin))
	{
		volatile int wait=0;
		do
		{
			wait++;
			_delay_ms(100);
		} while (gap_pind(pin));
		return wait;
	}
	return 0;
}

int getStateButton1()
{
	gap_ddrd(2,0);
	gap_portd(2,1);
	return gap_button_portd(2);
}

int getStateButton2()
{
	gap_ddrd(3,0);
	gap_portd(3,1);
	return gap_button_portd(3);
}

void setStateLED(int state)
{
	gap_ddrd(6,1);
	gap_portd(6,0);	
	
	gap_ddrd(5,1);
	gap_portd(5,state);
}
void setStatePIN(int port, int pin, int state)
{
	if (port==2)
	{
		gap_ddrb(pin,1);
		gap_portb(pin,state);	
	}
	if (port==1)
	{
		gap_ddrd(pin,1);
		gap_portd(pin,state);		
	}
}

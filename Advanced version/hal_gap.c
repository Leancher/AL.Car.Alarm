
typedef unsigned char byte;

void setStateLED (byte state)
{
	gap_ddrb(0,1);
	gap_portb(0,state);
}

void testButtonEnable()
{
	gap_ddrb(1,0);
	gap_portb(1,1);	
	return;
}

unsigned int gapTestButton()
{
	testButtonEnable();
	return gap_button_portb(1);
};

int getStateTestButton()
{
	return gapTestButton();
}

int getStateStopSignalSwitch()
{
// 	gap_ddrb(2,0);
// 	gap_portb(2,1);
// 	return gap_pinb(2);
	return 0;
}

int getStateLimitSwitch()
{
	gap_ddrb(1,0);
	gap_portb(1,1);
	return gap_pinb(1);
}

int tempGetRPM()
{
		gap_ddrb(0,0);
		gap_portb(0,1);
		return gap_pinb(0);
}

// void sendData()
// {
// 	static int currentComand=1;
// 	//if (getStateButton()>1)
// 	//{
// 	rfpacket_response.data_length=2;
// 	if (currentComand==1) rfpacket_response.response=10;
// 	if (currentComand==2) rfpacket_response.response=20;
// 	rfpacket_response.data[0]=1;
// 	rfpacket_response.data[1]=2;
// 	rfpacket_send();
// 	if (currentComand==1)
// 	{
// 		currentComand=2;
// 	}
// 	else
// 	{
// 		currentComand=1;
// 	}
// 	setStateLED(1);
// 	_delay_ms(300);
// 	setStateLED(0);
// 	_delay_ms(300);
// 	//}
// }
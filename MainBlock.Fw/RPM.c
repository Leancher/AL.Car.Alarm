/*
 * RPM.c
 *
 * Created: 13.10.2014 21:55:15
 *  Author: Andrew
 */ 

int getInputState()
{
	gap_ddrb(0,0);
	gap_portb(0,1);
	return gap_pinb(0);
}

typedef unsigned char byte;

byte detectChange ()
{
	static int lastState = 0;

	int currentState = getInputState();
	if (currentState != lastState)
	{
		lastState = currentState;
		if (currentState==1) return 1;
	}
	return 0;
}

int getRPM()
{
	int countState = 1;
	int widthPeriod = 0;
	
	while (countState<=4)
	{
		if (detectChange()==1)
		{
			countState++;
		}
		widthPeriod++;
		//Число 1.6 после теста в машине
		_delay_ms(1.6);
	}
	widthPeriod/=4;
	int rpm=1000/(widthPeriod)*60;
	uart_send_int("RPM: ",rpm);
	return rpm;
}
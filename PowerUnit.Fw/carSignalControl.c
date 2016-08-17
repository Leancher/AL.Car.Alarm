/*
 * carSignalControl.c
 *
 * Created: 13.10.2014 22:25:01
 *  Author: Andrew
 */ 

void flashTurnSignal(int number)
{
	for (int count=1; count<=number; count++)
	{
		setStateTurnSignalRelay(1);
		_delay_ms(100);
		setStateTurnSignalRelay(0);
		_delay_ms(100);
	}
}

void sirenSignal (int state)
{
	if (state==1) setStateSirenRelay(1);
	if (state==0) setStateSirenRelay(0);
}


void sirenSignalBeep ()
{
	setStateSirenRelay(1);
	_delay_ms(500);
	setStateSirenRelay(0);
}

void flashLight (int state)
{
	static int ledCycle=0;
	if (ledCycle==1)
	{
		if (state==4) setStateLED(1);
	}
	if (ledCycle==2)
	{
		if (state==4)
		{
			setStateLED(0);
			ledCycle=0;
		}

	}
	if (ledCycle==5)
	{
		if (state==2) setStateLED(1);
	}
	if (ledCycle==10)
	{
		if (state==3)
		{
			setStateLED(1);
		}
		else
		{
			setStateLED(0);
			ledCycle=0;
		}
	}
	ledCycle++;
	if (ledCycle == 20)
	{
		if (state != 5)
		{
			setStateLED(0);
			ledCycle = 0;
		}
		if (state == 5) setStateLED(1);
	}
	if (ledCycle == 40)
	{
		if (state == 5)
		{
			setStateLED(0);
			ledCycle = 0;
		}
	}
}

void showState (int state)
{
	if (state==0) setStateLED (0);
	if (state==1)
	{
		setStateLED(1);
	}
	else
	{
		flashLight(state);
	}
}
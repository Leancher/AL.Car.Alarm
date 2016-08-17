/*
 * relayControl.c
 *
 * Created: 13.10.2014 22:11:14
 *  Author: Andrew
 */ 


void setStateRelayPORTD (int pin, int state)
{
	gap_ddrd(pin,1);
	gap_portd(pin,state);
}

void setStateRelayPORTB (int pin, int state)
{
	gap_ddrb(pin,1);
	gap_portb(pin,state);
}

void setStateDoorLockRelay(int state)
{
	gap_ddrd(3,1);
	gap_portd(3,state);
}

void setStateDoorUnlockRelay(int state)
{
	gap_ddrd(2,1);
	gap_portd(2,state);
}

void setStateTurnSignalRelay(int state)
{
	gap_ddrd(7,1);
	gap_portd(7,state);
}

void setStateSirenRelay(int state)
{
	gap_ddrd(5,1);
	gap_portd(5,state);
}
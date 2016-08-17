/*
 * al.doorswitch.c
 *
 * Created: 13.08.2013 11:21:09
 *  Author: Андрей
 */ 



void doorUnLock ()
{
	setStateDoorUnlockRelay(1);
	_delay_ms(1000);
	setStateDoorUnlockRelay(0);	
	uart_send_string ("Door unlock.\n\r");
}

void doorLock()
{
	setStateDoorLockRelay(1);
	_delay_ms(1000);
	setStateDoorLockRelay(0);
	uart_send_string ("Door lock.\n\r");
}
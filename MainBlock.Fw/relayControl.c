#define RELAY_1 1 //D8/PB0 - Trunk lock
#define RELAY_2 2 //D7/PD7 - Siren
#define RELAY_3 3 //D6/PD6 - Door lock
#define RELAY_4 4 //D2/PD2 - Door unlock
#define RELAY_5 5 //D3/PD3 - Autostart - starter
#define RELAY_6 6 //D4/PD4 - Turn signal 1
#define RELAY_7 7 //D5/PD5 - Turn signal 2
#define RELAY_8 8 //D9/PB1 - Autostart - ignition

//Устанавливаем состояние определнного реле
void setStateRelay(int numberRelay1, int numberRelay2, int state)
{
	int stateRelay=0;
	if (state==1) stateRelay|=(1<<(numberRelay1-1));
	if (state==0) stateRelay&=(0<<(numberRelay1-1));
	//Если нужно включить одновременно два реле
	if (numberRelay2>0)
	{
		if (state==1) stateRelay|=(1<<(numberRelay2-1));
		if (state==0) stateRelay&=(0<<(numberRelay2-1));		
	}
	//Отправляем 3 раза для уменьшения ошибок передачи
	for (int numberSend=1;numberSend<4;numberSend++)
	{
		uart_software_send_byte(stateRelay);
		_delay_ms(10);
	}
	//Отправляем контрольное число для проверки
	uart_software_send_byte(0xAA);
	_delay_ms(10);
}

void trunkLock(int state)
{
	if (state==1) setStateRelay(RELAY_1,0,1);
	if (state==0) setStateRelay(RELAY_1,0,0);
}

void doorUnLock ()
{
	setStateRelay(RELAY_4,0,1);
	_delay_ms(1000);
	setStateRelay(RELAY_4,0,0);
	uart_send_string ("Door unlock.\n\r");
}

void doorLock()
{
	setStateRelay(RELAY_3,0,1);
	_delay_ms(1000);
	setStateRelay(RELAY_3,0,0);
	uart_send_string ("Door lock.\n\r");
}

void flashTurnSignal()
{
	setStateRelay(RELAY_6,RELAY_7,1);
	_delay_ms(500);
	setStateRelay(RELAY_6,RELAY_7,0);
	_delay_ms(500);
}

void sirenSignal (int state)
{
	if (state==1) setStateRelay(RELAY_2,0,1);
	if (state==0) setStateRelay(RELAY_2,0,0);
}

void sirenSignalBeep ()
{
	setStateRelay(RELAY_2,0,1);
	_delay_ms(1000);
	setStateRelay(RELAY_2,0,0);
}
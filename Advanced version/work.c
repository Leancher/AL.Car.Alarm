

//Какая кнопка нажата: 1 - открыть (деактивировать), 2 - закрыть (активировать)
int numberButtonPush=0;

//Текущее состояние сигнализации:
//0 - охрана снята, ожидание автоматической установки через 30 сек, если двери не открывались.
//1 - охрана снята, дверь открыта и закрыта (водитель сел).
//2 - двигатель запущен, нажали педаль тормоза, двери автоматически заблокировались. 
//3 - двигатель выключен, двери автоматически разблокировались, ожидание автоматической постановки на охрану 
//после открытия и закрытия дверей (водитель вышел)
//4 - охрана установлена.
int currentStateCarAlarm=0;

//Проверяем зажигания
int getStateEngine ()
{
	if (tempGetRPM()==1) return 1;
	//if (getRPM() > 100) return 1;
	
	return 0;
}

int getStateDoor()
{
	if (getStateLimitSwitch()==1)
	{
		if (currentStateCarAlarm==4) 
		{
			uart_send_string ("Brake into car!\n\r");
			return 1;			
		}

		uart_send_string ("\n\r");
		uart_send_string ("Door open.\n\r");
		while (getStateLimitSwitch()==1)
		{
			showState(4);
			_delay_ms(100);
		}
		uart_send_string ("Door close.\n\r");
		return 1;
	}
	return 0;
}

void getNumberButton()
{
	if (rfpacket_receive())
	{
		if (rfpacket_request.command==10) setStateLED(1);//numberButtonPush=1;
		
		if (rfpacket_request.command==20) setStateLED(0);//numberButtonPush=2;
	}
}

void carAlarmActivate()
{
	//Если нажали на кнопку раньше, чем открыли\закрыли дверь
	if (currentStateCarAlarm==3) currentStateCarAlarm=0;
	//Если нажали на кнопку после открыли\закрыли дверь, но не заводили двигатель
	if (currentStateCarAlarm==1) currentStateCarAlarm=0;
	numberButtonPush=0;
	if (getStateEngine()==1)
	{
		doorLock();
		trunkLock(1);
		return;
	}
	currentStateCarAlarm=4;
	uart_send_string ("Car alarm activate.\n\r");
	sirenSignalBeep(100);
	flashTurnSignal(1);	
	doorLock();
	trunkLock(1);
	showState(2);
}

void carAlarmDeactivate()
{
	//Охрана сработала, нажали кнопку на брелке
	if (currentStateCarAlarm==5)
	{
		sirenSignal(0);
		uart_send_string ("Siren turn off.\n\r");
		currentStateCarAlarm=4;
		numberButtonPush=0;
		return;
	}
	//Если нажали на кнопку раньше, чем открыли\закрыли дверь
	if (currentStateCarAlarm==3) currentStateCarAlarm=0;
	if (currentStateCarAlarm==4) currentStateCarAlarm=0;
	numberButtonPush=0;
	if (getStateEngine()==1)
	{
		doorUnLock();
		trunkLock(0);
		return;
	}
	int cycle=0;
	uart_send_string ("Car alarm deactivate.\n\r");
	sirenSignalBeep(100);
	flashTurnSignal(2);
	doorUnLock();
	trunkLock(0);
	
	if (currentStateCarAlarm==0)
	{
		uart_send_string ("Waiting for automatic activate:");
		while (cycle<=5)
		{
			getNumberButton();
			if (numberButtonPush != 0) return;
			if (getStateDoor()==1)
			{
				uart_send_string ("\n\r");
				currentStateCarAlarm=1;
				return;
			}
			uart_send_string ("*");
			cycle++;
			_delay_ms(100);
		}
		uart_send_string ("\n\r");
		numberButtonPush=2;
	}
}

void processingButtonPush()
{
	if (numberButtonPush==1) carAlarmDeactivate();
	
	if (numberButtonPush==2) carAlarmActivate();
}

void waitForActivateCarAlarm()
{
	int cycle=0;
	uart_send_string ("Waiting for automatic activate:");
	
	while (cycle<=10)
	{
		
		getNumberButton();
		if (numberButtonPush != 0) return;
		
		if (getStateDoor()==1)
		{
			cycle=0;
		}
		cycle++;
		uart_send_string ("*");
		_delay_ms(100);
	}
	uart_send_string ("\n\r");
	numberButtonPush=2;
}

void switchStateCurrentButton()
{
	if (getStateTestButton() > 1)
	{
		if (numberButtonPush==0) 
		{
			numberButtonPush=1;
			return;
		}
		uart_send_string ("Service button push.\n\r");	
		if (numberButtonPush==1)
		{
			numberButtonPush=2;
		}
		else
		{
			numberButtonPush=1;
		}
		processingButtonPush();
	}
}

void switchStateRelay()
{
	static int n=1;
	if (getStateTestButton()>1)
	{
		stateRelay=0;
		stateRelay|=(1<<3);
		if (n==9) n=1;
		setStateRelay(n,0,1);
		n++;
	}
}

void rfm69_send_data(byte data)
{
	rfpacket_response.data_length=2;
	rfpacket_response.response=data;
	rfpacket_response.data[0]=1;
	rfpacket_response.data[1]=2;
	rfpacket_send();
}

int rfm69_get_data()
{
	int data=0;
	do
	{
		if (rfpacket_receive())
		{
			if (rfpacket_request.command!=99) data=rfpacket_request.command;
			_delay_ms(50);
			rfm69_send_data(99);
		}
	} while (rfpacket_request.command!=99);

	return data;
}

void startWork()
{
	//switchStateCurrentButton();
	//Проверить УАРТ, определить какую кнопку нажали на брелке
	if(numberButtonPush==0) getNumberButton();
	//Обработать нажатие кнопки, если была нажата.
	processingButtonPush();
	//Проверить заведен ли двигатель
	if (getStateEngine()==1)
	{
		//Двигатель работает. Сигнализация была деактивирована, а затем открыта и закрыта дверь (в машину сели).
		if (currentStateCarAlarm==1)
		{
			uart_send_string ("Engine is working.\n\r");
			//При нажатии педали тормоза, заблокировать двери.
			if (getStateStopSignalSwitch()==1)
			{
				uart_send_string ("Brake pedal push.\n\r");
				doorLock();
				trunkLock(1);
				uart_send_string ("Engine is working.\n\r");
				//Установить переменную для последующего определения выключения двигателя
				currentStateCarAlarm=2;
			}		
		}
		if (currentStateCarAlarm==3) currentStateCarAlarm=1;
	}
	else
	{
		//Двигатель был заглушен. Автоматически разблокировать дверь.
		if (currentStateCarAlarm==2)
		{
			uart_send_string ("Engine has stopped.\n\r");
			doorUnLock();
			trunkLock(0);
			//Установить переменную для последующей автоматической установки сигналиции на охрану.
			currentStateCarAlarm=3;
		}
	}
	//Проверить открывалась ли дверь.
	if (getStateDoor()==1)
	{
		//Если вручную разблокировали дверь и открыли\закрыли ее при заведенном двигателе
		if (currentStateCarAlarm==2) currentStateCarAlarm=1;
		//Автоматическая установка охраны, когда двигатель заглушен и дверь открылась и закрылась (водитель вышел).
		if (currentStateCarAlarm==3) waitForActivateCarAlarm();
		//Если охрана установлена, а дверь открыли, включаем тревогу.
		if (currentStateCarAlarm==4) 
		{
			sirenSignal(1);
			uart_send_string ("Alarm! Siren turn on.\n\r");
			currentStateCarAlarm=5;			
		}
	}
	_delay_ms(100);
}
// 1 - Vin
// 2 - GND
// 3 (A0/PC0) - Indicator
// 4 (A1/PC1) - Service button
// 5 (A2/PC2) - Door switch button
// 6 (D7/PD7) - Out2
// 7 (D6/PD6) - Out1
// 8 - Out 5V
// 9 (A3/PC3) - Brake
// 10 (A4/PC4) - Capot
// 11 (A5/PC5) - Trunk
// 12 (D4/PD4) - Door
// 13 (D3/PD3) - Taho
// 14 (D2/PD2) - Ignition



//Устанавливаем состояние сигнализации
void setStateGuard()
{
	//Деактивировать охрану
	if (stateAlarm==1)
	{
		doorUnLock();
		trunkLock(1);
		stateLED=0;
	}
	//Активировать охрану
	if (stateAlarm==2)
	{
		doorLock();
		trunkLock(0);
		stateLED=1;
	}
	//Если нажата салонная кнопка, то не включаем сигналы машины.
	if (presenceDriver==1) return;
	sirenSignalBeep();
	flashTurnSignal();
}

//Отправляемм данные с помощью передатчика
void rfm69_send_data(byte data)
{
	rfpacket_response.data_length=2;
	rfpacket_response.response=data;
	rfpacket_response.data[0]=1;
	rfpacket_response.data[1]=2;
	rfpacket_send();
}

//Принимаем данные с помощью передатчика
int rfm69_get_data()
{
	int data=0;
	//Обмениваемся данными, пока оба передатчика не получат подтверждения, что данные пришли.
	//99 - контрольное число
	do
	{
		//Проверяем, получены ли данные
		if (rfpacket_receive())
		{
			//Если данные пришли и это не контрольное число, то записываем его.
			
			if (rfpacket_request.command!=99) data=rfpacket_request.command;
			_delay_ms(50);
			//Отправляем контрольное число обратно (можно отправить любое число, отличное от команды), сообщая, что данные получены
			rfm69_send_data(99);
		}
		//Принимаем до тех пор, пока не получим контрольное число
	} while (rfpacket_request.command!=99);

	return data;
}

void checkDoorSwitchButton()
{
	//Тревога включена. Выключить салонной кнопкой открытия двери нельзя.
	if (stateAlarm==3) return;
	//Проверяем салонную кнопку
	if (getStateDoorSwitch()>1)
	{
		wdt_reset();
		uart_send_string ("Door switch push.\n\r");
		//Переключаем состояние сигнализации
		if (stateAlarm==1) 
		{
			stateAlarm=2;
		}
		else 
		{
			stateAlarm=1;
		}
		//Если нажата салонная кнопка, значит водитель внутри, не включаем сигналы машины
		presenceDriver=1;
		//Устанавливаем состояние сигнализации
		setStateGuard();
	}
}

void setStateAlarm()
{
	//Охрана деактивирована, тревогу включать не нужно
	if (stateAlarm==1) return;
	//Охрана активирована. Проверяем не открыта ли дверь.
	if (stateAlarm==2)
	{
		if (getStateDoorTerminalSwitch()>1) // - ПЕРЕПИСАТЬ БЕЗ ОЖИДАНИЯ ОТПУСКАНИЯ КНОПКИ
		{
			wdt_reset();
			//Если дверь открыли, то включаем тревогу.
			stateAlarm=3;
			uart_send_string ("Alarm on!\n\r");			
		}
	}
	if (stateAlarm==3)
	{
		uart_send_string ("Alarm!\n\r");
		//Чтобы отключить тревогу без брелка, нужно нажать секретную кнопку
		if (getStateServiceButton()>1) 
		{
			uart_send_string ("Service button push.\n\r");
			stateAlarm=1;
			cycleAlarm=0;
			return;
		}
		//Подаем сигналы тревоги циклично через 1 сек
		if (cycleAlarm >= 10)
		{
			sirenSignal(1);
			flashTurnSignal();
			sirenSignal(0);
			cycleAlarm=0;	
		}
		cycleAlarm++;
	}
}

void startWork()
{
	setStateLED(stateLED);
	setStateAlarm();	
	checkDoorSwitchButton();
	int currentCommand = rfm69_get_data();
	wdt_reset();
	//Если полученное число равно 0, значит команду не отправляли.
	if (currentCommand!=0)
	{
		wdt_reset();
		uart_send_string ("Keychain button push.\n\r");
		//Нажата кнопка на брелке, водитель не в машине.
		presenceDriver=0;
		//Деактивировать охрану
		if (currentCommand==10) stateAlarm=1;
		//Активировать охрану
		if (currentCommand==20) stateAlarm=2;
		//Установить состояние сигналиции
		setStateGuard();
	}
	_delay_ms(100);
}
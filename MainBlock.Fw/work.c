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



//������������� ��������� ������������
void setStateGuard()
{
	//�������������� ������
	if (stateAlarm==1)
	{
		doorUnLock();
		trunkLock(1);
		stateLED=0;
	}
	//������������ ������
	if (stateAlarm==2)
	{
		doorLock();
		trunkLock(0);
		stateLED=1;
	}
	//���� ������ �������� ������, �� �� �������� ������� ������.
	if (presenceDriver==1) return;
	sirenSignalBeep();
	flashTurnSignal();
}

//����������� ������ � ������� �����������
void rfm69_send_data(byte data)
{
	rfpacket_response.data_length=2;
	rfpacket_response.response=data;
	rfpacket_response.data[0]=1;
	rfpacket_response.data[1]=2;
	rfpacket_send();
}

//��������� ������ � ������� �����������
int rfm69_get_data()
{
	int data=0;
	//������������ �������, ���� ��� ����������� �� ������� �������������, ��� ������ ������.
	//99 - ����������� �����
	do
	{
		//���������, �������� �� ������
		if (rfpacket_receive())
		{
			//���� ������ ������ � ��� �� ����������� �����, �� ���������� ���.
			
			if (rfpacket_request.command!=99) data=rfpacket_request.command;
			_delay_ms(50);
			//���������� ����������� ����� ������� (����� ��������� ����� �����, �������� �� �������), �������, ��� ������ ��������
			rfm69_send_data(99);
		}
		//��������� �� ��� ���, ���� �� ������� ����������� �����
	} while (rfpacket_request.command!=99);

	return data;
}

void checkDoorSwitchButton()
{
	//������� ��������. ��������� �������� ������� �������� ����� ������.
	if (stateAlarm==3) return;
	//��������� �������� ������
	if (getStateDoorSwitch()>1)
	{
		wdt_reset();
		uart_send_string ("Door switch push.\n\r");
		//����������� ��������� ������������
		if (stateAlarm==1) 
		{
			stateAlarm=2;
		}
		else 
		{
			stateAlarm=1;
		}
		//���� ������ �������� ������, ������ �������� ������, �� �������� ������� ������
		presenceDriver=1;
		//������������� ��������� ������������
		setStateGuard();
	}
}

void setStateAlarm()
{
	//������ ��������������, ������� �������� �� �����
	if (stateAlarm==1) return;
	//������ ������������. ��������� �� ������� �� �����.
	if (stateAlarm==2)
	{
		if (getStateDoorTerminalSwitch()>1) // - ���������� ��� �������� ���������� ������
		{
			wdt_reset();
			//���� ����� �������, �� �������� �������.
			stateAlarm=3;
			uart_send_string ("Alarm on!\n\r");			
		}
	}
	if (stateAlarm==3)
	{
		uart_send_string ("Alarm!\n\r");
		//����� ��������� ������� ��� ������, ����� ������ ��������� ������
		if (getStateServiceButton()>1) 
		{
			uart_send_string ("Service button push.\n\r");
			stateAlarm=1;
			cycleAlarm=0;
			return;
		}
		//������ ������� ������� �������� ����� 1 ���
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
	//���� ���������� ����� ����� 0, ������ ������� �� ����������.
	if (currentCommand!=0)
	{
		wdt_reset();
		uart_send_string ("Keychain button push.\n\r");
		//������ ������ �� ������, �������� �� � ������.
		presenceDriver=0;
		//�������������� ������
		if (currentCommand==10) stateAlarm=1;
		//������������ ������
		if (currentCommand==20) stateAlarm=2;
		//���������� ��������� ����������
		setStateGuard();
	}
	_delay_ms(100);
}
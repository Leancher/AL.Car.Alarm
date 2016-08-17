#define COMMAND_1 10
#define COMMAND_2 20
#define COMMAND_CONTROL 99

void sendData(int currentData)
{
	byte received=0;
	do
	{
		//Один раз отправляем данные
		rfpacket_response.data_length=2;
		rfpacket_response.response=currentData;
		rfpacket_response.data[0]=1;
		rfpacket_response.data[1]=2;
		rfpacket_send();
		//Несколько раз принимаем данные (тонкости работы передатчика)
		for (int i=0; i<100; i++)
		{
			_delay_ms(1);
			//Если что-то приняли, то устанавливаемм переменную в 1
			if (rfpacket_receive()){received=1;}
		}
	} while (received==0);
}

int getNumberButton()
{
	int currentButton=0;
	if (getStateButton1()>1) currentButton=1;
	if (getStateButton2()>1) currentButton=2;
	return currentButton;
}

void work()
{
	wdt_reset();
	int numberButton=getNumberButton();
	if (numberButton!=0)
	{
		setStateLED(1);
		//Отправляем команду
		if (numberButton==1) {sendData(COMMAND_1);}
		if (numberButton==2) {sendData(COMMAND_2);}
		_delay_ms(50);
		//Отправляем контрольное число, чтобы приемник мог опрделить, что отправка команды закончилась.
		sendData(COMMAND_CONTROL);
		_delay_ms(300);
		setStateLED(0);
		_delay_ms(300);		
	}

}
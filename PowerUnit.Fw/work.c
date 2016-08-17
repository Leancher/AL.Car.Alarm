#define AMOUNT_RELAY 8

#define RELAY_1 0 //D8/PB0 - Trunk lock
#define RELAY_2 7 //D7/PD7 - Siren
#define RELAY_3 6 //D6/PD6 - Door lock
#define RELAY_4 2 //D2/PD2 - Door unlock
#define RELAY_5 3 //D3/PD3 - Autostart - starter
#define RELAY_6 4 //D4/PD4 - Turn signal 1
#define RELAY_7 5 //D5/PD5 - Turn signal 2
#define RELAY_8 1 //D9/PB1 - Autostart - ignition

#define PD 1
#define PB 2

int stateRelays=0;

int relayState[AMOUNT_RELAY];
int relayPin[AMOUNT_RELAY];
int relayPort[AMOUNT_RELAY];

void relayPortInit()
{
	relayPin[0]=RELAY_1;//Pin0
	relayPin[1]=RELAY_2;//Pin7
	relayPin[2]=RELAY_3;//Pin6
	relayPin[3]=RELAY_4;//Pin2
	relayPin[4]=RELAY_5;//Pin3
	relayPin[5]=RELAY_6;//Pin4
	relayPin[6]=RELAY_7;//Pin5
	relayPin[7]=RELAY_8;//Pin1
	
	relayPort[0]=PB;
	relayPort[1]=PD;
	relayPort[2]=PD;
	relayPort[3]=PD;
	relayPort[4]=PD;
	relayPort[5]=PD;
	relayPort[6]=PD;
	relayPort[7]=PB;
}

void setStateRelay()
{
	for (int currentNumber=0;currentNumber<8;currentNumber++)
	{
		setStatePIN(relayPort[currentNumber], relayPin[currentNumber],relayState[currentNumber]);
	}
}

int getStateRelay()
{
	int dataUart = 0;
	//Получаем даннные с УАРТ, пока не получим контрольное число 0xAA
	do
	{
		//Получаем данные с УАРТ. Отправка из источника происходит несколько раз.
		do
		{
			dataUart=uart_peek();
			
		} while (dataUart==0xFF);
		//Если принятое значение не равно контрольному числу, то записываем значение в переменную
		if (dataUart!=0xAA) stateRelays=dataUart;
	}while(dataUart!=0xAA);
	//Возвращаем контрольное число
	return dataUart;
}

//Отправляем принятое значение на УАРТ для отладки
void uartSendStateRelay()
{
	for (int currentNumber=0;currentNumber<8;currentNumber++)
	{
		uart_send_only_int(currentNumber+1);
		uart_send_string(" - ");
		uart_send_only_int(relayState[currentNumber]);
		uart_send_string("\n\r");
	}
	uart_send_string("\n\r");
}

//Обновляем массив реле после приема новых данных.
void updateStateRelay()
{
	for (int n=0;n<8;n++)
	{
		//Если результат сравнения битов равен 1, то записываем 1 
		if (((stateRelays>>n)&1)==1) relayState[n]=1;
		//Если результат сравнения битов равен 0, то записываем 0
		if (((stateRelays>>n)&1)==0) relayState[n]=0;
	}
}

void startWork()
{
	//Если контрольное число совпадает, то работаем с массивом реле
	if (getStateRelay()==0xAA) 
	{
		updateStateRelay();	
		uartSendStateRelay();
		setStateRelay();
	}
	else
	{
		uart_send_string("Error.\n\r");
	}
}
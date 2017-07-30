#include "control_engine.h"
#include "../refs/bwl_simplserial.h"
#include "../refs/bwl_strings.h"
#include "../refs/bwl_adc.h"

typedef enum
{
	ENGINE_STOP=1,
	IGNITION_INIT=2,
	IGNITION_READY=3,
	ENGINE_STARTING=4,
	ENGINE_RUN=5,
	ENGINE_STOPPING=6,
	ENGINE_ALREADY_STARTED=7,
	ADD_FIVE_MIN=8,
} DEVICE_STATE;
DEVICE_STATE current_state = ENGINE_STOP;

int voltage_battery=0;
int number_minutes_work=0;
//1 - если двигатель запущен с помощью СМС
byte remote_running=0;

int get_voltage()
{
	int val=0;
	val=adc_read_average(3);
	val=val*ADC_VOLT_MULTIPLIER_MV;//+ADC_DIODE_CORRECTION;
	return val;
}

typedef enum
{
	RESULT_OK=50,
	RESULT_ERROR=51,
	RESULT_STOP=52,
	RESULT_ALREADY_RUN=53,
	RESULT_ADD_TIME=54,
} RESULT_MESSAGE;
RESULT_MESSAGE  number_command = RESULT_STOP;
//50 - успешный запуск
//51 - двигатель не смог запуститься
//52 - двигатель остановлен
//53 - двигатель уже запущен
//54 - добавлено 5 минут
void send_sms(int number_command)
{
	sserial_request.command=number_command;
	sserial_request.datalength=0;
	volatile char result=sserial_send_request_wait_response(UART_485, 100);
	if (result==0)
	{
		string_clear();
		string_add_string("Board not respond");
	}else
	{
		string_add_string("Board respond");
	}
}

//Включить зажигание
void ignition_turn_on()
{
	int counter = 0;
	relay_ignition_set_state(1);
	//Задержка для подготовки к запуску (включается бензонасос для установки давления)
	while (counter<DELAY_IGNITION_INIT)
	{
		wdt_reset();
		counter++;
		_delay_ms(100);
	}
}

void starter_control()
{
	int counter = 0;
	while (get_voltage()<VOLTAGE_STARTER_STOP)
	{
		//Если стартер включен долгое время, а двигатель не заводится, выключаем
		if (counter>DURING_STARTER_WORK)
		{
			current_state=ENGINE_STOP;
			return;
		}
		wdt_reset();
		counter++;
		_delay_ms(1);
	}
	current_state=ENGINE_RUN;
}

//запуск двигателя
void start_engine()
{
	wdt_reset();
	ignition_turn_on();
	relay_starter_set_state(1);
	starter_control();
	relay_starter_set_state(0);
	adc_off();
	//Если двигатель не запустился, выключаем зажигание
	if (current_state==ENGINE_STOP) relay_ignition_set_state(0);
}

void get_state_start_button()
{
	static int index=0;
	index++;
	if (index>300)
	{
		if (board_button_is_pressed()==1)
		{
			adc_init_voltage_generator();
			if (get_voltage()>VOLTAGE_RUN_ENGINE)
			{
				relay_ignition_set_state(0);
				return;
			}
			start_engine();
		}
		index=0;
	}
}

void process_command_start()
{
	adc_init_voltage_generator();
	if (get_voltage()>VOLTAGE_RUN_ENGINE)
	{
		//Двигатель уже запущен
		current_state=ENGINE_RUN;
		sserial_response.data[0]=ENGINE_ALREADY_STARTED;
		sserial_response.datalength=1;
		sserial_send_response();
		send_sms(RESULT_ALREADY_RUN);
		return;
	}
	//Двигатель выключен, отправляем ответ
	if (sserial_request.data[0]<255)	number_minutes_work = sserial_request.data[0];
	//during_work=2;
	sserial_response.data[0]=ENGINE_STARTING;
	remote_running=1;
	sserial_response.datalength=1;
	sserial_send_response();
	start_engine();
	//Если двигатель запустился, отправляем сообщение
	if (current_state==ENGINE_RUN) send_sms(RESULT_OK);
	//Если двигатель не запустился, отправляем сообщение, выключаем зажигание
	if (current_state==ENGINE_STOP) send_sms(RESULT_ERROR);
}

//Тест
void led_on()
{
	sserial_request.command=8;
	sserial_request.data[0]=1;
	sserial_request.datalength=1;
	volatile char result=sserial_send_request_wait_response(UART_485, 100);
	if (result==0)
	{
		string_clear();
		string_add_string("Board not respond");
	}else
	{
		string_add_string("LED on");
	}
}
//Тест
void led_off()
{
	sserial_request.command=9;
	sserial_request.data[0]=0;
	sserial_request.datalength=1;
	volatile char result=sserial_send_request_wait_response(UART_485, 100);
	if (result==0)
	{
		string_clear();
		string_add_string("Board not respond");
	}
	else
	{
		string_add_string("LED off");
	}
}
//Тест
void switch_led()
{
	static int current_state=0;
	static int index=0;
	index++;
	if (index>200)
	{
		if (board_button_is_pressed()==1)
		{
			if (current_state==0)
			{
				led_on();
				current_state=1;
			}
			else
			{
				led_off();
				current_state=0;
			}
		}
		index=0;
	}
}



void process_command_control_engine()
{
	//Напряжение батареи
	if (sserial_request.command==4)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		adc_init_vol_power_in();
		//Данные с АЦП состоят из пяти символов. Переслать можно только три символа, делим на 100
		sserial_response.data[0]=get_voltage()/100;
		sserial_response.datalength=1;
		sserial_send_response();
		adc_off();
	}

	//Остановить двигатель
	if (sserial_request.command==5)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		sserial_response.data[0] = ENGINE_STOP;
		sserial_response.datalength=1;
		sserial_send_response();
		relay_ignition_set_state(0);
		current_state=ENGINE_STOP;
		remote_running=0;
		send_sms(RESULT_STOP);
	}
	if (sserial_request.command==8)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		if (sserial_request.data[0]<255)	board_led_set_state(sserial_request.data[0]);
		sserial_response.data[0]=1;
		sserial_response.datalength=1;
		sserial_send_response();
	}
	if (sserial_request.command==9)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		if (sserial_request.data[0]<255)	board_led_set_state(sserial_request.data[0]);
		sserial_response.data[0]=1;
		sserial_response.datalength=1;
		sserial_send_response();
	}
	//Запустить двигатель на 10
	if (sserial_request.command==10)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_command_start();
	}
	//Запустить двигатель на 15
	if (sserial_request.command==15)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_command_start();
	}
	//Запустить двигатель на 20
	if (sserial_request.command==20)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_command_start();
	}
	//Запустить двигатель на 25
	if (sserial_request.command==25)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_command_start();
	}
	//Запустить двигатель на 30
	if (sserial_request.command==30)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_command_start();
	}
	//Добавить 5 минут
	if (sserial_request.command==35)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		sserial_response.data[0] = ADD_FIVE_MIN;
		sserial_response.datalength=1;
		sserial_send_response();
		number_minutes_work+=5;
		send_sms(RESULT_ADD_TIME);
	}
}

void process_running_engine()
{
	static int counter_ms=0;
	static int counter_sec=0;
	if (current_state==ENGINE_RUN)
	{
		if (remote_running==1) 
		{
			if (number_minutes_work>0)
			{
				counter_ms++;
				//Считаем милисекунды
				if (counter_ms>1000)
				{
					counter_sec++;
					counter_ms=0;
				}
				//Считаем секунды
				if (counter_sec>60)
				{
					number_minutes_work--;
					counter_sec=0;
				}
			}
			//Если время кончилось, переходим к остановке
			else
			{
				remote_running=0;
			}
			//Если ключ вставлен, то питание идет через замок зажигания, обнуляем счетчик и переходим к выключению
			if (sensor_ignition_key_is_pressed()==1) remote_running=0;
			
			_delay_ms(1);	
		}
		else
		{	
			relay_ignition_set_state(0);
			current_state=ENGINE_STOP;
			send_sms(RESULT_STOP);			
			counter_ms=0;
			counter_sec=0;		
		}
	}
}
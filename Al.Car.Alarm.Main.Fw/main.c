//Atmel Studio Firmware Car alarm v1

#include "board/board.h"

#include <avr/wdt.h>

//#METHODS
#define DEV_NAME "Car alarm 1.0"
#define ADC_VOLT_MULTIPLIER_MV		(68+2.2)/2.2 * 1.1 //35.1
#define ADC_DIODE_CORRECTION 600
#define DELAY_IGNITION_INIT 30 //30 * 100 ms = 3000 ms
#define DURING_STARTER_WORK 2000 //2000 ms
#define DELAY_BEFOR_NEXT_START 5000
#define VOLTAGE_STARTER_STOP 12500//12500 в машине
#define VOLTAGE_RUN_ENGINE 13000
#define VOLTAGE_BAT_MINIMAL 9000


typedef enum
{
	ENGINE_STOP=1,
	IGNITION_INIT=2,
	IGNITION_READY=3,
	ENGINE_STARTING=4,
	ENGINE_RUN=5,
	ENGINE_STOPPING=6,
	ENGINE_ALREADY_STARTED=7,
} DEVICE_STATE;
DEVICE_STATE current_state = ENGINE_STOP;

uint16_t voltage_battery=0;
int during_work=0;
//1 - если двигатель запущен с помощью СМС
byte engine_run_sms=0;
byte bluetooth_label_presence=0;

void device_init()
{
	set_unused_pin();
	button_start_engine_enable();
	relay_ignition_set_state(0);
	relay_starter_set_state(0);
	//bluetooth_data_in_enable();
}

int get_acc_voltage()
{
	uint16_t val=0;
	val=adc_read_average(3);
	val=val*ADC_VOLT_MULTIPLIER_MV+ADC_DIODE_CORRECTION;
	return val;
// 	static unsigned long sum=0;
// 	static int count=0;
// 	
// 	sum+=adc_read_once();
// 	count++;
// 	if (count>2)
// 	{
// 		sum=sum/count;
// 		sum=sum*ADC_VOLT_MULTIPLIER_MV;
// 		return sum/count;
// 	}
// 	return 0;
}

int get_voltage_battery()
{
	uint16_t val=0;
	val=adc_read_average(3);
	val=val*ADC_VOLT_MULTIPLIER_MV+ADC_DIODE_CORRECTION;
	return val;
}

void get_state_start_button()
{
	static int index=0;
	index++;	
	if (index>300)
	{
		if (button_start_engine_is_pressed()==1)
		{
			//select_current_state();
		}
		index=0;	
	}
}

//50 - успешный запуск
//51 - двигатель не смог запуститься
//52 - двигатель остановлен
//53 - двигатель уже запущен
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
	while (counter<DELAY_IGNITION_INIT)
	{
		wdt_reset();
		counter++;
		_delay_ms(100);
	}
}

void starter_work_control()
{
	int counter = 0;
	while (get_voltage_battery()<VOLTAGE_STARTER_STOP)
	{
		//Если стартер включен долгое время, а двигатель не заведен, выключаем
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

//Обработка запроса на запуск двигателя
void process_start_engine_by_sms()
{
	adc_init_voltage_input();
	if (get_voltage_battery()>VOLTAGE_RUN_ENGINE)
	{
		//Двигатель уже запущен
		current_state=ENGINE_RUN;
		sserial_response.data[0]=ENGINE_ALREADY_STARTED;
		sserial_response.datalength=1;
		sserial_send_response();
		send_sms(53);
		return;
	}
	//Двигатель выключен, отправляем ответ
	if (sserial_request.data[0]<255)	during_work = sserial_request.data[0];
	//during_work=2;
	sserial_response.data[0]=ENGINE_STARTING;
	engine_run_sms=1;
	sserial_response.datalength=1;
	sserial_send_response();	
	ignition_turn_on();
	relay_starter_set_state(1);
	starter_work_control();
	relay_starter_set_state(0);
	adc_off();
	if (engine_run_sms==1) 
	{
		//Если двигатель запустился, отправляем сообщение
		if (current_state==ENGINE_RUN) send_sms(50);
		//Если двигатель не запустился, отправляем сообщение, выключаем зажигание
		if (current_state==ENGINE_STOP)
		{
			send_sms(51);
			relay_ignition_set_state(0);
		}
	}
}

void sserial_process_request(unsigned char portindex)
{
	//Напряжение батареи
	if (sserial_request.command==4)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		adc_init_voltage_input();
		//Данные с АЦП состоят из пяти символов. Переслать можно только три символа, делим на 100
		sserial_response.data[0]=get_voltage_battery()/100;
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
		engine_run_sms=0;
		send_sms(52);
	}
	if (sserial_request.command==8)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		if (sserial_request.data[0]<255)	indicator_set_state(sserial_request.data[0]);
		sserial_response.data[0]=1;	
		sserial_response.datalength=1;
		sserial_send_response();
	}
	if (sserial_request.command==9)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		if (sserial_request.data[0]<255)	indicator_set_state(sserial_request.data[0]);
		sserial_response.data[0]=1;	
		sserial_response.datalength=1;
		sserial_send_response();
	}
	//Запустить двигатель на 10
	if (sserial_request.command==10)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_start_engine_by_sms();
	}
	//Запустить двигатель на 15
	if (sserial_request.command==15)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_start_engine_by_sms();
	}
	//Запустить двигатель на 20
	if (sserial_request.command==20)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_start_engine_by_sms();
	}
	//Запустить двигатель на 25
	if (sserial_request.command==25)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_start_engine_by_sms();
	}
	//Запустить двигатель на 30
	if (sserial_request.command==30)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_start_engine_by_sms();
	}
}

void get_bluetooth_data()
{
	static int counter=0;
	static byte current_state_label=0;
	current_state_label=bluetooth_data_in();

	if (current_state_label != bluetooth_label_presence)
	{
		counter++;
		if (counter>100) 
		{
			bluetooth_label_presence=current_state_label;
			counter=0;
		}
	}
}

//Тест
void led_on()
{
	sserial_request.command=8;
	sserial_request.data[0]=1;
	sserial_request.data[1]=0;
	sserial_request.datalength=2;
	volatile char result=sserial_send_request_wait_response(UART_485, 100);
	if (result==0)
	{
		string_clear();
		string_add_string("Board not respond");
	}else
	{
		string_add_string("LED on");
		indicator_set_state(0);
	}
}
//Тест
void led_off()
{
	sserial_request.command=8;
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
		indicator_set_state(1);
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
		if (button_start_engine_is_pressed()==1)
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

//Обратный отсчет времени после запуска по СМС
void count_during_work()
{
	static int counter_ms=0;
	static int counter_sec=0;

	if (during_work>0)
	{
		adc_init_voltage_acc();
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
			during_work--;
			counter_sec=0;
		}
		//Если ключ вставлен, то питание идет через замок зажигания, выключаем все.
		if (get_acc_voltage()>VOLTAGE_BAT_MINIMAL)
		{
			wdt_reset();
			relay_ignition_set_state(0);
			engine_run_sms=0;
			counter_ms=0;
			counter_sec=0;
		}
	}
	//Если время кончилось, переходим к остановке
	else
	{
		relay_ignition_set_state(0);
		current_state=ENGINE_STOP;
		send_sms(52);
		engine_run_sms=0;
	}
}

int main(void)
{
	wdt_enable(WDTO_4S);
	uart_init_withdivider(UART_USB,UBRR_VALUE);
	uart_init_withdivider(UART_485,UBRR_VALUE);

	device_init();

	indicator_set_state(1);
    while (1) 
    {
		wdt_reset();
		
//		get_bluetooth_data();
//		indicator_set_state(bluetooth_label_presence);
//		if (bluetooth_label_presence==1) get_state_start_button();
// 		uart_send_float(UART_USB,voltage_battery/1000,2);
// 		uart_send_string(UART_USB,"\r\n");
	 	
//		switch_led();
		if (current_state==ENGINE_RUN)
		{
			if (engine_run_sms==1) count_during_work();
			//if (get_voltage_battery()<VOLTAGE_RUN_ENGINE) current_state=ENGINE_STOP;
		}
 		sserial_poll_uart(UART_485);
		_delay_ms(1);

    }
}


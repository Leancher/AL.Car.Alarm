//Atmel Studio Firmware Car alarm v1

#include "board/board.h"

//#METHODS
#define DEV_NAME "Car alarm 1.0"
#define ADC_VOLT_MULTIPLIER_MV		(68+2.2)/2.2 * 1.1
#define DELAY_IGNITION_INIT 3000 //3000 ms / 50 cycles
#define DURING_STARTER_WORK 2000 //2000 ms / 50 cycles
#define VOLTAGE_RUNNING_ENGINE 12500


typedef enum
{
	ENGINE_STOP=1,
	IGNITION_INIT=2,
	ENGINE_STARTING=3,
	ENGINE_RUN=4,
	ENGINE_STOPPING=5,
} DEVICE_STATE;
DEVICE_STATE _current_state = ENGINE_STOP;

uint16_t voltage_battery=0;

void device_init()
{
	button_start_engine_enable();
	relay_ignition_set_state(0);
	relay_starter_set_state(0);
}

void get_input_voltage()
{
	adc_init(0, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
	//uint16_t val=adc_read_once()
	voltage_battery = adc_read_once()*ADC_VOLT_MULTIPLIER_MV;
}

void get_state_start_button()
{
	static int index=0;
	index++;	
	if (index>300)
	{
		if (button_start_engine_is_pressed()==1)
		{
			if (_current_state==IGNITION_INIT) _current_state=ENGINE_STOPPING;
			if (_current_state==ENGINE_STOP) _current_state=IGNITION_INIT;
			if (_current_state==ENGINE_RUN) _current_state=ENGINE_STOPPING;
		}
		index=0;	
	}
}



void stop_engine()
{
	relay_ignition_set_state(0);
	indicator_set_state(0);
	_current_state=ENGINE_STOP;
	_delay_ms(5000);
	indicator_set_state(1);
}

void ignition_turn_on()
{
	static int index = 0;
	relay_ignition_set_state(1);
	index++;
	if (index<DELAY_IGNITION_INIT) return;
	index=0;
	_current_state=ENGINE_STARTING;
}

void send_result()
{
	sserial_request.command=14;
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

void start_engine()
{
	//static int _conter_try = 0;
	static int index = 0;
	relay_starter_set_state(1);
	//Если стартер включен долгое время, а двигатель не заведен, выключаем
	if (index>DURING_STARTER_WORK)
	{
		relay_starter_set_state(0);
		_current_state=ENGINE_STOPPING;
		index=0;
		return;	
	}
	index++;
	if (voltage_battery<VOLTAGE_RUNNING_ENGINE) return;	
	relay_starter_set_state(0);
	_current_state=ENGINE_RUN;
	indicator_set_state(1);
	send_result();
}

void sserial_process_request(unsigned char portindex)
{
	//Напряжение батареи
	if (sserial_request.command==11)
	{
		sserial_response.result=128+sserial_request.command;
		//voltage_battery=1234;
		sserial_response.data[0]=voltage_battery/100;
		sserial_response.datalength=1;
		sserial_send_response();
	}
	//Запустить двигатель
	if (sserial_request.command==12)
	{
		sserial_response.result=128+sserial_request.command;
		if (_current_state==ENGINE_RUN) sserial_response.data[0]=ENGINE_RUN;
		if (_current_state==IGNITION_INIT) sserial_response.data[0]=ENGINE_RUN;
		if (_current_state==ENGINE_STOP)
		{
			_current_state=IGNITION_INIT;
			sserial_response.data[0]=ENGINE_STARTING;
		}
		sserial_response.datalength=1;
		sserial_send_response();
	}
	//Остановить двигатель
	if (sserial_request.command==13)
	{
		sserial_response.result=128+sserial_request.command;
		_current_state=ENGINE_STOPPING;
		sserial_response.data[0] = ENGINE_STOP;
		sserial_response.datalength=1;
		sserial_send_response();
	}
	if (sserial_request.command==19)
	{
		sserial_response.result=128+sserial_request.command;
		if (sserial_request.data[0]<255)	indicator_set_state(sserial_request.data[0]);
		sserial_response.data[0]=1;	
		sserial_response.datalength=1;
		sserial_send_response();
	}
	if (sserial_request.command==20)
	{
		sserial_response.result=128+sserial_request.command;
		if (sserial_request.data[0]<255)	indicator_set_state(sserial_request.data[0]);
		sserial_response.data[0]=1;	
		sserial_response.datalength=1;
		sserial_send_response();
	}
}

void led_on()
{
	sserial_request.command=19;
	sserial_request.data[0]=1;
	sserial_request.data[1]=0;
	sserial_request.datalength=2;
	volatile char result=sserial_send_request_wait_response(UART_485, 100);
	if (result==0)
	{
		string_clear();
		string_add_string("PowerBoard not respond");
	}else
	{
		string_add_string("LED on");
		indicator_set_state(0);
	}
}

void led_off()
{
	sserial_request.command=20;
	sserial_request.data[0]=0;
	sserial_request.data[1]=0;
	sserial_request.datalength=2;
	volatile char result=sserial_send_request_wait_response(UART_485, 100);
	if (result==0)
	{
		string_clear();
		string_add_string("PowerBoard not respond");
	}
	else
	{
		string_add_string("LED off");
		indicator_set_state(1);
	}
}

void get_button_state()
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

int main(void)
{
	uart_init_withdivider(UART_USB,UBRR_VALUE);
	uart_init_withdivider(UART_485,UBRR_VALUE);
	indicator_set_state(1);

	device_init();

    while (1) 
    {
		wdt_reset();
		//get_button_state();
		get_input_voltage();
// 		uart_send_float(UART_USB,voltage_battery/1000,2);
// 		uart_send_string(UART_USB,"\r\n");
 		get_state_start_button();
		if (_current_state==IGNITION_INIT) ignition_turn_on();
		if (_current_state==ENGINE_STARTING) start_engine();
		if (_current_state==ENGINE_STOPPING) stop_engine();
		sserial_poll_uart(UART_485);
		_delay_ms(1);

    }
}


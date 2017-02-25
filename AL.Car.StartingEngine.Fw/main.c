//Atmel Studio Firmware Starting Engine board v1.1

#include "board/board.h"
#include "config.h"
#include <avr/wdt.h>

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
//1 - ���� ��������� ������� � ������� ���
byte engine_run_sms=0;

void sserial_send_start(unsigned char portindex)
{
	if (portindex==UART_485){
		rs485_send_start();
	}
}

void sserial_send_end(unsigned char portindex)
{
	if (portindex==UART_485)
	{
		var_delay_ms(1);
		rs485_send_end();
	}
}

void device_init()
{
	set_unused_pin();
	board_button_enable();
	relay_ignition_set_state(0);
	relay_starter_set_state(0);
}

int get_acc_voltage()
{
	uint16_t val=0;
	val=adc_read_average(3);
	val=val*ADC_VOLT_MULTIPLIER_MV;//+ADC_DIODE_CORRECTION;
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

int get_voltage()
{
	uint16_t val=0;
	val=adc_read_average(3);
	val=val*ADC_VOLT_MULTIPLIER_MV;//+ADC_DIODE_CORRECTION;
	return val;
}

//50 - �������� ������
//51 - ��������� �� ���� �����������
//52 - ��������� ����������
//53 - ��������� ��� �������
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

//�������� ���������
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
	while (get_voltage()<VOLTAGE_STARTER_STOP)
	{
		//���� ������� ������� ������ �����, � ��������� �� �������, ���������
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

//��������� ������� �� ������ ���������
void process_start_engine_by_sms()
{
	adc_init_voltage_generator();
	if (get_voltage()>VOLTAGE_RUN_ENGINE)
	{
		//��������� ��� �������
		current_state=ENGINE_RUN;
		sserial_response.data[0]=ENGINE_ALREADY_STARTED;
		sserial_response.datalength=1;
		sserial_send_response();
		send_sms(53);
		return;
	}
	//��������� ��������, ���������� �����
	if (sserial_request.data[0]<255)	during_work = sserial_request.data[0];
	during_work=2;
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
		//���� ��������� ����������, ���������� ���������
		if (current_state==ENGINE_RUN) send_sms(50);
		//���� ��������� �� ����������, ���������� ���������, ��������� ���������
		if (current_state==ENGINE_STOP)
		{
			send_sms(51);
			relay_ignition_set_state(0);
		}
	}
}

void sserial_process_request(unsigned char portindex)
{
	//���������� �������
	if (sserial_request.command==4)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		adc_init_vol_power_in();
		//������ � ��� ������� �� ���� ��������. ��������� ����� ������ ��� �������, ����� �� 100
		sserial_response.data[0]=get_voltage()/100;
		sserial_response.datalength=1;
		sserial_send_response();
		adc_off();
	}

	//���������� ���������
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
	//��������� ��������� �� 10
	if (sserial_request.command==10)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_start_engine_by_sms();
	}
	//��������� ��������� �� 15
	if (sserial_request.command==15)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_start_engine_by_sms();
	}
	//��������� ��������� �� 20
	if (sserial_request.command==20)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_start_engine_by_sms();
	}
	//��������� ��������� �� 25
	if (sserial_request.command==25)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_start_engine_by_sms();
	}
	//��������� ��������� �� 30
	if (sserial_request.command==30)
	{
		wdt_reset();
		sserial_response.result=128+sserial_request.command;
		process_start_engine_by_sms();
	}
}

//����
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
		//board_led_set_state(0);
	}
}
//����
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
		//board_led_set_state(1);
	}
}
//����
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

//�������� ������ ������� ����� ������� �� ���
void count_during_work()
{
	static int counter_ms=0;
	static int counter_sec=0;

	if (during_work>0)
	{
		adc_init_vol_dig_in();
		counter_ms++;
		//������� �����������
		if (counter_ms>1000)
		{
			counter_sec++;
			counter_ms=0;
		}
		//������� �������
		if (counter_sec>60)
		{
			during_work--;
			counter_sec=0;
		}
		//���� ���� ��������, �� ������� ���� ����� ����� ���������, ��������� ���.
// 		if (get_acc_voltage()>VOLTAGE_BAT_MINIMAL)
// 		{
// 			wdt_reset();
// 			relay_ignition_set_state(0);
// 			engine_run_sms=0;
// 			counter_ms=0;
// 			counter_sec=0;
// 		}
	}
	//���� ����� ���������, ��������� � ���������
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
	uart_init_withdivider(UART_BT,UBRR_VALUE);
	uart_init_withdivider(UART_485,UBRR_VALUE);

	device_init();

	board_led_set_state(1);
    while (1) 
    {
		wdt_reset();
	 	
		//switch_led();
		if (current_state==ENGINE_RUN)
		{
			if (engine_run_sms==1) count_during_work();
			//if (get_voltage_battery()<VOLTAGE_RUN_ENGINE) current_state=ENGINE_STOP;
		}
 		sserial_poll_uart(UART_485);
		_delay_ms(1);
    }
}


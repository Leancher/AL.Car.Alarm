//Atmel Studio Firmware Car alarm v1

#include "board/board.h"

//#METHODS
#define DEV_NAME "Car alarm 1.0"
#define ADC_VOLT_MULTIPLIER_MV		(68+2.2)/2.2 * 1.1

typedef enum
{
	ENGINE_STOP=1,
	IGNITION_INIT=2,
	ENGINE_STARTING=3,
	ENGINE_RUN=4,
	ENGINE_STOPPING=5,
} DEVICE_STATE;

DEVICE_STATE _current_state = ENGINE_STOP;

void device_init()
{
	button_start_engine_enable();
	relay_ignition_set_state(0);
	relay_starter_set_state(0);
}

float get_input_voltage()
{
	adc_init(0, ADC_ADJUST_RIGHT, ADC_REFS_INTERNAL_1_1,  ADC_PRESCALER_32);
	uint16_t val=adc_read_average(3);
	return val*ADC_VOLT_MULTIPLIER_MV/1000+0.5;
}

void get_state_start_button()
{
	static int index=0;
	index++;	
	if (index>2)
	{
		if (button_start_engine_is_pressed()==1)
		{
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
	if (index<20) return;
	index=0;
	_current_state=ENGINE_STARTING;
}

void start_engine()
{
	static int _conter_try = 0;
	static int index = 0;
	relay_starter_set_state(1);
	if (index>20)
	{
		relay_starter_set_state(0);
		_current_state=ENGINE_STOPPING;
		index=0;
		return;	
	}
	index++;
	if (get_input_voltage()<13.3) return;	
	relay_starter_set_state(0);
	_current_state=ENGINE_RUN;
	indicator_set_state(1);
}



int main(void)
{
	//#INIT
	uart_init_withdivider(0,UBRR_VALUE);
	uart_send_string(0,DEV_NAME);
	uart_send_string(0,"\r\n");
	indicator_set_state(1);
	device_init();

    while (1) 
    {
		uart_send_float(0,get_input_voltage(),2);
		uart_send_string(0,"\r\n");
		get_state_start_button();
		if (_current_state==IGNITION_INIT) ignition_turn_on();
		if (_current_state==ENGINE_STARTING) start_engine();
		if (_current_state==ENGINE_STOPPING) stop_engine();
 		_delay_ms(100);

    }
}


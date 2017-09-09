#ifndef CONTROL_ENGINE_H_
#define CONTROL_ENGINE_H_

#include "../board/board.h"

#define ADC_VOLT_MULTIPLIER_MV		(68+2.2)/2.2 * 1.1 //35.1
#define ADC_DIODE_CORRECTION 600

#define DELAY_IGNITION_INIT 30 //30 * 100 ms = 3000 ms
#define DURING_STARTER_WORK 2000 //2000 ms
#define DELAY_BEFOR_NEXT_START 5000
#define VOLTAGE_STARTER_STOP 12500//12500 в машине
#define VOLTAGE_RUN_ENGINE 13000
#define VOLTAGE_BAT_MINIMAL 9000

typedef unsigned char byte;

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
DEVICE_STATE current_state;

//1 - если двигатель запущен с помощью СМС
byte remote_running;
int number_minutes_work;

void send_sms(int number_command);
void ignition_turn_on();
void starter_control();
void start_engine();
void get_state_start_button();
void process_command_start();
void process_command_control_engine();
void count_minutes_work();
void process_running_engine();

void led_on();
void led_off();
void switch_led();

#endif /* CONTROL_ENGINE_H_ */
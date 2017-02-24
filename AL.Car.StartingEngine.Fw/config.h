/*
 * config.h
 *
 * Created: 24.02.2017 17:05:18
 *  Author: Andrew
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#define DEV_NAME "Car.StartingEngine 1.1"
#define ADC_VOLT_MULTIPLIER_MV		(68+2.2)/2.2 * 1.1 //35.1
#define ADC_DIODE_CORRECTION 600
#define DELAY_IGNITION_INIT 30 //30 * 100 ms = 3000 ms
#define DURING_STARTER_WORK 2000 //2000 ms
#define DELAY_BEFOR_NEXT_START 5000
#define VOLTAGE_STARTER_STOP 12500//12500 в машине
#define VOLTAGE_RUN_ENGINE 14000//13000 в машине
#define VOLTAGE_BAT_MINIMAL 9000
#define ADC_IN_GENERATOR_VOLTAGE 0//2


#endif /* CONFIG_H_ */
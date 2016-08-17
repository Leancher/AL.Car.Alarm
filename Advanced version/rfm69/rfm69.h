#ifndef __RFM69_H
#define __RFM69_H

#include <stdint.h>
typedef unsigned char byte;

#include "rfm69registers.h"
#include "rfpackets.h"

#define rfm69_freq_start 868.8
#define rfm69_freq_end 869.1
#define rfm69_freq_koeff 1000000.0/61.03515625
unsigned char rfm69_power_level;
unsigned char rfm69_power_pa_mode;

void rfm69_reset();
void rfm69_normal_init();
void rfm69_sdr_init();
void rfm69_sleep();

uint32_t rfm69_freq2syntvalue(double freq);
void rfm69_set_syntfreq(uint32_t syntvalue);
void rfm69_set_params(int deviation, int rxbw, int bitrate, int power);

void rfm69_send(const byte* buffer, byte start, byte bufferSize);
void rfm69_sdr_send(const byte* buffer, byte start, byte bufferSize);
void rfm69_send_preamble(byte length);
void rfm69_writereg(byte addr, byte value);
byte rfm69_readreg(byte addr);

byte rfm69_check_receive();
void rfm69_set_encrypt(const char* key);

int rfm69_read_rssi();
int rfm69_read_fei();
byte rfm69_read_temp(); 
void rfm69_rccal(); 

#define MAX_DATA_LEN        61

#define RFM69_FEIMETHOD_OFF 0
#define RFM69_FEIMETHOD_ON_DATA 10
#define RFM69_FEIMETHOD_ON_NEXT_PREAMBLE 20

struct
{
	byte data[MAX_DATA_LEN];	
	byte data_len;
	int rssi;			
	int fei_value;
	byte fei_valid;
} rfm69_data;

struct
{
	byte mode;		
} rfm69_internal;

struct
{
	byte fei_method;
	byte fei_precious;
	byte fei_postamble_delay;
} rfm69_settings;

#endif
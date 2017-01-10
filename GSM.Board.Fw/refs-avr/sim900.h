#ifndef SIM900_H_
#define SIM900_H_

#define GSM_RECEIVED_BUFFER_LENGTH 128
#define GSM_SMS_BUFFER_LENGTH 64
#define GSM_ERROR_NONE 0
#define GSM_ERROR_NOT_RESPOND 1
#define GSM_ERROR_NOT_POWERED 2
#define GSM_ERROR_NOT_CONFIGURED 3
#define GSM_ERROR_WRONG_MODULE 4
#define GSM_ERROR_NOT_OPERATIONAL 5

typedef unsigned char byte;

//настройки
#define UART_GSM 2

//данные
char gsm_received_sms_text[GSM_SMS_BUFFER_LENGTH];
char gsm_received_sms_phone[14];
char gsm_working;
char gsm_error_code;

//вызывать
void gsm_init();
void gsm_poll();
void gsm_send_sms(char* phone,char* message);
void gsm_checkstate();
void gsm_powerkey_on();
void gsm_get_time_line(char *buffer);

//должны быть реализованы
void gsm_received_sms();

//должны быть реализованы
char gsm_statusled_get();
void gsm_powerkey();
void var_delay_ms(int ms);
void uart_send(unsigned char port, unsigned char byte );
unsigned char uart_get(unsigned char port );
unsigned char uart_received( unsigned char port );
char gsm_gprs_connect();
char gsm_http_get_request(char *url);
char gsm_check_gprs_state();
void gsm_get_location();

#define getbit(port, bit)		((port) &   (1 << (bit)))
#define setbit(port,bit,val)	{if ((val)) {(port)|= (1 << (bit));} else {(port) &= ~(1 << (bit));}}
#include <avr/io.h>

#endif /* SIM900_H_ */

#ifndef GSM_H_
#define GSM_H_

#include "../board/board.h"

#define WATCHDOG_SEC_TO_RESET		30
#define WATCHDOG_SEC_TO_RESET_PAUSE	60*30
#define WATCHDOG_SEC_TO_RESET_FIRST	60*5
#define WATCHDOG_SEC_TO_POWERON		10

//состояние
byte		gsm_present;
byte		gsm_connected;

//вызывать
void		gsm_update();			
void		sserial_process_gsm();	//из основного sserial_process

//должны быть определены

#endif /* GSM_H_ */
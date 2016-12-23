//Atmel Studio Firmware #PRJNAME

#include "board/board.h"

//#METHODS

int main(void)
{
	//#INIT
    while (1) 
    {
		//#CYCLE
		DDRB=0b00100000;
		PORTB=0b00100000;
    }
}


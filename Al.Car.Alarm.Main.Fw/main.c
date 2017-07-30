#include "board/board.h"

void board_init()
{
	relay_ignition_set_state(0);
	relay_starter_set_state(0);
	board_button_enable();
	sensor_ignition_key_enable();
	board_led_set_state(1);
	setbit(UCSR1B,RXCIE1,1);
	sei();
}

ISR(USART1_RX_vect)
{
	cli();
	sserial_poll_uart(UART_485);
	sei();
}

ISR(BADISR_vect)
{
	while(1);
}

void sserial_process_request(unsigned char portindex)
{
	process_command_control_engine();
}

int main(void)
{
	wdt_enable(WDTO_4S);
	uart_init_withdivider(UART_485,UBRR_VALUE);
	board_init();
    while (1) 
    {
		wdt_reset();
	 	//get_state_start_button();
		switch_led();
		process_running_engine();
    }
}


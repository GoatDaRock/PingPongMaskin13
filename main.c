#include <avr/io.h>
#include "uart.h"

int main(void)
{
    uart_init();

    while (1)
    {
        uart_transmit('A');
    }
	return 0;
}
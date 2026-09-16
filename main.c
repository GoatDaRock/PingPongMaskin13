#include <avr/io.h>
#include <stdio.h>
#include "uart.h"
#include "test.h"
#include "sram.h"
#include "adc.h"

int main(void)
{
    uart_init();
    fdevopen(uart_putchar, uart_getchar);

   
	xmem_init();

    SRAM_test();
	
	adc_init();
	while (1)
    {
        adc_data_t adc = adc_read_all();

        printf("JOY_X: %u  JOY_Y: %u  PAD_X: %u  PAD_Y: %u\r\n",
               adc.joy_x,
               adc.joy_y,
               adc.pad_x,
               adc.pad_y);

        // Just slow down terminal printing
        for (volatile uint32_t i = 0; i < 50000; i++)
        {
        }
    }
	return 0;
}
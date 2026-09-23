// #include <avr/io.h>
// #include <stdio.h>
// #include "uart.h"
// #include "test.h"
// #include "sram.h"
// #include "adc.h"

// int main(void)
// {
//     uart_init();
//     fdevopen(uart_putchar, uart_getchar);

   
// 	xmem_init();

//     SRAM_test();
	
// 	adc_init();
// 	while (1)
//     {
//         adc_data_t adc = adc_read_all();

//         printf("JOY_X: %u  JOY_Y: %u  PAD_X: %u  PAD_Y: %u\r\n",
//                adc.joy_x,
//                adc.joy_y,
//                adc.pad_x,
//                adc.pad_y);

//         // Just slow down terminal printing
//         for (volatile uint32_t i = 0; i < 50000; i++)
//         {
//         }
//     }
// 	return 0;
// }

#include <avr/io.h>

#include "spi.h"
#include "oled.h"
#include "adc.h"
#include <stdbool.h>
#include "uart.h"

int main(void)
{
	uart_init();
    fdevopen(uart_putchar, uart_getchar);

	adc_init();

    /*
     * Initialize ATmega162 SPI.
     */
    spi_init();


    /*
     * Initialize SSD1309 OLED.
     */
    oled_init();




	char options[3][10] = {"OPTION 1", "OPTION 2", "OPTION 3"};

	int selected = 0;
	int number_of_options = 3;
	bool menu_can_change = true;

	write_display:
		/*
		* Clear old contents.
		*/
		oled_clear();


		/*
		* LINE 0
		*/
		oled_goto_line(0);
		oled_goto_column(0);

		oled_printf("USER INTERFACE:");
		for (int i = 0; i < number_of_options; i++){
			oled_goto_line(i+1);
			oled_goto_column(0);
			if (selected == i){
				oled_printf("* ");
			}
			oled_printf(options[i]);
		}

    while (1)
    {

		adc_data_t adc = adc_read_all();
		uint8_t adc_y = adc.joy_y;
		printf("joy_y: %u\r\n", adc_y);
		for (volatile uint32_t i = 0; i < 50000; i++){}

		if (!menu_can_change){
			if (adc_y > 100 && adc_y < 200){
				menu_can_change = true;
			}
			continue;
		}
		if (adc_y > 200 && selected < number_of_options -1) {
			printf("select next\n");
			selected++;
			menu_can_change = false;
			goto write_display;
		}
		if (adc_y < 100 && selected > 0){
			printf("select previous\n");
			selected--;
			menu_can_change = false;
			goto write_display;
		}
    }


    return 0;
}
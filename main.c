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
#include "sram.h"
#include "io.h"
#include "can.h"

int main(void)
{
	uart_init();
    fdevopen(uart_putchar, uart_getchar);

	xmem_init();
	adc_init();

    /*
     * Initialize ATmega162 SPI.
     */
    spi_init();
	
	
    printf("CAN initialized in loopback mode\r\n");
	can_init();


    


    /*
     * Create CAN message.
     */
    can_message_t message;

    message.id = 0x05;
    message.length = 3;

    message.data[0] = 0x11;
    message.data[1] = 0x22;
    message.data[2] = 0x33;


    /*
     * Send CAN message.
     */
    printf("Sending CAN message...\r\n");

    can_send_message(message);


    /*
     * In loopback mode, the MCP2515
     * should receive its own message.
     *
     * can_receive_message() waits until
     * RXB0 contains a message.
     */
    can_message_t received_message;

    received_message = can_receive_message();
	printf("Waiting for CAN message...\r\n");


    /*
     * Print received CAN message.
     */
    printf("Received CAN message:\r\n");

    printf("ID     = 0x%X\r\n",
           received_message.id);

    printf("Length = %u\r\n",
           received_message.length);

    printf("Data   = ");

    for (uint8_t i = 0;
         i < received_message.length;
         i++)
    {
        printf("0x%02X ",
               received_message.data[i]);
    }

    printf("\r\n");


    /*
     * Nothing else to do.
     */
    while (1)
    {
    }


    return 0;
	/*
     * Initialize SSD1309 OLED.
     */
    oled_init();

	io_joystick_t joystick;
    io_buttons_t buttons;



	char options[5][10] = {"OPTION 1", "OPTION 2", "OPTION 3", "OPTION 4", "OPTION 5"};

	int selected = 0;
	int number_of_options = 5;
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

		oled_printf("USER INTERFACE: (%u)", selected+1);
		for (int i = 0; i < number_of_options; i++){
			oled_goto_line(i+1);
			oled_goto_column(0);
			if (selected == i){
				oled_printf("* ");
			} else {
				oled_printf(". ");
			}
			oled_printf(options[i]);
		}

    while (1)
    {

		adc_data_t adc = adc_read_all();
		uint8_t adc_y = adc.joy_y;

		joystick = io_read_joystick();

        buttons = io_read_buttons();

		oled_goto_line(6);
        oled_goto_column(0);
		oled_printf("JOY B: %3u  ", joystick.btn);

		oled_goto_line(7);
        oled_goto_column(0);
		oled_printf("LEFT:  %u , RIGHT: %u", buttons.left, buttons.right);

		if (!menu_can_change){
			if (adc_y > 120 && adc_y < 200){
				menu_can_change = true;
			}
			continue;
		}
		if (adc_y > 200 && selected > 0) {
			selected--;
			menu_can_change = false;
			goto write_display;
		}
		if (adc_y < 120 && selected < number_of_options-1){
			selected++;
			menu_can_change = false;
			goto write_display;
		}
    }


    return 0;
}

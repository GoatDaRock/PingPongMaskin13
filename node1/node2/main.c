#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "can.h"

#define JOYSTICK_CAN_ID 0x10
#define JOYSTICK_POSITION_MIN 70
#define JOYSTICK_POSITION_MAX 250	
/*	
 * Remember to update the Makefile with the (relative) path to the uart.c file.
 * This starter code will not compile until the UART file has been included in the Makefile. 
 * If you get somewhat cryptic errors referencing functions such as _sbrk, 
 * _close_r, _write_r, _fstat etc, you have most likely not done that correctly.

 * If you get errors such as "arm-none-eabi-gcc: no such file", you may need to reinstall the arm gcc packages using
 * apt or your favorite package manager.
 */
#include "uart.h"
#include "servo_driver.h"
#include "pwm_driver.h"

int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    //Uncomment after including uart above
    //uart_init(/*cpufreq*/, /*baud*/);
    //printf("Hello World\n\r");
	uart_init(84000000, 9600);
	printf("Arduino Due CAN Test Program\r\n");
	
	can_init(CAN_NODE1_125K_TIMING, 0);

	if (!servo_init(1000, 2000, 84000000)) { // Initialize servo with min pulse width 1000us, max pulse width 2000us, and clock frequency 1MHz
		printf("Failed to initialize servo\r\n");
		return -1;
	}


	CanMsg message;
	// wait to receive message
	printf("Waiting for CAN message...\r\n");
	while (!can_rx(&message))
	{
		
	}

	
	can_printmsg(message);
	//printf("Hello World\n\r");

	uint8_t joystick_x;
	uint8_t joystick_y;

    while (1)
    {
		//printf("Hello World\n\r");
		while (!can_rx(&message))
		{
		}
		if (message.id == JOYSTICK_CAN_ID){
			joystick_x = message.byte[0];
			joystick_y = message.byte[1];
			// Map joystick position to servo position (0.0 to 1.0)
			float servo_x = (float)(joystick_x - JOYSTICK_POSITION_MIN) / (float)(JOYSTICK_POSITION_MAX - JOYSTICK_POSITION_MIN);
			float servo_y = (float)(joystick_y - JOYSTICK_POSITION_MIN) / (float)(JOYSTICK_POSITION_MAX - JOYSTICK_POSITION_MIN);

			// Set servo position
			if (servo_set_position(servo_x)) {
				printf("Set servo X position to %f\r\n", servo_x);
			}
		}
    }
}


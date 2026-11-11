// #include <stdio.h>
// #include <stdarg.h>
// #include "sam.h"
// #include "can.h"

// #define JOYSTICK_CAN_ID 0x10
// #define JOYSTICK_POSITION_MIN 70
// #define JOYSTICK_POSITION_MAX 250	
// /*	
//  * Remember to update the Makefile with the (relative) path to the uart.c file.
//  * This starter code will not compile until the UART file has been included in the Makefile. 
//  * If you get somewhat cryptic errors referencing functions such as _sbrk, 
//  * _close_r, _write_r, _fstat etc, you have most likely not done that correctly.

//  * If you get errors such as "arm-none-eabi-gcc: no such file", you may need to reinstall the arm gcc packages using
//  * apt or your favorite package manager.
//  */
// #include "uart.h"
// #include "servo_driver.h"
// #include "pwm_driver.h"


// uint8_t goals = 0;

// int main()
// {
//     SystemInit();

//     WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

//     //Uncomment after including uart above
//     //uart_init(/*cpufreq*/, /*baud*/);
//     //printf("Hello World\n\r");
// 	uart_init(84000000, 9600);
// 	printf("Arduino Due CAN Test Program\r\n");
	
// 	can_init(CAN_NODE1_125K_TIMING, 0);

// 	if (!servo_init(1000, 2000, 84000000)) { // Initialize servo with min pulse width 1000us, max pulse width 2000us, and clock frequency 1MHz
// 		printf("Failed to initialize servo\r\n");
// 		return -1;
// 	}


// 	CanMsg message;
// 	// wait to receive message
// 	printf("Waiting for CAN message...\r\n");
// 	while (!can_rx(&message))
// 	{
		
// 	}
	
	
// 	can_printmsg(message);
// 	//printf("Hello World\n\r");

// 	uint8_t joystick_x;
// 	uint8_t joystick_y;

//     while (1)
//     {
// 		//printf("Hello World\n\r");
// 		while (!can_rx(&message))
// 		{
// 		}
// 		if (message.id == JOYSTICK_CAN_ID){
// 			joystick_x = message.byte[0];
// 			joystick_y = message.byte[1];
// 			// Map joystick position to servo position (0.0 to 1.0)
// 			float servo_x = (float)(joystick_x - JOYSTICK_POSITION_MIN) / (float)(JOYSTICK_POSITION_MAX - JOYSTICK_POSITION_MIN);
// 			float servo_y = (float)(joystick_y - JOYSTICK_POSITION_MIN) / (float)(JOYSTICK_POSITION_MAX - JOYSTICK_POSITION_MIN);

// 			// Set servo position
// 			if (servo_set_position(servo_x)) {
// 				printf("Set servo X position to %f\r\n", servo_x);
// 			}
			
// 		}
//     }
// }




// #include <stdio.h>
// #include <stdint.h>
// #include <stdbool.h>

// #include "sam.h"
// #include "can.h"
// #include "uart.h"
// #include "servo_driver.h"
// #include "pwm_driver.h"

// /* Node 1 sends the joystick X and Y values in bytes 0 and 1. */
// #define JOYSTICK_CAN_ID        0x10u
// #define JOYSTICK_MIN           70
// #define JOYSTICK_MAX           250
// #define JOYSTICK_CENTER        160
// #define JOYSTICK_DEADBAND      5

// /* Motor shield: PWM channel 0 -> PB12; direction -> PC23. */
// #define MOTOR_PWM_CHANNEL      0u
// #define MOTOR_PWM_PERIOD       1000u   /* 1 MHz PWM clock / 1000 = 1 kHz */
// #define MOTOR_DIR_PIN          (1u << 23)

// static float joystick_to_position(uint8_t value)
// {
//     float position = ((float)value - (float)JOYSTICK_MIN) /
//                      (float)(JOYSTICK_MAX - JOYSTICK_MIN);

//     if (position < 0.0f) position = 0.0f;
//     if (position > 1.0f) position = 1.0f;
//     return position;
// }

// static bool motor_init(void)
// {
//     /* PWM has already been initialized by servo_init(). */
//     PMC->PMC_PCER0 = (1u << ID_PIOC);

//     /* PC23: ordinary GPIO output for the motor direction. */
//     PIOC->PIO_PER = MOTOR_DIR_PIN;
//     PIOC->PIO_CODR = MOTOR_DIR_PIN;
//     PIOC->PIO_OER = MOTOR_DIR_PIN;

//     /* PWM channel 0 (PB12) for the motor enable input. */
//     return pwm_start(MOTOR_PWM_CHANNEL, MOTOR_PWM_PERIOD, 0u);
// }

// static void motor_set_joystick(uint8_t x)
// {
//     int32_t displacement = (int32_t)x - JOYSTICK_CENTER;

//     if (displacement >= -JOYSTICK_DEADBAND &&
//         displacement <=  JOYSTICK_DEADBAND)
//     {
//         pwm_write_duty(MOTOR_PWM_CHANNEL, 0u);
//         return;
//     }

//     /* Select direction. Stop command first when switching direction. */
//     if (displacement > 0)
//     {
//         PIOC->PIO_SODR = MOTOR_DIR_PIN;
//     }
//     else
//     {
//         PIOC->PIO_CODR = MOTOR_DIR_PIN;
//         displacement = -displacement;
//     }

//     uint32_t high_time = ((uint32_t)displacement * MOTOR_PWM_PERIOD) /
//                           (uint32_t)(JOYSTICK_MAX - JOYSTICK_CENTER);

//     if (high_time > MOTOR_PWM_PERIOD)
//     {
//         high_time = MOTOR_PWM_PERIOD;
//     }

//     pwm_write_duty(MOTOR_PWM_CHANNEL, high_time);
// }

// int main(void)
// {
//     SystemInit();
//     WDT->WDT_MR = WDT_MR_WDDIS;

//     uart_init(84000000u, 9600u);
//     printf("Node 2: joystick control\r\n");

//     can_init(CAN_NODE1_125K_TIMING, 0);

//     /* This calls the teacher's pwm_init() and starts servo PWM channel 1. */
//     if (!servo_init(1000u, 2000u, 84000000u))
//     {
//         printf("Servo initialization failed\r\n");
//         while (1) { }
//     }

//     if (!motor_init())
//     {
//         printf("Motor initialization failed\r\n");
//         while (1) { }
//     }

//     servo_neutral();
//     printf("Waiting for CAN joystick messages...\r\n");

//     CanMsg message;
//     while (1)
//     {
//         if (can_rx(&message))
//         {
//             if (message.id == JOYSTICK_CAN_ID && message.length >= 2u)
//             {
//                 uint8_t joystick_x = message.byte[0];
//                 uint8_t joystick_y = message.byte[1];

//                 /* Y controls the servo position (PWM channel 1). */
//                 servo_set_position(joystick_to_position(joystick_y));

//                 /* X controls DC motor speed and direction (PWM channel 0). */
//                 motor_set_joystick(joystick_x);
//             }
//         }
//     }
// }
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "sam.h"
#include "uart.h"
#include "can.h"
#include "servo_driver.h"
#include "pwm_driver.h"
#include "ir_driver.h"
#include "encoder_driver.h"
#include "motor_controller.h"
#include "motor_driver.h"
#include "solenoid_driver.h"

#define ADC_CHANNEL 7u
#define JOYSTICK_CAN_ID 0x10
#define JOYSTICK_MIN 70
#define JOYSTICK_MAX 250

// Replace with measured ADC thresholds
#define IR_BLOCKED_THRESHOLD 100
#define IR_CLEAR_THRESHOLD   1000

#define encoder_limit_high 2800
#define encoder_limit_low 0

#define SOLENOID_COOLDOWN_MS 1000

// int main(void)
// {
// 	SystemInit();

// 	WDT->WDT_MR = WDT_MR_WDDIS; // Disable Watchdog Timer
// 	uart_init(84000000, 9600);
	
	
// 	

// 	while (1)
// {
//     printf("Encoder: %lu\r\n",
//            (unsigned long)encoder_read());

    
// }


// }
//This code registers GOAL when the IR beam is blocked and counts the number of times the beam is blocked. It also sends the joystick position over CAN to Node 2, which controls a servo and a DC motor based on the joystick input.

static volatile uint32_t milliseconds = 0;
static uint32_t goals = 0;

void SysTick_Handler(void)
{
    milliseconds++;
}

static float normalize_joystick(uint8_t value)
{
    float position =
        ((float)value - JOYSTICK_MIN) /
        (JOYSTICK_MAX - JOYSTICK_MIN);

    if (position < 0.0f)
        return 0.0f;

    if (position > 1.0f)
        return 1.0f;

    return position;
}

static float normalize_encoder(uint32_t value)
{
	if (value > encoder_limit_high)
		return 1.0f;

	if (value < encoder_limit_low)
		return 0.0f;

	return (float)(value - encoder_limit_low) /
		   (float)(encoder_limit_high - encoder_limit_low);
}

int main(void)
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS;

    uart_init(84000000, 9600);
    can_init(CAN_NODE1_125K_TIMING, 0);

    if (!servo_init(1000, 2000, 84000000))
    {
        printf("Servo initialization failed\r\n");
        while (1) {}
    }

    servo_neutral();

    ir_init(IR_BLOCKED_THRESHOLD,
            IR_CLEAR_THRESHOLD);
	
	MotorController motor_controller;

	motor_controller_init(&motor_controller, 1.0f, 1.0f, 1.0f);
	motor_init();

	encoder_init();
	solenoid_init(PIOC, (1u << 22), false);

    // Configure 1 ms system tick
    SysTick_Config(84000000u / 1000u);

    printf("IR goal detection started\r\n");

    CanMsg message;

    uint32_t last_sample = milliseconds;
    uint32_t last_print = milliseconds;
	uint32_t last_solenoid_trigger = -SOLENOID_COOLDOWN_MS; // Initialize to allow immediate trigger

    // Require the beam to clear before counting
    bool previous_blocked = true;
	float joystick_x = 0.5f; // Initialize joystick_x to center position
    while (1)
    {
        // Existing CAN and servo control
		solenoid_update(milliseconds); // Update solenoid state based on time

        if (can_rx(&message))
        {
            if (message.id == JOYSTICK_CAN_ID &&
                message.length >= 3)
            {
                uint8_t joystick_y = message.byte[1];
				
				if (message.byte[2] == 1 && (int32_t)(milliseconds - last_solenoid_trigger) >= SOLENOID_COOLDOWN_MS) // Assuming button pressed is indicated by 1
				{
					solenoid_trigger(milliseconds, 10); // Trigger solenoid for 100 ms
					last_solenoid_trigger = milliseconds;

				}

                float servo_y =
                    normalize_joystick(joystick_y);

                servo_set_position(servo_y);

				joystick_x = 1 - normalize_joystick(message.byte[0]);

				
            }
        }
        // Sample the IR detector every 1 ms
        uint32_t now = milliseconds;

        if ((uint32_t)(now - last_sample) >= 1u)
        {
            last_sample = now;

            bool blocked = ir_update();

            if (blocked && !previous_blocked)
            {
                goals++;
				 // Optional: debounce delay to avoid multiple counts for a single goal
                printf("GOAL! Score: %lu\r\n",
                       (unsigned long)goals);
            }

            previous_blocked = blocked;

			unsigned long encoder_value = encoder_read(); // can integer overflow
			if (encoder_value  > encoder_limit_high*3)
			{
				encoder_value = 0; // Reset to 0 if it exceeds the high limit because of potential overflow
			}
			float motor_speed = motor_controller_update(&motor_controller, joystick_x, normalize_encoder(encoder_value), 0.001f); // Assuming dt = 1 ms = 0.001 s
			bool solenoid_active = solenoid_is_active();
			printf("Encoder: %f, target: %f, Motor Speed: %f, Solenoid Active: %s\r\n",
				normalize_encoder(encoder_value),
				joystick_x,
				motor_speed,
				solenoid_active ? "Yes" : "No");
			motor_set_speed(motor_speed * -1.0f);
        }

        // Print ADC value every 200 ms
        if ((uint32_t)(now - last_print) >= 200u)
        {
            last_print = now;
        }

		


    }
}







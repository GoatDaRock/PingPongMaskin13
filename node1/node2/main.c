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

#define START_GAME_CAN_ID 0x20

#define GAME_STATE_CAN_ID 0x30

// Replace with measured ADC thresholds
#define IR_BLOCKED_THRESHOLD 100
#define IR_CLEAR_THRESHOLD 1000

#define SOLENOID_COOLDOWN_MS 1000

static volatile uint32_t milliseconds = 0;

static volatile int32_t encoder_limit_low = 0;
static volatile int32_t encoder_limit_high = 0;


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

static float normalize_encoder(int32_t value)
{
    if (value > encoder_limit_high)
        return 1.0f;

    if (value < encoder_limit_low)
        return 0.0f;

    return (float)(value - encoder_limit_low) /
           (float)(encoder_limit_high - encoder_limit_low);
}

void send_game_state_over_can(uint8_t lives, uint8_t score)
{
    CanMsg message;
    message.id = GAME_STATE_CAN_ID;
    message.length = 2; // Sending two bytes: lives and score
    message.byte[0] = lives;
    message.byte[1] = score;

	printf("Sending game state over CAN: Lives=%u, Score=%u\r\n", lives, score);

    if (!can_tx(message))
    {
        printf("Failed to send game state over CAN\r\n");
    }
}

void encoder_set_limits(uint32_t low, uint32_t high)
{
	encoder_limit_low = low;
	encoder_limit_high = high;
}

uint8_t run_game(void)
{
    CanMsg message;

    MotorController motor_controller;

    uint32_t last_sample = milliseconds;

    motor_controller_init(&motor_controller, 1.0f, 1.0f, 0.5f);

    // move motor to right for 2 seconds and reset encoder to 0
    motor_set_speed(1.0f);

    while ((int32_t)(milliseconds - last_sample) < 2000)
    {
        // Wait for 2 seconds
    }

	motor_set_speed(0.0f); // Stop the motor

    encoder_init();

	last_sample = milliseconds;
	while ((int32_t)(milliseconds - last_sample) < 1000)
	{
		// Wait for 1000 ms to ensure encoder is stable
	}

	int32_t encoder_low = encoder_read();
	printf("Encoder low limit: %ld\r\n", encoder_low);

	// move motor to left for 2 seconds and read encoder value
	motor_set_speed(-1.0f);

	last_sample = milliseconds;

	while ((int32_t)(milliseconds - last_sample) < 2000)
	{
		// Wait for 2 seconds
	}

	motor_set_speed(0.0f); // Stop the motor

	int32_t encoder_high = encoder_read();
	printf("Encoder high limit: %ld\r\n", encoder_high);

	// set encoder limits 
	encoder_set_limits(encoder_low, encoder_high);

    uint8_t lives = 3; // Initialize lives
    uint8_t score = 0; // Initialize score

    uint32_t score_sample = milliseconds;
    uint32_t last_solenoid_trigger = -SOLENOID_COOLDOWN_MS; // Initialize to allow immediate trigger

    // Require the beam to clear before counting
    bool previous_blocked = true;
    float joystick_x = 0.5f; // Initialize joystick_x to center position

    while (lives > 0)
    {
        solenoid_update(milliseconds); // Update solenoid state based on time
        if (can_rx(&message))
        {
            if (message.id == JOYSTICK_CAN_ID &&
                message.length >= 3)
            {
                uint8_t joystick_y = message.byte[1];

                if (message.byte[2] == 1 && (int32_t)(milliseconds - last_solenoid_trigger) >= SOLENOID_COOLDOWN_MS) // Assuming button pressed is indicated by 1
                {
                    solenoid_trigger(milliseconds, 100); // Trigger solenoid for 100 ms
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
                lives--;
                send_game_state_over_can(lives, score);
            }

            previous_blocked = blocked;

            int32_t encoder_value = encoder_read(); // can integer overflow
            if (encoder_value > encoder_limit_high * 3)
            {
                encoder_value = 0; // Reset to 0 if it exceeds the high limit because of potential overflow
            }
            float motor_speed = motor_controller_update(&motor_controller, joystick_x, normalize_encoder(encoder_value), 0.001f); // Assuming dt = 1 ms = 0.001 s
			printf("Encoder: %.2f, Target: %.2f, Motor Speed: %.2f\r\n", (float)encoder_value, joystick_x, motor_speed);
            motor_set_speed(motor_speed * -1.0f);
        }

        if ((uint32_t)(now - score_sample) >= 1000u)
        {
            score_sample = now;
            score++;
            send_game_state_over_can(lives, score);
        }
    }
    solenoid_stop();
    motor_set_speed(0.0f);
    return score;
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
        while (1)
        {
        }
    }

    servo_neutral();

    ir_init(IR_BLOCKED_THRESHOLD,
            IR_CLEAR_THRESHOLD);

    motor_init();

    encoder_init();
    solenoid_init(PIOC, (1u << 22), false);

    // Configure 1 ms system tick
    SysTick_Config(84000000u / 1000u);

    uint8_t last_score = 0;

    CanMsg message;
	printf("Node 2 initialized and waiting for start game signal\r\n");
    while (1)
    {
        if (can_rx(&message))
        {
            if (message.id == START_GAME_CAN_ID)
            {
				printf("Received start game signal\r\n");
                last_score = run_game();
            }
            if (message.id == JOYSTICK_CAN_ID)
            {
                send_game_state_over_can(0, last_score); // if last game state got lost, send the last score to node 1
            }
        }
    }
}

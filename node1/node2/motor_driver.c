#include "sam.h"
#include "pwm_driver.h"
#include "motor_driver.h"

#define MOTOR_PWM_CHANNEL  0u
#define MOTOR_PWM_PERIOD   50000u
#define MOTOR_DIR_PIN      (1u << 23)

// Start with a maximum of 20% duty cycle
#define MOTOR_MAX_DUTY     15000u

bool motor_init(void)
{
    // Enable PIOC clock
    PMC->PMC_PCER0 = (1u << ID_PIOC);

    // Configure PC23 as GPIO output
    PIOC->PIO_PER = MOTOR_DIR_PIN;
    PIOC->PIO_CODR = MOTOR_DIR_PIN;
    PIOC->PIO_OER = MOTOR_DIR_PIN;

    // Assumes servo_init() already initialized PWM
    // Start PWM channel 0 with zero duty
    return pwm_start(MOTOR_PWM_CHANNEL,
                     MOTOR_PWM_PERIOD,
                     0u);
}

void motor_stop(void)
{
    pwm_write_duty(MOTOR_PWM_CHANNEL, 0u);
}

void motor_set_speed(float speed)
{
    // Clamp command to [-1, 1]
    if (speed > 1.0f)
        speed = 1.0f;

    if (speed < -1.0f)
        speed = -1.0f;

    // Stop before changing direction
    motor_stop();

    if (speed > 0.0f)
    {
        // Direction 1
        PIOC->PIO_SODR = MOTOR_DIR_PIN;
    }
    else if (speed < 0.0f)
    {
        // Direction 2
        PIOC->PIO_CODR = MOTOR_DIR_PIN;
        speed = -speed;
    }
    else
    {
        return;
    }

    // Convert speed magnitude into PWM duty
    uint32_t duty =
        (uint32_t)(speed * MOTOR_MAX_DUTY);

    pwm_write_duty(MOTOR_PWM_CHANNEL, duty);
}
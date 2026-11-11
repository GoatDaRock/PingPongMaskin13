#include "solenoid_driver.h"

static Pio *solenoid_port = 0;
static uint32_t solenoid_pin = 0;

static bool output_active_high = true;
static bool active = false;

static uint32_t start_time = 0;
static uint32_t pulse_duration = 0;

// Set the relay control output
static void set_output(bool enabled)
{
    if (solenoid_port == 0)
        return;

    if (enabled == output_active_high)
    {
        solenoid_port->PIO_SODR = solenoid_pin;
    }
    else
    {
        solenoid_port->PIO_CODR = solenoid_pin;
    }
}

void solenoid_init(Pio *port,
                   uint32_t pin_mask,
                   bool active_high)
{
    solenoid_port = port;
    solenoid_pin = pin_mask;
    output_active_high = active_high;

    active = false;

    // Set inactive output before enabling GPIO
    set_output(false);

    // Configure selected pin as GPIO
    port->PIO_PER = pin_mask;

    // Configure selected pin as output
    port->PIO_OER = pin_mask;
}

void solenoid_trigger(uint32_t now_ms,
                      uint32_t pulse_ms)
{
    // Do not retrigger while active
    if (active || pulse_ms == 0)
        return;

    start_time = now_ms;
    pulse_duration = pulse_ms;

    active = true;
    set_output(true);
}

void solenoid_update(uint32_t now_ms)
{
    if (active &&
        (uint32_t)(now_ms - start_time) >= pulse_duration)
    {
        solenoid_stop();
    }
}

void solenoid_stop(void)
{
    set_output(false);
    active = false;
}

bool solenoid_is_active(void)
{
    return active;
}
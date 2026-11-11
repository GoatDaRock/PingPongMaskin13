#include "ir_driver.h"
#include "adc_driver.h"

#define IR_CONFIRM_SAMPLES 5u

static uint16_t threshold_blocked;
static uint16_t threshold_clear;
static uint16_t last_adc;

static uint8_t consecutive = 0;
static bool beam_blocked = false;

void ir_init(uint16_t blocked_threshold,
             uint16_t clear_threshold)
{
    adc_init();

    threshold_blocked = blocked_threshold;
    threshold_clear = clear_threshold;

    last_adc = 0;
    consecutive = 0;
    beam_blocked = false;
}

bool ir_update(void)
{
    last_adc = adc_read();

    if (!beam_blocked)
    {
        // Confirm that the beam is blocked
        if (last_adc < threshold_blocked)
        {
            if (++consecutive >= IR_CONFIRM_SAMPLES)
            {
                beam_blocked = true;
                consecutive = 0;
            }
        }
        else
        {
            consecutive = 0;
        }
    }
    else
    {
        // Confirm that the beam is restored
        if (last_adc > threshold_clear)
        {
            if (++consecutive >= IR_CONFIRM_SAMPLES)
            {
                beam_blocked = false;
                consecutive = 0;
            }
        }
        else
        {
            consecutive = 0;
        }
    }

    return beam_blocked;
}

uint16_t ir_get_adc_value(void)
{
    return last_adc;
}
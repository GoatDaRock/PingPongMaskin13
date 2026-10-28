#pragma once
#include <stdint.h>
#include <stdbool.h>

// use limits
bool servo_init(uint32_t min_pulse_width, uint32_t max_pulse_width, uint32_t clock_frequency);
bool servo_set_position(float position);
bool servo_set_pulse_width(uint32_t pulse_width);
bool servo_neutral();

#pragma once
#include <stdint.h>
#include <stdbool.h>

bool pwm_init(uint32_t clock_frequency);

bool pwm_start(uint8_t channel, uint32_t period, uint32_t high_time);

bool pwm_write_duty(uint8_t channel, uint32_t high_time);
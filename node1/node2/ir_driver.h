#ifndef IR_DRIVER_H
#define IR_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

void ir_init(uint16_t blocked_threshold,
             uint16_t clear_threshold);

bool ir_update(void);
uint16_t ir_get_adc_value(void);

#endif
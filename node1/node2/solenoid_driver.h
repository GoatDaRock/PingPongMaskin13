#ifndef SOLENOID_DRIVER_H
#define SOLENOID_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "sam.h"

void solenoid_init(Pio *port,
                   uint32_t pin_mask,
                   bool active_high);

void solenoid_trigger(uint32_t now_ms,
                      uint32_t pulse_ms);

void solenoid_update(uint32_t now_ms);

void solenoid_stop(void);

bool solenoid_is_active(void);

#endif
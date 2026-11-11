#ifndef ENCODER_DRIVER_H
#define ENCODER_DRIVER_H

#include <stdint.h>

void encoder_init(void);
uint32_t encoder_read(void);

#endif
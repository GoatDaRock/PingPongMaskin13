#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <stdbool.h>

bool motor_init(void);
void motor_set_speed(float speed);
void motor_stop(void);

#endif
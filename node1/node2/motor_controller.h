#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

typedef struct
{
    float kp;
    float ki;
    float integral;
    float output_limit;
} MotorController;

void motor_controller_init(MotorController *controller,
                           float kp,
                           float ki,
                           float output_limit);

float motor_controller_update(MotorController *controller,
                              float reference,
                              float position,
                              float dt);

void motor_controller_reset(MotorController *controller);

#endif

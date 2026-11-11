#include "motor_controller.h"

static float clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void motor_controller_init(MotorController *controller,
                           float kp,
                           float ki,
                           float output_limit)
{
    controller->kp = kp;
    controller->ki = ki;
    controller->integral = 0.0f;
    controller->output_limit = output_limit;
}

void motor_controller_reset(MotorController *controller)
{
    controller->integral = 0.0f;
}

float motor_controller_update(MotorController *controller,
                              float reference,
                              float position,
                              float dt)
{
    float error = reference - position;

    // Proportional contribution
    float proportional = controller->kp * error;

    // Candidate new integral
    float new_integral =
        controller->integral + error * dt;

    float unsaturated =
        proportional + controller->ki * new_integral;

    float limit = controller->output_limit;

    // Conditional integration anti-windup
    if (!((unsaturated > limit && error > 0.0f) ||
          (unsaturated < -limit && error < 0.0f)))
    {
        controller->integral = new_integral;
    }

    float output =
        proportional + controller->ki * controller->integral;

    return clamp(output, -limit, limit);
}
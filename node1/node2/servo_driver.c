 #include "servo_driver.h"
 #include "pwm_driver.h"
 #include "sam.h"

 static uint32_t min_pulse_width;
 static uint32_t max_pulse_width;

 bool servo_init(uint32_t min_pulse_width_u, uint32_t max_pulse_width_u, uint32_t clock_frequency){
  min_pulse_width = min_pulse_width_u;
  max_pulse_width = max_pulse_width_u;
  if (!pwm_init(clock_frequency)) {
    return false;
  }
  if (!pwm_start(1, 20000, min_pulse_width)) { // Start PWM on channel 1 with a period of 20ms (50Hz) and initial high time of min_pulse_width
	return false;
  }
  return true;
 }

 bool servo_set_position(float position){
  if (position < 0.0f) position = 0.0f;
  if (position > 1.0f) position = 1.0f;
  uint32_t pulse_width = min_pulse_width + (uint32_t)((max_pulse_width - min_pulse_width) * position);
  return servo_set_pulse_width(pulse_width);
 }

 bool servo_set_pulse_width(uint32_t pulse_width){
  return pwm_write_duty(1, pulse_width);
 }

 bool servo_neutral(){
  return servo_set_position(0.5f);
 }
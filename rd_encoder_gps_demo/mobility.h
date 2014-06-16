#ifndef _mobility_h_
#define _mobility_h_

#include <avr/interrupt.h>
#include "RobotDevil.h"
#include "globals.h"

////////////////////////////////////////////////////////////////////////////////
// Mobility Constants
#define STEERING_PORT                PORTD
#define GASBRAKE_PORT                PORTD
#define STEERING_PIN                 (1 << 2)   // PD2 (2)
#define GASBRAKE_PIN                 (1 << 3)   // PD3 (3)

#define STEERING_NEUTRAL             1550
#define GASBRAKE_NEUTRAL             1500
#define STEERING_MIN                 1000       // steer full-right
#define STEERING_MAX                 2000       // steer full-left
#define GASBRAKE_MIN                 1100
#define GASBRAKE_MAX                 1900
#define GASBRAKE_CRUISE              1700

////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routines
// negating the steering and gasbrake pins terminates the PWM pulse
ISR(TIMER1_COMPA_vect) {
  STEERING_PORT &= ~STEERING_PIN;
}

ISR(TIMER1_COMPB_vect) {
  GASBRAKE_PORT &= ~GASBRAKE_PIN;
}


////////////////////////////////////////////////////////////////////////////////
// Mobility Variables


////////////////////////////////////////////////////////////////////////////////
// Mobility Functions
void update_control_values(void) {
  float seconds = globals.loop_counter / LOOP_FREQUENCY_HZ;

  if (seconds < SPEED_CONTROLLER_DELAY) {
    globals.steering_servo_us = STEERING_NEUTRAL;
    globals.gasbrake_servo_us = GASBRAKE_NEUTRAL;
    
    return;
  }
  
  else if (seconds > STOP_VEHICLE_TIME) {
    globals.steering_servo_us = STEERING_NEUTRAL;
    globals.gasbrake_servo_us = GASBRAKE_NEUTRAL;
    
    return;
  }

  // negative heading_error values indicate that we need to steer left (and
  // vice versa). to steer more to the left we must increase the pwm signal
  // value (and vice versa), which is why we subract the correction value from
  // the neutral steering value.
  //uint16_t steering = (uint16_t)(STEERING_NEUTRAL - globals.heading_error_deg * STEERING_GAIN);
  uint16_t steering = (uint16_t)(STEERING_NEUTRAL - globals.target_angle * RAD_TO_DEG * STEERING_GAIN);

  if (steering < STEERING_MIN) {
    steering = STEERING_MIN;
  }
  else if (steering > STEERING_MAX) {
    steering = STEERING_MAX;
  }
  
  if (globals.started) {
    globals.steering_servo_us = (uint16_t) steering;
    //globals.gasbrake_servo_us = GASBRAKE_NEUTRAL;
    globals.gasbrake_servo_us = GASBRAKE_CRUISE;
  }

  return;
}

#endif 

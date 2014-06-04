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

#define STEERING_NEUTRAL             1500
#define GASBRAKE_NEUTRAL             1500



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
  
  if (seconds < SPEED_CONTROLLER_DELAY || globals.mission_started == 0) {
    globals.steering_servo_us = STEERING_NEUTRAL;
    globals.gasbrake_servo_us = GASBRAKE_NEUTRAL;
    
    return;
  }
  
  else if (seconds > STOP_VEHICLE_TIME) {
    globals.steering_servo_us = STEERING_NEUTRAL;
    globals.gasbrake_servo_us = GASBRAKE_NEUTRAL;
    
    return;
  }
  
  /*
  
  INSERT CONTROL CODE HERE
  */  

  return;
}

#endif 

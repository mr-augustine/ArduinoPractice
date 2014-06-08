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
#define STEERING_MIN                 1100       // steer full-right
#define STEERING_MAX                 1900       // steer full-left
#define GASBRAKE_MIN                 1100
#define GASBRAKE_MAX                 1900
#define GASBRAKE_CRUISE              1600

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
float previous_hdgerror_deg = 0.0;
float timeChange = 0.0;    // time between two successive compass readings
////////////////////////////////////////////////////////////////////////////////
// Mobility Functions
void update_heading_error(void) {
  //globals.heading_error_deg = 0.0;
  float heading_delta = 0.0;
  
  if ((globals.status_bits & STATUS_COMPASS_VALID) == STATUS_COMPASS_VALID) {
    heading_delta = globals.target_heading_deg - globals.compass_deg;
   
    if (heading_delta > 180.0) {
      heading_delta -= 360.0;
    }
    else if (heading_delta < -180.0) {
      heading_delta += 360.0;
    }
    
    previous_hdgerror_deg = globals.heading_error_deg;
    globals.heading_error_deg = heading_delta;
    // negative heading_error values indicate that you need to steer left to
    // move more directly towards the target, and vice versa
  }
}

void update_control_values(void) {
  float seconds = globals.loop_counter / LOOP_FREQUENCY_HZ;
  
  if (seconds < SPEED_CONTROLLER_DELAY || globals.mission_started == 0) {
    globals.steering_servo_us = STEERING_NEUTRAL;
    globals.gasbrake_servo_us = GASBRAKE_NEUTRAL;
    
    return;
  }
  
  else if (globals.button_press_count >= 3) {//(seconds > STOP_VEHICLE_TIME) {
    globals.steering_servo_us = STEERING_NEUTRAL;
    globals.gasbrake_servo_us = GASBRAKE_NEUTRAL;
    
    return;
  }

  // if there was no overflow during the last loop period, then we can
  // assume that the two compass readings occured one loop period apart.
  // if there was a timer overflow, then we'll have to calculate a more accurate
  // time measurement (i.e., one loop period + overflow duration)
  timeChange = 1 / LOOP_FREQUENCY_HZ;
  if ((globals.status_bits & STATUS_TIMER1_OVERFLOW) == STATUS_TIMER1_OVERFLOW) {
    timeChange += ((overflowedAt_ticks - LOOP_PERIOD_TICKS) /
      (LOOP_FREQUENCY_HZ * LOOP_PERIOD_TICKS));
  }
  
  // hdgerror_rate_dps is the heading error rate in degrees per second;
  // i.e., the rate you are closing in or drifting away from the target heading.
  // if the vehicle is closing in towards the target heading, this value will
  // be positive.
  float hdgerror_rate_dps = 
      (previous_hdgerror_deg - globals.heading_error_deg) / timeChange;
    
  float steering_k_diff = PID_K_DIFF * hdgerror_rate_dps;  
  float steering_k_prop = PID_K_PROP * globals.heading_error_deg;
  
  // the corrected steering control includes the proportional and differential
  // components. the proportional component should always direct the vehicle
  // toward the target heading, and the differential component should
  // simultaneously direct the vehicle away from the target heading.
  // here we subtract steering_k_prop because (1) negative heading_error values
  // indicate we need to steer left (and vice versa); and (2) if we need to 
  // steer more to the left we must increase the pwm signal value (and vice
  // versa)
  float steering = globals.steering_servo_us - steering_k_prop;
  if (globals.heading_error_deg <= 0 ) {
    // if the vehicle needs to turn left towards the target, add right-turn
    // differential correction. remember: smaller PWM values steer right, and
    // larger PWM values steer left.
    steering -= steering_k_diff;
  }
  // otherwise, if the vehicle needs to turn right towards the target, add
  // left-turn differential correction
  else {
    steering += steering_k_diff;
  }
  
  if (steering < STEERING_MIN) {
    //PORTB |= (1 << 5);
    steering = STEERING_MIN;
  }
  else if (steering > STEERING_MAX) {
    steering = STEERING_MAX;
    //PORTB |= (1 << 5);
  }
  //else { PORTB &= ~(1 << 5); }
  
  
  globals.steering_servo_us = (uint16_t) steering;
  globals.gasbrake_servo_us = GASBRAKE_NEUTRAL;
  //globals.gasbrake_servo_us = GASBRAKE_CRUISE;

  return;
}

#endif 

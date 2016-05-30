#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "mobility.h"
#include "pins.h"

typedef enum {
  Driving_Forward,
  Driving_Reverse,
  Driving_Neutral
} Drive_State;

static uint16_t mobility_throttle_us;
static uint16_t mobility_steer_us;

static uint8_t current_hold_iterations;

Drive_State current_state;

// These Interrupt Service Routines are used to terminate the PWM pulses 
// for the steering servo and drive motor. The PWM pulses are terminated 
// when the timer reaches the values associated with the desired pulse 
// durations. Those values are set in the output compare registers.
ISR(STEERING_ISR_VECT) {
  STEERING_PORT &= ~(1 << STEERING_PIN);
}

ISR(THROTTLE_ISR_VECT) {
  THROTTLE_PORT &= ~(1 << THROTTLE_PIN);
}

static void tnp_bypass(void) {
  uint16_t pulse_on_duration_us = 1500;
  uint16_t pulse_off_duration_us = 23500;
  uint8_t pulse_iterations = 250;
  uint8_t i;

  // Ensure the drive pin is given a low signal
  THROTTLE_PORT &= ~(1 << THROTTLE_PIN);

  for (i = 0; i < pulse_iterations; i++) {
    // Start the neutral pwm pulse
    THROTTLE_PORT |= (1 << THROTTLE_PIN);

    // Hold the pulse
    _delay_us(pulse_on_duration_us);

    // end the pulse
    THROTTLE_PORT &= ~(1 << THROTTLE_PIN);
    _delay_us(pulse_off_duration_us);
  }

  return;
}

uint8_t mobility_init(void) {
  current_state = Driving_Neutral;

  THROTTLE_PORT = 0;
  STEERING_PORT = 0;

  THROTTLE_DDR = (1 << THROTTLE_PIN);
  STEERING_DDR = (1 << STEERING_PIN); 

  // Begin Throttle Neutral Protection bypass
  tnp_bypass();

  mobility_stop();
  steer_to_direction(1500);
  
  return 1;
}

// TODO update this function to have an architecture similar to the
// stop and reverse functions
void mobility_drive_fwd(Drive_Speed speed) {
  switch (speed) {
    case Drive_Creep:
      mobility_throttle_us = 1600;
      break;
    case Drive_Cruise:
      mobility_throttle_us = 1800;
      break;
    case Drive_Ludicrous:
      mobility_throttle_us = 2000;
      break;
    default:
      mobility_stop();
      return;
  }

  THROTTLE_COMPARE_REG = mobility_throttle_us >> 2;

  // If you're currently driving forward, then set the target speed
  // No need to update current_state

  // Else if you're in neutral, start to ramp up to speed
  // Update current_state = Driving_Forward

  // Else if you're in reverse begin slow to stop procedure
  // Update current_state = Driving_Neutral only when you're in Neutral long enough
}

void mobility_drive_rev(Drive_Speed speed) {
  // If you've already completed the reverse init
  // Identify the PWM that corresponds with the specified speed
  if (current_state == Driving_Reverse) {
    uint16_t target_speed_us = SPEED_NEUTRAL;

    switch(speed) {
      case Drive_Creep:
        target_speed_us = SPEED_REV_CREEP;
        break;
      case Drive_Cruise:
        target_speed_us = SPEED_REV_CRUISE;
        break;
      case Drive_Ludicrous:
        target_speed_us = SPEED_REV_LUDICROUS;
        break;
      default:
        // target_speed_us will be neutral
        // TODO consider reporting an error in this case
    }

    //
    if (mobility_throttle_us > target_speed_us) {
      mobility_throttle_us -= REV_RATE_US;
    }
    // Note: if you're trying to reduce your reverse speed, you will immediately
    // adjust the PWM signal to the target speed. We're more concerned about
    // ramping up the reverse speed gradually.
    else {
      mobility_throttle_us = target_speed_us;
    }
    // continue decreasing PWM until you hit Drive_Speed
  }

  // If you haven't completed the reverse init
  else {
    switch (current_state) {
      case Driving_Forward:
        mobility_stop();
        return;
        break;
      case Driving_Neutral:
        current_state = Driving_Pre_Reverse;
        mobility_throttle_us -= REV_RATE_US;
        break;
      case Driving_Pre_Reverse:
        if (mobility_throttle_us > PRE_REV_STOP_US) {
          mobility_throttle_us -= REV_RATE_US;
          current_hold_iterations = 0;
        } else {
          // Setting the throtte in case the REV_RATE_US and PWM values don't
          // create a value expressed in hundreds of microseconds
          mobility_throttle_us = PRE_REV_STOP_US;

          // continue to hold the pre_reverse_stop signal until counter expires
          if (current_hold_iterations < PRE_REV_HOLD_ITERS) {
            current_hold_iterations++
          } else {
            current_state = Driving_Reverse;
            mobility_throttle_us = SPEED_NEUTRAL;
            current_hold_iterations = 0;
          }
        }
        break;
      default:
        // TODO consider reporting an error in this case
    }    
    // If you're Driving_Forward, call mobility_stop()
    // Else If you're Driving_Neutral, start Driving_Pre_Reverse
    // Else if you're Driving_Pre_Reverse, continue decreasing PWM until you hit stop, then wait
  }

  THROTTLE_COMPARE_REG = mobility_throttle_us >> 2;

  return;
}

void mobility_hardstop(void) {
  mobility_throttle_us = SPEED_NEUTRAL;
  
  THROTTLE_COMPARE_REG = SPEED_NEUTRAL >> 2;

  return;
}

void mobility_stop(void) {
  switch (current_state) {

    // If you're already in neutral
    case Driving_Neutral:
      mobility_throttle_us = SPEED_NEUTRAL;
      break;

    // If you're currently driving forward and want to stop, decrease the PWM signal
    // by a certain amount for this iteration. Continue doing so for subsequent
    // iterations until the PWM width equals 1500. Then change the current_state
    // to Driving_Neutral.
    case Driving_Forward:
      if (mobility_throttle_us > SPEED_NEUTRAL) {
        mobility_throttle_us -= FWD_TO_STOP_RATE_US;
      } else {
        mobility_throttle_us = SPEED_NEUTRAL;
        current_state = Driving_Neutral;
      }
      break;

    // Likewise, if you're currently driving in reverse and want to stop, increase
    // the PWM signal by a certain amount for this iteration. Continue doing so 
    // until the PWM width equals 1500. Then change the current_state to
    // Driving_Neutral.
    case Driving_Reverse:
      if (mobility_throttle_us < SPEED_NEUTRAL) {
        mobility_throttle_us += REV_TO_STOP_RATE_US;
      } else {
        mobility_throttle_us = SPEED_NEUTRAL;
        current_state = Driving_Neutral;
      }
      break;

    default:
      // TODO consider reporting an error in this case
  }

  THROTTLE_COMPARE_REG = mobility_throttle_us >> 2;

  return;
}

void steer_to_direction(uint16_t turn_degree) {
  // Steering values greater than 1500 will steer Left. And steering values
  // less than 1500 will steer Right. This code snippet prevents the
  // steering servo from getting a signal that would command it beyond its
  // turn limits.
  if (turn_degree < TURN_FULL_RIGHT) {
    turn_degree = TURN_FULL_RIGHT;
  } else if (turn_degree > TURN_FULL_LEFT) {
    turn_degree = TURN_FULL_LEFT;
  } 

  mobility_steer_us = turn_degree;

  STEERING_COMPARE_REG = mobility_steer_us >> 2;

  return;
}


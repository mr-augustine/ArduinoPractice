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

static uint16_t mobility_drive_us;
static uint16_t mobility_steer_us;

Drive_State current_state;
Drive_State target_state;

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
  target_state = Driving_Neutral;

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

void mobility_drive_fwd(Drive_Speed speed) {
  switch (speed) {
    case Drive_Creep:
      mobility_drive_us = 1600;
      break;
    case Drive_Cruise:
      mobility_drive_us = 1800;
      break;
    case Drive_Ludicrous:
      mobility_drive_us = 2000;
      break;
    default:
      mobility_stop();
      return;
  }

  THROTTLE_COMPARE_REG = mobility_drive_us >> 2;

  // If you're currently driving forward, then set the target speed
  // No need to update current_state or target_state

  // Else if you're in neutral, start to ramp up to speed
  // Update current_state = Driving_Forward

  // Else if you're in reverse begin slow to stop procedure
  // Update current_state = Driving_Neutral only when you're in Neutral long enough
}

void mobility_drive_rev(Drive_Speed speed) {

}

void mobility_stop(void) {
  mobility_drive_us = SPEED_NEUTRAL;
  
  THROTTLE_COMPARE_REG = SPEED_NEUTRAL >> 2;

  return;

  target_state = Driving_Neutral;

  // If you're already in neutral
  if (current_state == target_state) {
    mobility_drive_us = SPEED_NEUTRAL;
  }

  // If you're currently drving foward and want to stop, decrease the PWM signal
  // by a certain amount for this iteration. Continue doing so for subsequent
  // iterations until the PWM width equals 1500. Then change the current_state
  // Driving_Neutral.
  else if (current_state == Driving_Forward) {
    mobility_drive_us -= FWD_TO_STOP_RATE_US;

    if (mobility_drive_us == SPEED_NEUTRAL) {
      current_state = Driving_Neutral;
    }
  }

  else if (current_state == Driving_Reverse) {
    mobility_drive_us += REV_TO_STOP_RATE_US;

    if (mobility_drive_us == SPEED_NEUTRAL) {
      current_state = Driving_Neutral;
    }
  }

  THROTTLE_COMPARE_REG = mobility_drive_us >> 2;

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


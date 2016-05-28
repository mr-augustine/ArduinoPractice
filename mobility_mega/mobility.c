#include <avr/interrupt.h>
#include <avr/io.h>
#include "mobility.h"
#include "pins.h"

typedef enum {
  Driving_Forward,
  Driving_Reverse,
  Driving_Neutral
} Drive_State;

typedef enum {
  Drive_Creep,
  Drive_Cruise,
  Drive_Ludicrous
} Drive_Speed;

static uint16_t mobility_drive_us;
static uint16_t mobility_steer_us;

Drive_State current_state;
Drive_State target_state;

// These Interrupt Service Routines are used to terminate the PWM pulses 
// for the steering servo and drive motor. The PWM pulses are terminated 
// when the timer reaches the values associated with the desired pulse 
// durations. Those values are set in the output compare registers.
ISR(STEERING_ISR_VECT) {
  STEERING_PORT &= ~STEERING_PIN;
}

ISR(DRIVE_ISR_VECT) {
  DRIVE_PORT &= ~DRIVE_PIN;
}

uint8_t mobility_init(void) {

  current_state = Driving_Neutral;
  target_state = Driving_Neutral;

  mobility_stop();

  return 1;
}

void mobility_drive_fwd(Drive_Speed speed) {
  switch (speed) {
    case Drive_Creep:
      mobility_drive_us = 1600 >> 2;
      break;
    case Drive_Cruise:
      mobility_drive_us = 1800 >> 2;
      break;
    case Drive_Ludicrous:
      mobility_drive_us = 2000 >> 2;
      break;
    default:
      mobility_stop();
      return;
  }

  DRIVE_COMPARE_REG = mobility_drive_us;

  // If you're currently driving forward, then set the target speed
  // No need to update current_state or target_state

  // Else if you're in neutral, start to ramp up to speed
  // Update current_state = Driving_Forward

  // Else if you're in reverse begin slow to stop procedure
  // Update current_state = Driving_Neutral only when you're in Neutral long enough
}

void mobility_drive_rev(uint16_t speed) {

}

void mobility_stop(void) {
  mobility_drive_us = 1500 >> 2;
  DRIVE_COMPARE_REG = mobility_drive_us;

  return;
}

void steer_to_direction(uint16_t turn_degree) {
  mobility_steer_us = turn_degree >> 2;
  STEERING_COMPARE_REG = mobility_steer_us;

  return;
}


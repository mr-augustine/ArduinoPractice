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

ISR(DRIVE_ISR_VECT) {
  DRIVE_PORT &= ~(1 << DRIVE_PIN);
}

static void tnp_bypass(void) {
  uint16_t pulse_on_duration_us = 1500;
  uint16_t pulse_off_duration_us = 23500;
  uint8_t pulse_iterations = 250;
  uint8_t i;
  // Ensure the drive pin is given a low signal
  DRIVE_PORT &= ~(1 << DRIVE_PIN);

  for (i = 0; i < pulse_iterations; i++) {
    // Start the neutral pwm pulse
    DRIVE_PORT |= (1 << DRIVE_PIN);

    // Hold the pulse
    _delay_us(pulse_on_duration_us);

    // end the pulse
    DRIVE_PORT &= ~(1 << DRIVE_PIN);
    _delay_us(pulse_off_duration_us);
  }

  return;
}

uint8_t mobility_init(void) {
  current_state = Driving_Neutral;
  target_state = Driving_Neutral;

  DRIVE_PORT = 0;
  STEERING_PORT = 0;

  DRIVE_DDR = (1 << DRIVE_PIN);
  STEERING_DDR = (1 << STEERING_PIN); 

  mobility_stop();
  steer_to_direction(1500);
  
  // Begin Throttle Neutral Protection bypass
  tnp_bypass();

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
  
  //OCR1B = mobility_drive_us;
  DRIVE_COMPARE_REG = mobility_drive_us;

  return;
}

void steer_to_direction(uint16_t turn_degree) {
  mobility_steer_us = turn_degree >> 2;

  //OCR1A = mobility_steer_us;
  STEERING_COMPARE_REG = mobility_steer_us;

  return;
}


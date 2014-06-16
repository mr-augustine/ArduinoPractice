/*******************************************************************************
filename: rd_encoder_gps_demo.ino
author:   Augustine (github: mr-augustine)
          http://pushbuttonrobotics.weebly.com/
date:     14 June 2014

The rd_encoder_gps_demo sketch builds on top of the rd_headingsteer_demo by
adding GPS and wheel encoder measurements.

This code is based on the Rocksteady framework.
*******************************************************************************/
#include <avr/pgmspace.h>
#include "RobotDevil.h"
#include "globals.h"
#include "pushButton.h"
#include "compass.h"
#include "mobility.h"
#include "encoder.h"
#include "gps.h"
#include "state_estimator.h"
#include "waypoints.h"

volatile uint8_t timer1_overflow = 0;
globals_t globals;

////////////////////////////////////////////////////////////////////////////////
// Constants
#define LOOP_ACTIVE_PORT        PORTB
#define LOOP_ACTIVE_PIN         (1 << 1)


////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routines
ISR(TIMER1_OVF_vect) {                                       // TIMER1 OVERFLOW
  timer1_overflow = 1;
}

////////////////////////////////////////////////////////////////////////////////
// Arduino Functions
void setup() {
  noInterrupts();

  ////////////////////////////////////////////////////////////////////////////
  // Configure PORTB: [SD Card]
  // PB0 ( 8) -> output, GPS TX, wired to PD0 (USART RX)
  // PB1 ( 9) -> output, loop-active indicator
  // PB2 (10) -> output, SPI CS, SD Card
  // PB3 (11) -> output, SPI MOSI, SD Card
  // PB4 (12) -> input, SPI MISO, SD Card
  // PB5 (13) -> output, SPI CLK, SD Card
  DDRB = 0b00101110;    // set LED pins as output pins (Digital Pins 10-12)
  PORTB = 0b00010000;   // enable pull-up resistor for MISO
  
  ////////////////////////////////////////////////////////////////////////////
  // Configure PORTC: [Encoder][PushButton][Compass]
  // PC0 (A0) -> input, Encoder Ch A
  // PC1 (A1) -> input, Encoder Ch B
  // PC2 (A2) -> input, pushbutton
  // PC3 (A3) -> not used
  // PC4 (A4) -> output/input, SDA, compass
  // PC5 (A5) -> output, SCL, compass
  DDRC = 0b00110000;    // configure the compass pins as output pins
  PORTC = 0b00000100;   // enable pull-up resistor for pushbutton

  ////////////////////////////////////////////////////////////////////////////
  // Configure PORTD: [Steering][GasBrake][GPS]
  // PD0 (0) -> input, USART RX connected from GPS TX (Pin 8)
  // PD1 (1) -> not used
  // PD2 (2) -> output, steering
  // PD3 (3) -> output, gasbrake
  // PD4 (4) -> not used
  // PD5 (5) -> not used
  // PD6 (6) -> not used
  // PD7 (7) -> input, GPS RX (GPS input), ignored; wired to PD0
  PORTD = 0b00000000;   // disable all pull-up resistors;
  DDRD = 0b11111100;    // set gasbrake and steering as output pins

  ////////////////////////////////////////////////////////////////////////////
  // Initialize Variables
  memset(&globals, 0, sizeof(globals));  // set globals memory block to zero
  globals.start_bytes = GLOBAL_START;
  globals.stop_bytes = GLOBAL_STOP;
  set_padding(&globals);
  
  globals.steering_servo_us = STEERING_NEUTRAL;
  globals.gasbrake_servo_us = GASBRAKE_NEUTRAL;

  ////////////////////////////////////////////////////////////////////////////
  // Initialize Devices
  init_gps();
  init_compass();
  init_encoder();
  init_state_estimator();
  //init_logger();
  init_waypoints();
  init_pushbutton();

  ////////////////////////////////////////////////////////////////////////////
  // Configure TIMER1
  TCCR1A = 0b00000000;  // normal compare output mode & normal WGM
  TCCR1B = 0b00000011;  // set prescaler value to 64
  TCCR1C = 0b00000000;  // disable force output compare
  TIMSK1 = 0b00000000;  // disable output compare & timer overflow interrups

  
  interrupts();
  
  // enable TIMER1 overflow interrupts
  TIMSK1 |= (1 << TOIE1);
  
  globals.loop_counter = 0;
}

void loop() {
  LOOP_ACTIVE_PORT |= LOOP_ACTIVE_PIN;
  TCNT1 = 0; // reset timer/counter 1 for each loop iteration
  
  // start the PWM pulses for steering and gasbrake
  PORTD |= (GASBRAKE_PIN | STEERING_PIN);

  // set the output compare registers with the timer values that correspond
  // with the end of the PWM pulses
  OCR1A = globals.steering_servo_us >> 2;
  OCR1B = globals.gasbrake_servo_us >> 2;

  // also enable output compare timers to trigger when there's a match
  // so the program can end the PWM pulses
  TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B);

  // finalize values in globals
  if (timer1_overflow) {
    globals.status_bits |= STATUS_TIMER1_OVERFLOW;
  }
  
  //start_log();
  
  globals.loop_counter += 1;
  globals.status_bits = 0;
  timer1_overflow = 0;
  

  update_compass();
  update_pushbutton();
  update_gps();
  update_encoder();
  
  update_state_estimator();
  
  update_waypoints();

  update_control_values();
  
  
  //finish_log();
  
  // take note if loops runs longer than desired loop period
  if (TCNT1 > LOOP_PERIOD_TICKS) {
    globals.status_bits |= STATUS_MAIN_LOOP_SLOW;
    return;
  }
  
  // otherwise go idle until the loop period is complete
  // but first turn off the loop_active indicator
  LOOP_ACTIVE_PORT &= ~LOOP_ACTIVE_PIN;
  
  while (1) {
    if (timer1_overflow) {
      return;
    }
    
    if (TCNT1 > LOOP_PERIOD_TICKS) {
      return;
    }
  }
}

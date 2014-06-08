/*******************************************************************************
filename: rd_headingsteer_demo.ino
author:   Augustine (github: mr-augustine)
          http://pushbuttonrobotics.weebly.com/
date:     3 June 2014

The rd_headingsteer_demo sketch demonstrates steering control towards a chosen
heading. The target heading is set by first pointing the compass in the desired
direction and then pressing a pushbutton. As the vehicle heading is changed,
it's front wheels will steer towards the target heading.

The CMPS10 compass is used in this demo. It is a tilt-compensated compass.
The specs can be found here:
http://www.robot-electronics.co.uk/htm/cmps10doc.htm
http://www.robot-electronics.co.uk/htm/cmps10i2c.htm

This code is based on the Rocksteady framework.
*******************************************************************************/
#include <avr/pgmspace.h>
#include "RobotDevil.h"
#include "globals.h"
#include "pushButton.h"
#include "compass.h"
#include "mobility.h"

volatile uint8_t timer1_overflow = 0;
globals_t globals;
uint16_t overflowedAt_ticks = 0;
////////////////////////////////////////////////////////////////////////////////
// Constants
#define LOOP_ACTIVE_PORT        PORTB
#define LOOP_ACTIVE_PIN         (1 << 1)


////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routines
ISR(TIMER1_OVF_vect) {                                       // TIMER1 OVERFLOW
  timer1_overflow = 1;
}

// Interrupt Service Routine (ISR) to handle the button press.
// remember: an interrupt is made when the pin toggles.
// see section 13 in the Atmel datasheet (External Interrupts).
// since the pull-up resistor is activated on our button pin and we're using a
// momentary pushbutton, the pin toggles from high to low and back to high
// each time the button is pressed.
ISR(PCINT1_vect) {                                          // PUSHBUTTON PRESS
  if (PINC & BUTTON_PIN) {
    button_pressed += 1;
    PINB |= BUTTON_LED;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Arduino Functions
void setup() {
  noInterrupts();
  
  // enable pin change interrupts for the group of pins that includes A2
  PCICR = 0b00000010; 
  // enable interrupts on pin A2 (PCINT10)
  PCMSK1 |= (1 << 2);
  
  ////////////////////////////////////////////////////////////////////////////
  // Configure PORTB:
  // PB0 ( 8) -> output, pushbutton toggle LED
  // PB1 ( 9) -> output, loop-active indicator
  // PB2 (10) -> output, target heading selected LED
  // PB3 (11) -> output, mission started LED
  // PB4 (12) -> output, on-target LED
  // PB5 (13) -> output, troubleshooting LED
  DDRB = 0b00111111;    // set LED pins as output pins (Digital Pins 10-12)
  PORTB = 0;            // disable pull-up resistors; using external resistors
  
  ////////////////////////////////////////////////////////////////////////////
  // Configure PORTC: [PushButton][Compass]
  // PC2 (A2) -> input, pushbutton
  // PC4 (A4) -> SDA, compass
  // PC5 (A5) -> SCL, compass
  DDRC = 0b00110000;    // configure the compass pins as output pins
  PORTC = 0b00000100;    // enable pull-up resistor for pushbutton

  ////////////////////////////////////////////////////////////////////////////
  // Configure PORTD: [Steering][GasBrake]
  // PD2 (2) -> output, steering
  // PD3 (3) -> output, gasbrake
  PORTD = 0;            // disable pull-up resistors;
  DDRD = 0b00001100;    // set gasbrake and steering as output pins
  
  ////////////////////////////////////////////////////////////////////////////
  // Configure TIMER1
  TCCR1A = 0b00000000;  // normal compare output mode & normal WGM
  TCCR1B = 0b00000011;  // set prescaler value to 64
  TCCR1C = 0b00000000;  // disable force output compare
  TIMSK1 = 0b00000000;  // disable output compare & timer overflow interrups
  
  memset(&globals, 0, sizeof(globals));  // set globals memory block to zero
  
  init_compass();
  init_pushbutton();
  
  interrupts();
  
  TIMSK1 = 0b00000001;
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
  TIMSK1 = 0b00000111;
  
  globals.loop_counter += 1;
  globals.status_bits = 0;
  timer1_overflow = 0;
  

  update_compass();
  update_pushbutton();
  update_heading_error();
  update_control_values();


  
  // take note if loops runs longer than desired loop period
  if (TCNT1 > LOOP_PERIOD_TICKS) {
    overflowedAt_ticks = TCNT1;
    globals.status_bits |= STATUS_MAIN_LOOP_SLOW;
    return;
  }
  
  LOOP_ACTIVE_PORT &= ~LOOP_ACTIVE_PIN;
  
  // otherwise go idle until the loop period is complete
  while (1) {
    if (timer1_overflow) {
      return;
    }
    
    if (TCNT1 > LOOP_PERIOD_TICKS) {
      return;
    }
  }
}

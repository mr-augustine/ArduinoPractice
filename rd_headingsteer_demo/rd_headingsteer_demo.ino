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

////////////////////////////////////////////////////////////////////////////////
// Constants




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
  }
}

////////////////////////////////////////////////////////////////////////////////
// Arduino Functions
void setup() {
  noInterrupts();
  
  ////////////////////////////////////////////////////////////////////////////
  // Configure PORTB:
  
  ////////////////////////////////////////////////////////////////////////////
  // Configure PORTC: [PushButton][Compass]
  // PC2 (A2) -> input, pushbutton
  // PC4 (A4) -> SDA, compass
  // PC5 (A5) -> SCL, compass
  DDRC = 0b00110000;    // configure the compass pins as output pins
  PORTC = 0b0000100;    // enable pull-up resistor for pushbutton

  ////////////////////////////////////////////////////////////////////////////
  // Configure PORTD: [Steering][GasBrake]
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
  timer1_overflow = 0;
  

  update_compass();
  update_pushbutton();
  update_control_values();
  
  while (1) {
    if (timer1_overflow) {
      return;
    }
    
    if (TCNT1 > LOOP_PERIOD) {
      return;
    }
  }
}

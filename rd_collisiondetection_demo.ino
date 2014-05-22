/*******************************************************************************
filename: rd_collisiondetection_demo.ino
author:   Augustine (github: mr-augustine)
          http://pushbuttonrobotics.weebly.com/
date:     20 May 2014

The rd_collisiondetection_demo sketch simulates the process of starting the
vehicle, letting it drive until a collision occurs, and then responding to the
collision. A collision is simulated by pressing and releasing a bump switch.
*******************************************************************************/
#include "rd_collisiondetection_globals.h"

volatile uint8_t timer1_overflow = 0;

////////////////////////////////////////////////////////////////////////////////
// Defines
#define LOOP_PERIOD 6250
#define START_BUTTON_PIN (1 << 2)      // PortC register, bit 2 => Analog Pin 2
#define STEERING_PORT PORTD
#define GASBRAKE_PORT PORTD
#define STEERING_PIN (1 << 2)          // PortD register, bit 2 => Digital Pin 2
#define GASBRAKE_PIN (1 << 3)          // PortD register, bit 3 => Digital Pin 3

////////////////////////////////////////////////////////////////////////////////
// Global Variables
uint32_t loop_counter;
uint16_t steering_servo_us = STEERING_NEUTRAL;
uint16_t gasbrake_servo_us = GASBRAKE_NEUTRAL;

uint8_t steer_mode = 0;
volatile uint8_t gasbrake_mode = 0;
uint8_t resweep = 0;  // indicates the start of a new steering servo sweep
uint8_t neutral_hold = 0;  // counter used to maintain a pwm value for several
                           // loop iterations


////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routines
// negating the steering and gasbrake pins terminates the PWM pulse
                                                                    // STEERING
ISR(TIMER1_COMPA_vect) {
  STEERING_PORT &= ~STEERING_PIN;
}
                                                                         // ESC
ISR(TIMER1_COMPB_vect) {
  GASBRAKE_PORT &= ~GASBRAKE_PIN;
}

ISR(PCINT1_vect) {                                              // START BUTTON
  if (PINC & START_BUTTON_PIN) {
    // writing a 1 to this bit will toggle the output for the pin.
    // see section 14.2.2 in the Atmel datasheet (Toggling the Pin)
    PINB |= (1 << 1);
    gasbrake_mode = 9;  // put the vehicle in neutral
  }
}

ISR(TIMER1_OVF_vect) {                                       // TIMER1 OVERFLOW
  timer1_overflow = 1;
}

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
void update_control_values(void);

void setup() {
  noInterrupts();

  ////////////////////////////////////////////////////////////////////////////
  // Configure Controls  
  PORTD = 0;            // disable pull-up resistors;
  DDRD = 0b00001100;    // set gasbrake and steering as output pins
  
  ////////////////////////////////////////////////////////////////////////////
  // Configure TIMER1
  TCCR1A = 0b00000000;  // normal compare output mode & normal WGM
  TCCR1B = 0b00000011;  // set prescaler value to 64
  TCCR1C = 0b00000000;  // disable force output compare
  TIMSK1 = 0b00000000;  // output compare & timer overflow interrups
                        // temporarily disabled (OCIE1B, OCIE1A, TOIE1)
                        
  ////////////////////////////////////////////////////////////////////////////
  // Configure Start Button 
  // configure the pushbutton pin as an input pin (Analog Pin 2, PCINT10).
  DDRC = 0;
  // turn on the pull-up resistor for the pushbutton pin
  // PC0,PC1,PC3..PC5 -> input, not used
  // PC2 (A2) -> input, pushbutton
  PORTC = 0b0000100;
  // enable pin change interrupts for the group of pins that includes A2
  PCICR = 0b00000010; 
  // enable interrupts on pin A2 (PCINT10)
  PCMSK1 |= (1 << 2);

  ////////////////////////////////////////////////////////////////////////////
  // Configure LED
  // configure the LED pin as an output pin (Digital Pin 9)
  DDRB = 0b00000010;
  // turn off the pull-up resistor for the LED pin
  // PB0,PB10..PB13 -> input, not used
  // PB1 ( 9) -> output, LED
  PORTB = 0;
  
  interrupts();
  
  TIMSK1 = 0b00000001;   // only enable TIMER1 overflow interrupts
  loop_counter = 0;
}

void loop() {
  TCNT1 = 0;            // reset timer/counter 1 for each loop iteration
  
  // start the PWM pulse for steering and gasbrake
  PORTD |= (GASBRAKE_PIN | STEERING_PIN);
  
  // set the output compare registers with the timer values that correspond
  // with the end of the PWM pulses
  OCR1A = steering_servo_us >> 2;
  OCR1B = gasbrake_servo_us >> 2;
  TIMSK1 = 0b00000111;  // enable output compare triggering to end PWM pulses
  
  loop_counter += 1;
  timer1_overflow = 0;


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

/*
  The update_control_values() function adjusts the pwm values that correspond
  with the steering and ESC signals. The steering and driving maneuvers are
  broken up into distinct phases/modes.
*/
void update_control_values() {
  float seconds = loop_counter / LOOP_FREQUENCY_HZ;
  
  //////////////////////////////////////////////////////////////////////////////
  // Steering Control
  // steer_mode 0 - apply neutral
  //            1 - begin sweep from right to left
  //            2 - continue sweep from left to right

  if (steer_mode == 0) {
    steering_servo_us = STEERING_NEUTRAL;
    resweep = 1;
    steer_mode = 1;
  }

  else if (steer_mode == 1) {
    if (steering_servo_us == 1500 && resweep == 1) {
      steering_servo_us = 1100;
      resweep = 0;
    }
    
    else {
      steering_servo_us += 10;
    }
    
    if (steering_servo_us == 1900) {
      steer_mode = 2;
    }
  }
  
  else if (steer_mode == 2) {
    steering_servo_us -= 10;
    
    if (steering_servo_us == 1100) {
      steer_mode = 0;
    }
  }

  
  
  /*-----------  
    ESC Control
    -----------*/  
  
  /*
    wait until 5 seconds elapsed before activating the drive motor
    until then, keep sending a neutral signal. this bypasses the 
    throttle neutral protection feature on the esc.
    given the timer settings, 200 loop iterations occur after 5 seconds
    i.e., loop period = (1/40) sec/loop = 25 ms/loop = 0.025 sec/loop
        5 sec / 0.025 sec/loop = 200 loops
  */
  if (seconds < SPEED_CONTROLLER_DELAY_SEC) {
  //if (loop_counter < 200) {
    gasbrake_servo_us = GASBRAKE_NEUTRAL;
    neutral_hold = 0;
    return;
  }

  // send one last neutral signal
  if (gasbrake_mode == 0) {
    gasbrake_servo_us = GASBRAKE_NEUTRAL;
    gasbrake_mode = 1;
  }
  
  // move the wheels forward
  else if (gasbrake_mode == 1) {
    gasbrake_servo_us += 1;
    
    if (gasbrake_servo_us == 1700) {
      gasbrake_mode = 2;
    }
  }
  
  // decelerate back to neutral
  else if (gasbrake_mode == 2) {
    gasbrake_servo_us -= 1;
    
    if (gasbrake_servo_us == GASBRAKE_NEUTRAL) {
      gasbrake_mode = 3;
      neutral_hold = 0;
    }
  }
  
  // after moving forward, stay in neutral for 1 second
  else if (gasbrake_mode == 3) {
    gasbrake_servo_us = GASBRAKE_NEUTRAL;
    
    neutral_hold += 1;
    
    if (neutral_hold > 40) {
      gasbrake_mode = 4;
      neutral_hold = 0;
    }
  }
  
  
  /*
    start the double-tap procedure to initiate reverse
    the double-tap mimics the input you send the receiver when using the stock
    transmitter (i.e., double-tapping the trigger forward send a reverse command)
    note: here we are decrementing by 10 instead of by 1
  */
  // todo: nail down exactly how tight we can make this double-tap procedure
  else if (gasbrake_mode == 4) {
    gasbrake_servo_us -= 10;
    
    if (gasbrake_servo_us <= 1400) {
      gasbrake_mode = 5;
      gasbrake_servo_us = GASBRAKE_NEUTRAL;
      neutral_hold = 0;
    }
  }

  // send another neutral signal for one second
  if (gasbrake_mode == 5) {
    neutral_hold += 1;
    
    if (neutral_hold > 40) {
      gasbrake_mode = 6;
      return;
    }
        
    if (gasbrake_servo_us == GASBRAKE_NEUTRAL) {
      gasbrake_mode = 6;
    }
  }  
  
  // complete the double-tap procedure to actually begin reversing
  else if (gasbrake_mode == 6) {
    gasbrake_servo_us -= 1;
    
    if (gasbrake_servo_us == 1300) {
      gasbrake_mode = 7;
    }
  }
  
  // slow the reverse towards neutral
  else if (gasbrake_mode == 7) {
    gasbrake_servo_us += 10;
    
    if (gasbrake_servo_us == GASBRAKE_NEUTRAL) {
      gasbrake_mode = 8;
    }
  }

  // send a neutral signal for one second
  else if (gasbrake_mode == 8) {
    gasbrake_servo_us = GASBRAKE_NEUTRAL;
    
    neutral_hold += 1;
    
    if (neutral_hold > 40) {
      // after sending a neutral signal for 1 second, start sequence again
      // beginning with the 5-second delay to bypass throttle neutral
      // protection
      gasbrake_mode = 0;
      neutral_hold = 0;
      loop_counter = 0;
    }
  }
  
  // collision response
  else if (gasbrake_mode == 9) {
    gasbrake_servo_us = GASBRAKE_NEUTRAL;
  }
  
  return;
}

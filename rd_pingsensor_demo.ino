/*******************************************************************************
filename: rd_pingsensor_demo.ino
author:   Augustine (github: mr-augustine)
          http://pushbuttonrobotics.weebly.com/
date:     23 May 2014

The rd_pingsensor_demo sketch is a basic demonstration of ping sensor control
using timers and interrupts. The ping sensor readings are shown via three LEDs
which represent three contiguous obstacle ranges: FAR_FIELD, NEAR_FIELD, and
DANGER_CLOSE.
*******************************************************************************/
#include "rd_pingsensor_globals.h"

volatile uint8_t timer1_overflow = 0;
volatile uint16_t ping_status;


////////////////////////////////////////////////////////////////////////////////
// Defines
#define LOOP_PERIOD 6250
#define PING_PCICR_GROUP (1 << 2)      // PCICR bit 2
#define FLEFT_PING_PIN   (1 << 4)      // PortD register, bit 4 => Digital Pin 4
#define DNGRCLOSE_LED_PIN (1 << 0)
#define NEARFIELD_LED_PIN (1 << 1)
#define FARFIELD_LED_PIN  (1 << 2)
//#define FCENTER_PING_PIN (1 << 5)      // PortD register, bit 5 => Digital Pin 5
//#define FRIGHT_PING_PIN  (1 << 6)      // PortD register, bit 6 => Digital Pin 6


////////////////////////////////////////////////////////////////////////////////
// Global Variables
uint32_t loop_counter;
volatile uint16_t ping_start_ticks;
volatile uint16_t ping_end_ticks;
volatile uint32_t ping_duration_us;
volatile uint8_t ping_sent;

////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routines
ISR(TIMER1_OVF_vect) {                                       // TIMER1 OVERFLOW
  timer1_overflow = 1;
}

ISR(TIMER0_COMPA_vect) {
  PIND |= FLEFT_PING_PIN;     // end the ping sensor trigger pulse
  DDRD &= ~FLEFT_PING_PIN;    // set ping pins as input pins
  PCMSK2 |= FLEFT_PING_PIN;   // enable interrupts on ping pin to get echo
  PCICR |= (1 << 2);          // enable interrupts on ping pin group
}

ISR(PCINT2_vect) {                                         // PING ECHO RECEIVED
  // for the start of the echo pulse (from LOW to HI)
  if (PIND & FLEFT_PING_PIN) {
    //PORTB |= FARFIELD_LED_PIN;
    if (ping_sent == 0) {
      //PORTB |= FARFIELD_LED_PIN;
      ping_sent = 1;
      ping_start_ticks = TCNT1;
    }
  }
  // for the end of the echo pulse (from HI to LOW)
  else if (PIND & ~FLEFT_PING_PIN) {
    //PORTB &= ~FARFIELD_LED_PIN;
    ping_end_ticks = TCNT1;
    ping_duration_us = (ping_end_ticks - ping_start_ticks) << 2;
    
    if (ping_duration_us < 6243) {
      PORTB |= DNGRCLOSE_LED_PIN;
      PORTB &= ~NEARFIELD_LED_PIN;
      PORTB &= ~FARFIELD_LED_PIN;
    }
    else if (ping_duration_us > 12373) {
      PORTB |= FARFIELD_LED_PIN;
      PORTB &= ~DNGRCLOSE_LED_PIN;
      PORTB &= ~NEARFIELD_LED_PIN;
    }
    else if (ping_duration_us > 6243 && ping_duration_us < 12373) {
      PORTB |= NEARFIELD_LED_PIN;
      PORTB &= ~DNGRCLOSE_LED_PIN;
      PORTB &= ~FARFIELD_LED_PIN;
    }
    // all LEDs turned off as default
    else {
      PORTB &= ~DNGRCLOSE_LED_PIN & ~NEARFIELD_LED_PIN & ~FARFIELD_LED_PIN;
    }
  }
  //}
}

////////////////////////////////////////////////////////////////////////////////
// Function Declarations


void setup() {
  noInterrupts();

  ////////////////////////////////////////////////////////////////////////////
  // Configure Ping Sensors
  PORTD = 0;            // disable pull-up resistors;
  //DDRD = 0b01110000;    // set ping pins as output pins
  DDRD = 0;
  PCICR = 0;            // disable interrupts on the ping pin group
  //PCICR = 0b00000100;   // enable pin change interrupts for the pin group
  PCMSK2 &= ~FLEFT_PING_PIN;  // disable interrupts on pin D4

  ////////////////////////////////////////////////////////////////////////////
  // Configure TIMER0
  // used to trigger the ping sensors
  TCCR0A = 0b00000000;  // normal compare output mode & normal WGM
  TCCR0B = 0b00000001;  // set prescaler value to 1 -> 0.0625 us/tick
  TIMSK0 = 0b00000010;  // enable output compare interrupt to end ping trigger
  OCR0A = 64;           // 64 ticks / (0.0625 us/tick) = 4 us
                        // ping trigger signal must be 2 to 5 us wide
                        // oscilloscope show it's approx 4.95 us wide
  // the timer setting below isn't as stable; it frequently goes beyond 5 us 
  //TCCR0B = 0b00000010;  // set prescaler value to 8 -> 0.5 us/tick; TOP = 255
  //OCR0A = 8;            // 8 ticks / (0.5 us/tick) = 4 us
  
  ////////////////////////////////////////////////////////////////////////////
  // Configure TIMER1
  // used to receive echo return timing
  TCCR1A = 0b00000000;  // normal compare output mode & normal WGM
  TCCR1B = 0b00000011;  // set prescaler value to 64
  TCCR1C = 0b00000000;  // disable force output compare
  TIMSK1 = 0b00000000;  // output compare & timer overflow interrups
                        // temporarily disabled (OCIE1B, OCIE1A, TOIE1)
                        
  ////////////////////////////////////////////////////////////////////////////
  // Configure PortC
  DDRC = 0;
  PORTC = 0;
  PCICR = 0;

  ////////////////////////////////////////////////////////////////////////////
  // Configure LEDs
  DDRB = 0b00000111;    // set LED pins as output pins (Digital Pins 8, 9, 10)
  PORTB = 0;            // disable pull-up resistors; using external resitors
  
  interrupts();
  
  TIMSK1 = 0b00000001;   // only enable TIMER1 overflow interrupts
  loop_counter = 0;
}

void loop() {
  TCNT1 = 0;            // reset timer/counter 1 for each loop iteration
  TCNT0 = 0;            // reset timer/counter 0 for the ping trigger
  
  ////////////////////////////////////////////////////////////////////////////
  // Start Ping
  PORTD = 0;                  // set the ping pin low
  ping_sent = 0;
  ping_start_ticks = 0;
  
  PCMSK2 &= ~FLEFT_PING_PIN;  // disable interrupts on pin D4
  DDRD |= FLEFT_PING_PIN;     // set ping pins as output pins
  PIND |= FLEFT_PING_PIN;     // toggle from LOW to HI


  loop_counter += 1;
  timer1_overflow = 0;


  while (1) {
    if (timer1_overflow) {
      return;
    }

    if (TCNT1 > LOOP_PERIOD) {
      return;
    }
  }
}

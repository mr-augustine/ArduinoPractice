/*******************************************************************************
filename: rd_locomotion_demo.ino
author:   Augustine (github: mr-augustine)
          http://pushbuttonrobotics.weebly.com/
date:     15 May 2014

The rd_locomotion_demo sketch uses a timer and interrupts to control the
steering servo and Electronic Speed Control (ESC) for an R/C vehicle (Traxxas
Stampede). This demo sweeps the front wheels from right to left, and left to 
right. It also drives the motor forward and then in reverse.

This sketch is a timers-and-interrupts version of a sketch created by Jeff
McAlvay. http://jeffsinventions.com/autonomous-car-steering-and-acceleration/
*******************************************************************************/
volatile uint8_t timer1_overflow = 0;

// The main loop period is in units of TIMER1 counts. TIMER1 is configured to
// run at 250 kHz by prescaling the Arduino's main timer by 64. (16 MHz / 64 =
// 250,000 Hz). Our desired loop rate is 40 Hz, therefore our loop period
// (expressed as TIMER1 ticks) will be 250,000 Hz / 40 Hz = 6250 ticks per loop.
#define LOOP_PERIOD 6250

#define STEERING_PORT PORTD
#define GASBRAKE_PORT PORTD
#define STEERING_PIN (1 << 2)  //PortD register, bit 2 => Digital Pin 2
#define GASBRAKE_PIN (1 << 3)  //PortD register, bit 3 => Digital Pin 3

// initialize control pulsewidths to neutral (i.e., no turning, no driving)
uint16_t steering_servo_us = 1500;
uint16_t gasbrake_servo_us = 1500;
uint8_t steer_mode = 0;
uint8_t gasbrake_mode = 0;
uint8_t resweep = 0;  // indicates the start of a new steering servo sweep
uint8_t neutral_hold = 0;  // counter used to maintain a pwm value for several
                           // loop iterations

// keeps track of how many times the main loop is executed
uint32_t loop_counter;

// adjusts the pulsewidth values for the steering and ESC
void update_control_values(void);

// Interrupt Service Routines
// negating the steering and gasbrake pins terminates the PWM pulse
ISR(TIMER1_COMPA_vect) {
  STEERING_PORT &= ~STEERING_PIN;
}

ISR(TIMER1_COMPB_vect) {
  GASBRAKE_PORT &= ~GASBRAKE_PIN;
}

ISR(TIMER1_OVF_vect) {
  timer1_overflow = 1;
}

void setup() {
  // temporarily disable interrupts before configuring the registers
  noInterrupts();
  
  // 0 - Disable pull-up resistors on input pins, drive the steering and
  //     gasbrake pins to low; set steering and gasbrake pins as output pins
  PORTD = 0;
  DDRD = 0b00001100;
  
  // configure TIMER1
  // 1 - Set normal compare output mode and normal waveform generation mode
  TCCR1A = 0b00000000;
  
  // 2 - Set no input capture "stuff", normal waveform generation mode, 
  //     set prescaler value to 64
  TCCR1B = 0b00000011;
  
  // 3 - Disable the force output compare feature
  TCCR1C = 0b00000000;
  
  // 4 - Set no input compare interrupts, no output compare interrupts, no
  //     overflow interrupts (most of these will be changed during the loop;
  //     in particular: OCIE1B, OCIE1A, TOIE1)
  TIMSK1 = 0b00000000;
  
  // re-enable interrupts
  interrupts();
  
  // only enable TIMER1 overflow interrupts
  TIMSK1 = 0b00000001;
  loop_counter = 0;
}

void loop() {
  // reset timer/counter 1
  TCNT1 = 0;
  
  // start the PWM pulse for steering and gasbrake
  PORTD |= (GASBRAKE_PIN | STEERING_PIN);
  
  // set the output compare registers with the timer values that correspond
  // with the end of the PWM pulses
  OCR1A = steering_servo_us >> 2;
  OCR1B = gasbrake_servo_us >> 2;
  
  // also enable output compare timers to trigger when there's a match
  // so the program can end the PWM pulses
  TIMSK1 = 0b00000111;
  
  loop_counter += 1;
  timer1_overflow = 0;


  update_control_values();

  /* 
     If there's still time to spare during the loop period, the program will
     sit in this while-loop and wait until the TIMER1 value reaches the 
     loop period count. If instead the loop is running really slow (i.e.,
     TIMER1 counted all the way up to its TOP value: 249,999), then the 
     program will immediately restart the loop() function. Since this main loop
     is very small, we shouldn't have to worry about this. The segment below
     is a carry-over from Rocksteady.
  */
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
  /*-----------------  
    Steering Control
    -----------------*/ 
  if (steer_mode == 0) {
    steering_servo_us = 1500;
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
  if (loop_counter <= 200) {
    gasbrake_servo_us = 1500;
    neutral_hold = 0;
    return;
  }

  // send one last neutral signal
  if (gasbrake_mode == 0) {
    gasbrake_servo_us = 1500;
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
    
    if (gasbrake_servo_us == 1500) {
      gasbrake_mode = 3;
      neutral_hold = 0;
    }
  }
  
  // after moving forward, stay in neutral for 1 second
  else if (gasbrake_mode == 3) {
    gasbrake_servo_us = 1500;
    
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
      gasbrake_servo_us = 1500;
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
        
    if (gasbrake_servo_us == 1500) {
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
    
    if (gasbrake_servo_us == 1500) {
      gasbrake_mode = 8;
    }
  }

  // send a neutral signal for one second
  else if (gasbrake_mode == 8) {
    gasbrake_servo_us = 1500;
    
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
  
  return;
}

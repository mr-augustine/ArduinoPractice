/*******************************************************************************
filename: rd_pushbutton_once.ino
author:   Augustine (github: mr-augustine)
          http://pushbuttonrobotics.weebly.com/
date:     18 May 2014

The rd_pushbutton_once sketch turns on an LED when a pushbutton switch is 
pressed. This sketch only activates the LED after the first button press. Use a
momentary pushbutton with this sketch. If you use a push on/off button, you
have to remember to retoggle the button to the OFF position before you start
the sketch again.

This sketch was written to mimic Rocksteady's timing setup and main function.
The pushbutton feature is just one part of the larger Rocksteady framework. 
*******************************************************************************/
volatile uint8_t timer1_overflow = 0;

// The main loop period is in units of TIMER1 counts. TIMER1 is configured to
// run at 250 kHz by prescaling the Arduino's main timer by 64. (16 MHz / 64 =
// 250,000 Hz). Our desired loop rate is 40 Hz, therefore our loop period
// (expressed as TIMER1 ticks) will be 250,000 Hz / 40 Hz = 6250 ticks per loop.
#define LOOP_PERIOD 6250

#define BUTTON_PIN (1 << 2)
#define LED_PIN (1 << 1)

uint32_t loop_counter;
uint8_t button_pressed;

void update_pushbutton(void);

ISR(TIMER1_OVF_vect)
{
  timer1_overflow = 1;
}

void setup() {
  // temporarily disable pin change interrupts
  PCICR = 0;
  // temporarily disable pin change interrupts on PCINT[14:8] - all Analog Pins
  PCMSK1 = 0;

  // configure the pushbutton pin as an input pin (Analog Pin 2, PCINT10).
  // nothing else is plugged into the analog pins, so make them inputs as well.
  DDRC = 0;  
  
  // turn on the pull-up resistor for the pushbutton pin
  // PC0 (A0) -> input, not used
  // PC1 (A1) -> input, not used
  // PC2 (A2) -> input, pushbutton
  // PC3 (A3) -> input, not used
  // PC4 (A4) -> input, not used
  // PC5 (A5) -> input, not used
  // PC5 (reset button)
  PORTC = 0b0000100;


  // configure the LED pin as an output pin (Digital Pin 9)
  DDRB = 0b00000010;
  
  // turn off the pull-up resistor for the LED pin
  // PB0 ( 8) -> input, not used
  // PB1 ( 9) -> output, LED
  // PB2 (10) -> input, not used
  // PB3 (11) -> input, not used
  // PB4 (12) -> input, not used
  // PB5 (13) -> input, not used
  // PB6 (crystal)
  // PB7 (crystal)
  PORTB = 0;
  
  // none of the port D pins are used so we can disable the pull-ups
  // and set them as input pins
  PORTD = 0;
  DDRD = 0;
  
  // configure TIMER1 for prescaler of 64
  TCCR1A = 0;
  TCCR1B = 0b00000011;
  TCCR1C = 0;
  TIMSK1 = 0;
  
  interrupts();
  
  // enable TIMER1 overflow interrupts
  TIMSK1 = 0b00000001;
  loop_counter = 0;
}

void loop() {
  TCNT1 = 0;
  
  loop_counter += 1;
  timer1_overflow = 0;
  
  update_pushbutton();

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

void update_pushbutton() {
  // if the button pin is HI, then the button wasn't pressed; so keep the LED
  // OFF. this pin will normally be HI since the pullup resistor is turned ON
  if (PINC & BUTTON_PIN) {
    button_pressed = 0;
    PORTB |= (0 << 1);
  }
  
  // if the pin goes LOW, then the button was pressed
  else {
    button_pressed = 1;
    PORTB |= (1 << 1);
  }
}

/*******************************************************************************
filename: rd_pushbutton_toggle.ino
author:   Augustine (github: mr-augustine)
          http://pushbuttonrobotics.weebly.com/
date:     18 May 2014

The rd_pushbutton_toggle sketch uses an interrupt to toggle an LED each time 
a pushbutton is pressed. Use a momentary pushbutton to cause the button pin
to toggle (i.e., from HI -> LOW -> HI) each time the button is pressed. If you
use a push on/off button, you'll have to double-press the button to achieve the
same effect.

(Don't forget to use a capacitor across the pushbutton input and ground to 
smoothen the button press signal.)

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
uint8_t start_button = 0;


ISR(TIMER1_OVF_vect)
{
  timer1_overflow = 1;
}

// Interrupt Service Routine (ISR) to handle the button press.
// remember: an interrupt is made when the pin toggles.
// see section 13 in the Atmel datasheet (External Interrupts).
// since the pull-up resistor is activated on our button pin and we're using a
// momentary pushbutton, the pin toggles from high to low and back to high
// each time the button is pressed.
ISR(PCINT1_vect) {
  if (PINC & BUTTON_PIN) {
    // writing a 1 to this bit will toggle the output for the pin.
    // see section 14.2.2 in the Atmel datasheet (Toggling the Pin)
    PINB |= (1 << 1);
  }
  
  // since there are no other interrupts configured for this pin group in
  // this sketch, we could get rid of the if-statement. but in general, it's
  // better to verify that the interrupt occured on the exact pin we're 
  // interested in.
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
  // PC5 (reset button) -> for system use
  PORTC = 0b0000100;

  // enable pin change interrupts for the group of pins that includes A2
  PCICR = 0b00000010; 
  // enable interrupts on pin A2 (PCINT10)
  PCMSK1 |= (1 << 2);


  // configure the LED pin as an output pin (Digital Pin 9)
  DDRB = 0b00000010;
  
  // turn off the pull-up resistor for the LED pin
  // PB0 ( 8) -> input, not used
  // PB1 ( 9) -> output, LED
  // PB2 (10) -> input, not used
  // PB3 (11) -> input, not used
  // PB4 (12) -> input, not used
  // PB5 (13) -> input, not used
  // PB6 (crystal) -> for system use
  // PB7 (crystal) -> for system use
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
  
  
  while (1) {
    if (timer1_overflow) {
      return;
    }
    
    if (TCNT1 > LOOP_PERIOD) {
      return;
    }
  }
}

/*******************************************************************************
  filename: QuadEncoderSimSlow.ino
  author:   Augustine (github: mr-augustine)
  date:     18 April 2014

  The QuadEncoderSimSlow sketch uses a timer to simulate a quad encoder rotating 
  very slowly.
  
  This sketch produces two rectangular signals to simulate encoder channels 
  A and B. Since the pulse rate is so slow, the blink sequence can be easily 
  seen using two LEDs.
*******************************************************************************/

#define redLED 9
#define greenLED 10

void setup()
{
  // configure the LED pins as output pins
  //pinMode(redLED, OUTPUT);
  //pinMode(greenLED, OUTPUT);

  // alternatively, configure the LED pins as output pins by setting the
  // data direction register for pin group B
  // PINB1 == digital pin 9; PINB2 == digital pin 2
  DDRB |= (1 << PINB1) | (1 << PINB2);
  
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  
  OCR1A = 62500;
  OCR1B = 31250;
  TCCR1B |= (1 << CS12);  //set prescaler to 256
  
  // set pins 9 and 10 to be output pins
  //DDRB |= (1 << X) | (1 << Y);
  
  // set toggle on compare match for OC1A and OC1B
  TCCR1A |= (1 << COM1A0) | (1 << COM1B0);
  
  // set clear timer on compare (CTC) mode
  // timer is cleared when TCNT1 == OCR1A
  TCCR1B |= (1 << WGM12);
    
  interrupts();
}

void loop()
{
}

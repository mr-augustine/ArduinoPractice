/*******************************************************************************
  filename: QuadEncoderSim5mps.ino
  author:   Augustine (github: mr-augustine)
  date:     18 April 2014

  The QuadEncoderSim5mps sketch uses a timer to simulate a quad encoder rotating 
  at approximately 5 meters per second on a Traxxas Stampede R/C truck chassis.
  The encoder being simulated is a US Digital E4P with an encoder disc that 
  produces 1440 pulses per revolution. The encoder is placed on the truck's 
  transmission.
  
  This sketch produces two rectangular signals to simulate encoder channels 
  A and B. The signals are fed into two LEDs. Since the pulse rate is so fast, 
  the LEDs will appear solid to human eyes.
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
  
  OCR1A = 600;  // really 601.6, but 600 should be ok
  OCR1B = 300;  // since they're 90 deg out of phase OCR1B = OCR1A / 2
  TCCR1B |= (1 << CS10);  //set prescaler to 1
  
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

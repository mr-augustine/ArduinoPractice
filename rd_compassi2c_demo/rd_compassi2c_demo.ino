/*******************************************************************************
filename: rd_compassi2c_demo.ino
author:   Augustine (github: mr-augustine)
          http://pushbuttonrobotics.weebly.com/
date:     25 May 2014

The rd_compass_demo sketch is a demonstration of compass control using timers,
interrupts, and the I2C (or TWI) protocol. The compass readings are shown via
four LEDs which represent the quadrants between the cardinal headings. These
regions are: North-East, South-East, South-West, and North-West.

The CMPS10 compass is used in this demo. It is a tilt-compensated compass.
The specs can be found here:
http://www.robot-electronics.co.uk/htm/cmps10doc.htm
http://www.robot-electronics.co.uk/htm/cmps10i2c.htm

This code is based on the Rocksteady framework.
*******************************************************************************/
#include "rd_compassi2c_globals.h"

volatile uint8_t timer1_overflow = 0;

////////////////////////////////////////////////////////////////////////////////
// Constants
#define LOOP_PERIOD                        6250
#define LOOP_FREQUENCY_HZ                  40.0

#define NE_LED_PIN                         (1 << 0)
#define SE_LED_PIN                         (1 << 1)
#define SW_LED_PIN                         (1 << 2)
#define NW_LED_PIN                         (1 << 3)


////////////////////////////////////////////////////////////////////////////////
// Compass Variables
volatile uint8_t compass_status;
volatile uint8_t compass_reading_ready = 0;
volatile uint16_t compass_reading = 0;
volatile uint8_t compass_error = 0;
volatile uint8_t compass_active = 0;
uint16_t compass_raw = 0;
uint16_t compass_deg = 0;
uint16_t compass_rad = 0;

uint32_t loop_counter;

////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routines
ISR(TIMER1_OVF_vect) {                                       // TIMER1 OVERFLOW
  timer1_overflow = 1;
}

ISR(TWI_vect) {                                              // COMPASS HEADING
  uint8_t status = TW_STATUS;
  
  switch (status) {
    // start sequence was sent; now send the compass address + write
    case TW_START:
      TWDR = (COMPASS_ADDR << 1) | TW_WRITE;
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
      break;
    
    // compass address was received; now send the register value that contains
    // the 16-bit compass heading
    case TW_MT_SLA_ACK:
      TWDR = COMPASS_READING_REG;
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
      break;
      
    // compass acknowledged receiving data (i.e., the register value) now send
    // repeat-start signal to begin compass heading transfer
    case TW_MT_DATA_ACK:
      TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
      break;
      
    // repeat-start sent, now send compass address + read
    case TW_REP_START:
      TWDR = (COMPASS_ADDR << 1) | TW_READ;
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
      break;
      
    // compass acknowledged read request and will send data; now prepare to
    // read compass heading, and then send acknowledge signal
    case TW_MR_SLA_ACK:
      TWCR = (1 << TWEA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
      // TODO: perhaps we should move the master ack to the next case (see pg 235)
      break;
      
    // received high order byte of compass heading; process the high order
    // byte; low order byte should follow
    case TW_MR_DATA_ACK:
      compass_reading = TWDR;
      compass_reading = compass_reading << 8;
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
      break;
      
    // received low order byte of compass heading follwed by NACK; process the
    // low order bye and send a stop signal
    // note: the compass_reading is given in tenths of degrees [0..3599]
    case TW_MR_DATA_NACK:
      compass_reading |= TWDR;
      compass_reading_ready = 1;
      TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
      compass_active = 0;
      break;
    
    // if something unexpected occurs at any point in this transfer sequence,
    // treat it as an error
    default:
      compass_error = 1;
      compass_reading = 0xEEEE;
      compass_active = 0;
      TWCR = (1 << TWSTO) | (1 << TWEN);
  }
}
////////////////////////////////////////////////////////////////////////////////
// Function Declarations

// prepares the compass for use
void init_compass(void);

// processes a received compass value and turns on an LED which corresponds
// with the heading
void update_compass(void);

void setup() {
  noInterrupts();
  
  ////////////////////////////////////////////////////////////////////////////
  // Configure Compass Sensor
  // PC4 (A4) -> SDA, compass
  // PC5 (A5) -> SCL, compass
  DDRC = 0b00110000;    // configure the compass pins as output pins
  PORTC = 0;            // disable pull-up resistors

  
  ////////////////////////////////////////////////////////////////////////////
  // Configure LEDs
  DDRB = 0b00001111;    // set LED pins as output pins (Digital Pins 8 - 11)
  PORTB = 0;            // disable pull-up resistors; using external resistors
  
  ////////////////////////////////////////////////////////////////////////////
  // Configure TIMER1
  TCCR1A = 0b00000000;  // normal compare output mode & normal WGM
  TCCR1B = 0b00000011;  // set prescaler value to 64
  TCCR1C = 0b00000000;  // disable force output compare
  TIMSK1 = 0b00000000;  // disable output compare & timer overflow interrups

  init_compass();

  interrupts();
  
  TIMSK1 = 0b00000001;
  loop_counter = 0;
}

void loop() {
  TCNT1 = 0;            // reset timer/counter 1 for each loop iteration

  loop_counter += 1;
  timer1_overflow = 0;

  update_compass();  


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
  The init_compass() function description goes here
*/
void init_compass(void) {
  compass_reading_ready = 0;
  compass_reading = 0;
  compass_error = 0;
  compass_active = 0;

  // configure SCL to 100 kHz (see Atmel Sect 22.5.2 Bit Rate Generator Unit)
  // where CPU_clock_freq = 16 MHz; TWI_Prescale_Value = 1
  TWBR = 72;
  
  // enable the Two-Wire Interface and its interrupts
  TWCR = (1 << TWEN) | (1 << TWIE);
}

/*
  The update_compass() function description goes here
*/
void update_compass(void) {
  compass_raw = 0;
  compass_deg = 0;
  
  if (compass_reading_ready == 1) {
    compass_raw = compass_reading;
    compass_deg = (compass_raw + COMPASS_OFFSET_RAW) / 10.0;
    compass_rad = compass_deg * DEG_TO_RAD;
    
    if (compass_deg >= 0 && compass_deg < 90) {
      // turn on North-East LED
      PORTB |= NE_LED_PIN;
      PORTB &= ~SE_LED_PIN & ~SW_LED_PIN & ~NW_LED_PIN;
    }
    
    else if (compass_deg >= 90 && compass_deg < 180) {
      // turn on South-East LED
      PORTB |= SE_LED_PIN;
      PORTB &= ~NE_LED_PIN & ~SW_LED_PIN & ~NW_LED_PIN;
    }
    
    else if (compass_deg >= 180 && compass_deg < 270) {
      // turn on South-West LED
      PORTB |= SW_LED_PIN;
      PORTB &= ~NE_LED_PIN & ~SE_LED_PIN & ~NW_LED_PIN;
    }
    
    else if (compass_deg >= 270 && compass_deg <= 359) {
      // turn on North-West LED
      PORTB |= NW_LED_PIN;
      PORTB &= ~NE_LED_PIN & ~SE_LED_PIN & ~SW_LED_PIN;
    }
    
    else {
      // turn off all LEDs
      PORTB &= ~NE_LED_PIN & ~SE_LED_PIN & ~SW_LED_PIN & ~NW_LED_PIN;
    }
  }
  
  // we won't bother with the compass reading error condition in this demo
  // (i.e., if (compass_error == 1))
  
  // reset the compass variables
  compass_error = 0;
  compass_reading = 0xFFFF;
  compass_reading_ready = 0;
  compass_active = 1;
  
  // prepare for another compass reading
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
}

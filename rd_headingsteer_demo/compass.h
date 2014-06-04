#ifndef _compass_h_
#define _compass_h_

#include <avr/interrupt.h>
#include "rd_twi.h"
#include "RobotDevil.h"
#include "globals.h"

////////////////////////////////////////////////////////////////////////////////
// Compass Constants
#define COMPASS_ADDR                0x60
#define COMPASS_READING_REG         2
#define COMPASS_OFFSET_RAW          0       // if the compass is not oriented
                                            // north-up, use this to reorient
                                            // the readings

////////////////////////////////////////////////////////////////////////////////
// Compass Variables
volatile uint8_t  compass_reading_ready = 0;
volatile uint16_t compass_reading = 0;
volatile uint8_t  compass_error = 0;
volatile uint8_t  compass_active = 0;


////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routine
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
// Compass Functions
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

void update_compass(void) {
  globals.status_bits &= ~(STATUS_COMPASS_VALID);  // clear the status bit
  globals.compass_raw = 0;
  globals.compass_deg = 0.0;
  
  // this condition should never happen
  if (compass_active) {
    return;
  }
  
  if (compass_reading_ready == 1) {
    globals.compass_raw = compass_reading;
    globals.compass_deg = (globals.compass_raw + COMPASS_OFFSET_RAW) / 10.0;
    globals.compass_rad = globals.compass_deg * DEG_TO_RAD;
    globals.status_bits |= STATUS_COMPASS_VALID;
  }

  else if (compass_error == 1) {
    globals.status_bits |= STATUS_COMPASS_ERROR;
  }
  
  // reset the compass variables
  compass_error = 0;
  compass_reading = 0xFFFF;
  compass_reading_ready = 0;
  compass_active = 1;
  
  // prepare another compass reading
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
}

#endif

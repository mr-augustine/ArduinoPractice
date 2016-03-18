#include <avr/interrupt.h>
#include "cmps10.h"
#include "twi.h"


/* We're using an unterminated do-while loop so that we can use a semicolon
 * when invoking the macro in our updated_xxx() functions. It's for the sake of
 * readability below.
 */
#define INIT_READING   \
do { TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWSTA); } \
while (0)

static volatile uint16_t heading_reading = 0;
static volatile uint8_t heading_ready = 0;

static volatile uint8_t pitch_reading = 0;
static volatile uint8_t pitch_ready = 0;

static volatile uint8_t roll_reading = 0;
static volatile uint8_t roll_ready = 0;

static volatile uint8_t compass_active = 0;
static volatile uint8_t compass_error = 0;

static enum Cmps_Reg {
  Register_Heading = 0,
  Register_Pitch,
  Register_Roll
};

static volatile uint8_t requested_register = Register_Heading;

ISR(TWI_vect) {
  uint8_t status = TW_STATUS;

  switch (status) {
    // AVR sent start sequence; now send the compass address + write
    case TW_START:
      TWDR = (COMPASS_ADDR << 1) | TW_WRITE;
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
      break;

    /* Compass received the address; now send the register address for
     * the value we're interested in [i.e., heading (2), pitch (4), roll (5)]
     */
    case TW_MT_SLA_ACK:
      switch (requested_register) {
        case Register_Heading:
          TWDR = COMPASS_HEADING_REG;
          break;
        case Register_Pitch:
          TWDR = COMPASS_PITCH_REG;
          break;
        case Register_Roll:
          TWDR = COMPASS_ROLL_REG;
          break;
        default:
          break;
      }
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
      break; // switch (status)

    /* Compass received the address of the register we want;
     * now send a repeated start
     */
    case TW_MT_DATA_ACK:
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWSTA);
      break;

    // AVR sent the repeated start; now send the compass address + read
    case TW_REP_START:
      TWDR = (COMPASS_ADDR << 1) | TW_READ;
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
      break;

    // Compass acknowledged the read request; now send an acknowledgment
    case TW_MR_SLA_ACK:
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
      break;

    // Read the high byte of the heading value; expect another byte to follow
    case TW_MR_DATA_ACK:
      switch (requested_register) {
        case Register_Heading:
          heading_reading = TWDR;
          heading_reading = heading_reading << 8;
        break;
        /* We should only have a heading requested. It's the only multi-byte
         * value that we would request. The others should have a NACK after
         * the first and only returned byte.
         */
        case Register_Pitch:
        case Register_Roll:
        default:
        break;
      }

      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
      break;

    /* Low byte of the heading received OR pitch received OR roll received;
     * now read the data and send an acknowledgement
     */
    case TW_MR_DATA_NACK:
      switch (requested_register) {
        case Register_Heading:
          heading_reading |= TWDR;
          heading_ready = 1;
          compass_active = 0;
        break;

        case Register_Pitch:
          pitch_reading = TWDR;
          pitch_ready = 1;
          compass_active = 0;
        break;

        case Register_Roll:
          roll_reading = TWDR;
          roll_ready = 1;
          compass_active = 0;
        break;

        default:
          compass_error = 1;
          heading_reading = 0xEEEE;   // 0xE is for error
          compass_active = 0;
          TWCR = (1 << TWSTO) | (1 << TWEN);
        break;
      } // switch (requested_register)

      break;
    default:
      compass_error = 1;
      heading_reading = 0xEEEE;   // 0xE is for error
      pitch_reading = 0xBB;       // 0xB is for bad
      roll_reading = 0xBB;
      compass_active = 0;
      TWCR = (1 << TWSTO) | (1 << TWEN);
  } // switch (status)
}

/* Helper function to initiate a new compass reading. Resets the
 * helper variables and triggers the compass.
 */
static void begin_new_reading(void) {
  // Reset all variables in preparation for a new reading
  heading_reading = 0xFFFF;
  heading_ready = 0;

  pitch_reading = 0xFF;
  pitch_ready = 0;

  roll_reading = 0xFF;
  roll_ready = 0;

  compass_error = 0;
  compass_active = 1;

  // Initiate a new compass reading
  INIT_READING;

  return;
}

/* Initialzes the compass by enabling the Two-Wire Interface (TWI), and
 * sets the SCL clock frequency to 100 kHz.
 */
void cmps10_init(void) {
  compass_active = 0;
  compass_error = 0;

  heading_reading = 0;
  heading_ready = 0;

  pitch_reading = 0;
  pitch_ready = 0;

  roll_reading = 0;
  roll_ready = 0;


  // Set the SCL clock frequency to 100 kHz
  // See Section 22.5.2 in the Atmel Specsheet for the formula
  TWBR = 0x48;

  // Enable the two wire interface and enable interrupts
  TWCR = (1 << TWEN) | (1 << TWIE);

  return;
}

void cmps10_update_heading(void) {
  if (compass_active) {
    return;
  }

  if (heading_ready) {
    // TODO: Save the heading value to statevars
  }

  if (compass_error) {
    // TODO: Save the error to statevars
  }

  requested_register = Register_Heading;
  begin_new_reading();

  return;
}

void cmps10_update_pitch(void) {
  if (compass_active) {
    return;
  }

  if (pitch_ready) {
    // TODO: Save the pitch value to statevars
  }

  if (compass_error) {
    // TODO: Save the error to statevars
  }

  requested_register = Register_Pitch;
  begin_new_reading();

  return;
}

void cmps10_update_roll(void) {
  if (compass_active) {
    return;
  }

  if (roll_ready) {
    // TODO: Save the roll value to statevars
  }

  if (compass_error) {
    // TODO: Save the error to statevars
  }

  requested_register = Register_Roll;
  begin_new_reading();

  return;
}


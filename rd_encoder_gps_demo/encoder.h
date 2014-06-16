#include <avr/interrupt.h>
#include "globals.h"
#include "RobotDevil.h"

////////////////////////////////////////////////////////////////////////////////
// Encoder Constants


////////////////////////////////////////////////////////////////////////////////
// Encoder Variables
typedef struct
{
  uint8_t error_flag;
  int16_t count;
} encoder_state_t;

volatile uint8_t last_encoder_pos = 0;
volatile encoder_state_t encoder_states[2];
volatile uint8_t active_encoder_state = 0;

////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routine                            // ENCODER STATE CHANGE
ISR(PCINT1_vect)
{
  // This is quick and dirty way to decode the encoder.  In the
  // future, I'd like to optimize this code.  The faster it runs, the
  // less overhead the encoder takes, and the faster we can drive
  // without missing encoder ticks.
  uint8_t new_encoder_pos = PINC & 0x3;
  uint8_t diff = last_encoder_pos ^ new_encoder_pos;
  uint8_t A = (new_encoder_pos & 0x02) >> 1;
  uint8_t B = (new_encoder_pos & 0x01);

  if (diff == 0x00)
  {
    encoder_states[active_encoder_state].error_flag = 1;
  }
  else if (diff == 0x01)
  {
    if (A == B)
      encoder_states[active_encoder_state].count++;
    else
      encoder_states[active_encoder_state].count--;
  }
  else if (diff == 0x10)
  {
    if (A != B)
      encoder_states[active_encoder_state].count++;
    else
      encoder_states[active_encoder_state].count--;
  }
  else if (diff == 0x11)
  {
    encoder_states[active_encoder_state].error_flag = 1;
  }

  last_encoder_pos = new_encoder_pos;
}


////////////////////////////////////////////////////////////////////////////////
// Encoder Functions
void init_encoder(void)
{
  last_encoder_pos = 0;
  encoder_states[0].error_flag = 0;
  encoder_states[0].count = 0;
  encoder_states[1].error_flag = 0;
  encoder_states[1].count = 0;
  active_encoder_state = 0;
  last_encoder_pos = PINC & 0x3;

  // enable pin change interrupts for the pin group that includes A0 and A1.
  PCICR = 0b00000010;

  // enable pin-change interrupts on pins A0 & A1 (ie. encoder Ch A & Ch B)
  PCMSK1 |= (1 << 0) | (1 << 1);
}

void update_encoder(void)
{
  // We have to be careful about how we read the encoder counter since
  // the encoder ISR could run during this function.  We don't want to
  // mask the interrupts because that risks missing a count, and we
  // don't want to add a mutex to the ISR since we want it to be as
  // lean as possible.  The strategy we use is to have two counters
  // that we can swap with a single atomic operation.  We do that by
  // having an array of two counters indexed by a single byte, which
  // we can change atomically.  This works because the ISR can
  // interrupt this routine, but this routine cannot interrupt the
  // ISR.
  //

  // First, clear out the current inactive counter before making it
  // active.
  uint8_t orig_active = active_encoder_state;
  uint8_t orig_inactive = active_encoder_state ^ 1;
  encoder_states[orig_inactive].error_flag = 0;
  encoder_states[orig_inactive].count = 0;

  // Swap the active counter.  Since this is a single byte write, it
  // will happen in one atomic operation.
  active_encoder_state = orig_inactive;

  // At this point, the inactive encoder counter will not change, so
  // it is safe to read.
  globals.status_bits |= STATUS_ENCODER_VALID;
  if (encoder_states[orig_active].error_flag)
    globals.status_bits |= STATUS_ENCODER_ERROR;
  globals.encoder_ticks = encoder_states[orig_active].count;
}



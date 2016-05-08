/*
 * file:  main.c
 * author: mr-augustine
 * date: 20160503
 *
 * This file exercises the ledbutton, cmps10, uwrite, and sdcard components
 * on the Mega. Whenever the button is pressed, a message is written to the
 * serial port. Continuous updates of the compass are also written to the
 * serial port. All data are written to the sdcard during each iteration.
 */
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include "pins.h"
#include "cmps10.h"
#include "ledbutton.h"
#include "sd_card.h"
#include "statevars.h"
#include "uwrite.h"

/* The MAINLOOP_PERIOD_TICKS value should be some fraction of:
 * 16,000,000 / prescaler
 * Remember: the timer starts counting from zero; so subtract 1
 */
#define MAINLOOP_PERIOD_TICKS   6249  // loop period 25 ms

statevars_t statevars;
volatile uint8_t mainloop_timer_overflow = 0;

// Interrupt Service Routine that triggers if the main loop is running longer
// than it should.
ISR(TIMER1_OVF_vect) {
  mainloop_timer_overflow = 1;
}

int main(void) {
  cli();

  /* We need to use a timer to control how long our main loop iterations are.
   * Experiments showed that the main loop was executing approximately every
   * 3.2 milliseconds, which is too quick for the sdcard to write a block
   * of data. Here we are configuring Timer1 because it is a "16-bit" timer
   * which would allow us to count to values greater than (2^8)-1. For a pre-
   * scale value of 64, the timer will count up to 16,000,000/64 = 250,000.
   * This gives us a timer period of 250,000 ticks/sec = 4 microseconds/tick.
   * Suppose we want the main loop to run 40 times per second (40 Hz), we would
   * need to restart the loop when Timer1 reaches the value: 250,000/40 = 6250
   * See Atmel datasheet for Mega, Section 17.11
   */
  TCCR1A = 0b00000000;  // Normal operation; no waveform generation by default
  TCCR1B = 0b00000011;  // No input capture, waveform gen; prescaler = 64
  TCCR1C = 0b00000000;  // No output compare
  TIMSK1 = 0b00000000;

  // TIMING DEBUG - Digital Pin 4
  DDRG |= (1 << 5);

  char msg[64];

  if (button_init() &&
      uwrite_init() &&
      cmps10_init() &&
      sdcard_init()) {
    uwrite_print_buff("All systems go!\r\n");
    return 0;
  } else {
    uwrite_print_buff("There was an error during init\r\n");
    return 1;
  }

  memset(msg, 0, sizeof(msg));
  memset(&statevars, 0, sizeof(statevars));
  statevars.prefix = 0xDADAFEED;
  statevars.suffix = 0xCAFEBABE;

  uint32_t iterations = 0;

  sei();

  TIMSK1 = 0b00000001;

  while (1) {
    // TIMING DEBUG FOR OSCILLOSCOPE
    PORTG |= (1 << 5);

    TCNT1 = 0;

    button_update();
    cmps10_update_all();
    statevars.main_loop_counter = iterations;
    mainloop_timer_overflow = 0;

    // TIMING DEBUG FOR OSCILLOSCOPE
    PORTG &= (0 << 5);

    if (button_is_pressed()) {
      uwrite_print_buff("The button is pressed! The LED should be on.\r\n");
      led_turn_on();
      statevars.mission_started = 1;
    } else {
      led_turn_off();
      statevars.mission_started = 0;
    }

    snprintf(msg, sizeof(msg),
      "Heading: %u  Pitch: %d  Roll: %hhd\r\n",
     cmps10_heading, cmps10_pitch, cmps10_roll); 
     // Needed to cast these values to display negative pitch and roll values
     //(int8_t) statevars.heading_raw, (int8_t) statevars.pitch_deg, (int8_t) statevars.roll_deg); 
    uwrite_print_buff(msg);

    sdcard_write_data();

    iterations++;

    if (iterations > 256) {
      uwrite_print_buff("Finished collecting data!\r\n");
      break;
    }

    /* Ensure that the main loop period is as long as we want it to be.
     * This means (1) triggering the main loop to restart we notice it is 
     * running too long, and (2) performing busy waiting if the instructions
     * above finish before the desired loop duration.
     */
    while (1) {
      if (mainloop_timer_overflow) {
        break;
      }

      if (TCNT1 >= MAINLOOP_PERIOD_TICKS) {
        break;
      }
    }

  }

  return 0;
}


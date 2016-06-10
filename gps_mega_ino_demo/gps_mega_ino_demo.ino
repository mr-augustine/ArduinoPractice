/*
 * file: gps_mega_ino_demo.ino
 * created: 20160609
 * author(s): mr-augustine
 *
 * This file exercises the GPS and SD card libraries. The GPS data are
 * parsed and then stored to the SD card.
 */
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>

#include "robot.h"

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

void setup() {
  cli();

  /* We need to use a timer to control how long our main loop iterations are.
   * Here we are configuring Timer1 because it is a "16-bit" timer
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

  if (uwrite_init() &&
      gps_init() &&
      sdcard_init()) {
    uwrite_print_buff("All systems go!\r\n");
  } else {
    return 1;
  }

  memset(&statevars, 0, sizeof(statevars));
  statevars.prefix = 0xDADAFEED;
  statevars.suffix = 0xCAFEBABE;

  uint32_t iterations = 0;

  sei();

  TIMSK1 = 0b00000001;
}

void loop() {

  TCNT1 = 0;
  statevars.status = 0;

  gps_update();
  statevars.main_loop_counter = iterations;
  mainloop_timer_overflow = 0;

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

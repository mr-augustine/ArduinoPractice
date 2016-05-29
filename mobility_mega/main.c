#include <avr/interrupt.h>
#include <stdio.h>
#include "pins.h"
#include "mobility.h"

/* The MAINLOOP_PERIOD_TICKS value should be some fraction of:
 * 16,000,000 / prescaler
 * Remember: the timer starts counting from zero; so subtract 1
 */
#define MAINLOOP_PERIOD_TICKS   6249  // loop period 25 ms

//statevars_t statevars;
volatile uint8_t mainloop_timer_overflow = 0;
uint32_t iterations = 0;

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

  mobility_init();


  sei();

  TIMSK1 = 0b00000001;  // Enable only TIMER1 overflow interrupts

  while (1) {

    TCNT1 = 0;

    mainloop_timer_overflow = 0;

    DRIVE_PORT |= (1 << DRIVE_PIN);
    STEERING_PORT |= (1 << STEERING_PIN);

    // Bypass throttle neutral protection by sending neutral signals for at least
    // five seconds.
    // TODO shift the TNP procedure into the mobility initialization function
    /*if (iterations <= 250) {
      mobility_stop();
      steer_to_direction(1500);
    } else {
      mobility_drive_fwd(Drive_Creep);
      steer_to_direction(1200);
    }*/

    mobility_drive_few(Drive_Creep);
    steer_to_direction(1500);

    TIMSK1 = 0b00000111;  // Enable output compare timers to trigger

    while (1) {
      if (mainloop_timer_overflow) {
        break;
      }

      if (TCNT1 >= MAINLOOP_PERIOD_TICKS) {
        break;
      }
    } // end of busy waiting loop

    iterations++;

  } // end of main loop

  return 0;
}


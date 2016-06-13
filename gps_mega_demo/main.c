/*
 * File: main.c
 */
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include "gps.h"
#include "sd_card.h"
#include "statevars.h"
#include "uwrite.h"


/* The MAINLOOP_PERIOD_TICKS value should be some fraction of:
 * 16,000,000 / prescaler
 * Remember: the timer starts counting from zero; so subtract 1
 */
#define MAINLOOP_PERIOD_TICKS   6249  // loop period 25 ms

/*static void print_gpgga_fields(void);
static void print_gpgsa_fields(void);
static void print_gprmc_fields(void);
static void print_gpvtg_fields(void);*/

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

  if (uwrite_init() &&
      gps_init() &&
      sdcard_init()) {
    uwrite_print_buff("All systems go!\r\n");
  } else {
    uwrite_print_buff("There was an error during init\r\n");
    return 1;
  }

  char msg[128];
  memset(msg, 0, sizeof(msg));
  memset(&statevars, 0, sizeof(statevars));
  statevars.prefix = 0xDADAFEED;
  statevars.suffix = 0xCAFEBABE;

  uint32_t iterations = 0;

  sei();

  TIMSK1 = 0b00000001;
return 0;
  while (1) {

    TCNT1 = 0;
    statevars.status = 0;

    gps_update();
    statevars.main_loop_counter = iterations;
    mainloop_timer_overflow = 0;

    // Print all $GPGGA fields that we are interested in
    //print_gpgga_fields();

    // Print all $GPGSA fields that we are interested in
    //print_gpgsa_fields();

    // Print all $GPRMC fields that we are interested in
    //print_gprmc_fields();

    // Print all $GPVTG fields that we are interested in
    //print_gpvtg_fields();

    sdcard_write_data();

    iterations++;

    if (iterations > 512) {
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

/*static void print_gpgga_fields(void) {
  uwrite_print_buff("hours: ");
  uwrite_println_byte(&statevars.gps_hours);
  uwrite_print_buff("minutes: ");
  uwrite_println_byte(&statevars.gps_minutes);
  uwrite_print_buff("seconds: ");
  uwrite_println_long(&statevars.gps_seconds);
  uwrite_print_buff("lat: ");
  uwrite_println_long(&statevars.gps_latitude);
  uwrite_print_buff("long: ");
  uwrite_println_long(&statevars.gps_longitude);
  uwrite_print_buff("sat count: ");
  uwrite_println_byte(&statevars.gps_satcount);
  uwrite_print_buff("hdop: ");
  uwrite_println_long(&statevars.gps_hdop);
  uwrite_print_buff("msl alt: ");
  uwrite_println_long(&statevars.gps_msl_altitude_m);

  return;
}

static void print_gpgsa_fields(void) {
  uwrite_print_buff("pdop: ");
  uwrite_println_long(&statevars.gps_pdop);
  uwrite_print_buff("vdop: ");
  uwrite_println_long(&statevars.gps_vdop);

  return;
}

static void print_gprmc_fields(void) {
  uwrite_print_buff("gnd speed (kt): ");
  uwrite_println_long(&statevars.gps_ground_speed_kt);
  uwrite_print_buff("gnd course (deg): ");
  uwrite_println_long(&statevars.gps_ground_course_deg);
  uwrite_print_buff("date: ");
  uwrite_print_buff(statevars.gps_date);
  uwrite_print_buff("\r\n");

  return;
}

static void print_gpvtg_fields(void) {
  uwrite_print_buff("true course (deg): ");
  uwrite_println_long(&statevars.gps_true_hdg_deg);
  uwrite_print_buff("speed (kt): ");
  uwrite_println_long(&statevars.gps_speed_kt);
  uwrite_print_buff("speed (kmph): ");
  uwrite_println_long(&statevars.gps_speed_kmph);
 
  return;
}*/


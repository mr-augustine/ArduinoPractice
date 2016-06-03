/*
 * File: main.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gps.h"
#include "statevars.h"
#include "uwrite.h"

#include <util/delay.h>

static void print_gpgga_fields(void);
//static void print_gpgsa_fields(void);
//static void print_gprmc_fields(void);
//static void print_gpvtg_fields(void);

statevars_t statevars;

int main(void) {
  char msg[128];

  memset(msg, 0, sizeof(msg));

  uwrite_init();
  gps_init();

  uwrite_print_buff("Starting...\r\n");

  while (1) {
    statevars.status = 0;

    gps_update();

    // Print all $GPGGA fields that we are interested in
    print_gpgga_fields();

    // Print all $GPGSA fields that we are interested in
    //print_gpgsa_fields();

    // Print all $GPRMC fields that we are interested in
    //print_gprmc_fields();

    // Print all $GPVTG fields that we are interested in
    //print_gpvtg_fields();

    _delay_ms(250);
  }

  return 0;
}

static void print_gpgga_fields(void) {
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

/*static void print_gpgsa_fields(void) {

  return;
}*/

/*static void print_gprmc_fields(void) {

  return;
}*/

/*static void print_gpvtg_fields(void) {

  return;
}*/


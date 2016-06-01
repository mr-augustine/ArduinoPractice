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

statevars_t statevars;

int main(void) {
  char msg[128];

  memset(msg, 0, sizeof(msg));

  uwrite_init();
  gps_init();

  // It looks like the uwrite and gps libraries cannot play
  // nicely together on the same USART port. Switch to the Mega.
  uwrite_print_buff("Here\r\n");

  while (1) {
    statevars.status = 0;

    gps_update();

    //uwrite_print_buff("statevars.status: ");
    //uwrite_print_long(&statevars.status);
    /*snprintf(msg, sizeof(msg),
      "lat: %f long: %f\r\n",
      32.3, 33.4);*/
      //(double) statevars.gps_latitude, (double) statevars.gps_longitude); 
      //"satcount: %d\r\n",
      //statevars.gps_satcount);
    //uwrite_print_buff(msg);

    // using dtostrf() to print the floating point values
    //uwrite_print_buff(dtostrf(statevars.gps_latitude, 9, 4, "%f"));

    //memset(msg, '\0', sizeof(msg));
    //sprintf(msg, dtostrf(statevars.gps_latitude, 8, 4, "%f")); 
    /*snprintf(msg, sizeof(msg),
      dtostrf(statevars.gps_latitude, 8, 4, "%f\0"));
    uwrite_print_buff(msg);*/

    /*memset(msg, '\0', sizeof(msg));
    snprintf(msg, sizeof(msg),
      dtostrf(statevars.gps_longitude, 9, 4, "%f\0"));*/

    uwrite_print_buff("lat: ");
    uwrite_println_long(&statevars.gps_latitude);
    uwrite_print_buff("long: ");
    uwrite_println_long(&statevars.gps_longitude);

    /*memset(msg, 0, sizeof(msg));
    sprintf(msg, "sizeof(long double): %d\r\n", sizeof(long double));
    uwrite_print_buff(msg);*/
    /*uwrite_print_buff(", ");
    memset(msg, 0, sizeof(msg));
    snprintf(msg, sizeof(msg),
      dtostrf(statevars.gps_longitude, 5, 8, "%f"));*/
      //dtostrf(statevars.gps_longitude, 10, 4, "%f"));
    //uwrite_print_buff(msg);

    /*uwrite_print_buff("sentence0: ");
    uwrite_print_buff(statevars.gps_sentence0);
    uwrite_print_buff("\r\n");

    uwrite_print_buff("sentence1: ");
    uwrite_print_buff(statevars.gps_sentence1);
    uwrite_print_buff("\r\n");

    uwrite_print_buff("sentence2: ");
    uwrite_print_buff(statevars.gps_sentence2);
    uwrite_print_buff("\r\n");

    uwrite_print_buff("sentence3: ");
    uwrite_print_buff(statevars.gps_sentence3);
    uwrite_print_buff("\r\n");*/
    _delay_ms(250);
  }

  return 0;
}


/*
 * File: main.c
 */
#include <stdio.h>
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
    snprintf(msg, sizeof(msg),
      "lat: %f long: %f\r\n",
      32.3, 33.4);
      //(double) statevars.gps_latitude, (double) statevars.gps_longitude); 
      //"satcount: %d\r\n",
      //statevars.gps_satcount);
    uwrite_print_buff(msg);

    uwrite_print_buff(statevars.gps_sentence0);
    uwrite_print_buff("\r\n");
    uwrite_print_buff(statevars.gps_sentence1);
    uwrite_print_buff("\r\n");
    uwrite_print_buff(statevars.gps_sentence2);
    uwrite_print_buff("\r\n");
    uwrite_print_buff(statevars.gps_sentence3);
    uwrite_print_buff("\r\n");
    _delay_ms(1000);
  }

  return 0;
}


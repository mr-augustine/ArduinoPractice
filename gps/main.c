/*
 * File: main.c
 */
#include <stdio.h>

#include "gps.h"
#include "statevars.h"
//#include "uwrite.h"

statevars_t statevars;

int main(void) {

  //uwrite_init();
  gps_init();

  // It looks like the uwrite and gps libraries cannot play
  // nicely together on the same USART port. Switch to the Mega.
  //uwrite_print_buff("Here\r\n");

  while (1) {
    //statevars.status = 0;

    //gps_update();

    //uwrite_print_buff("statevars.status: ");
    //uwrite_print_long(&statevars.status);
  }

  return 0;
}


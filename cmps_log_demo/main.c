/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmps10.h"
#include "ledbutton.h"
#include "pins.h"
#include "sd_card.h"
#include "statevars.h"
#include "uwrite.h"

statevars_t statevars;

int main(void) {
  uwrite_init(); 
  button_init();
  spi_init();
  sdcard_init();
  cmps10_init();

  memset(&statevars, 0, sizeof(statevars));

  char msg[32];
  memset(msg, 0, sizeof(msg));
  snprintf(msg, sizeof(msg),
    "sizeof(statevars): %d\r\n", sizeof(statevars));
  uwrite_print_buff(msg);

  //spi_init();

  //uwrite_print_buff("Printing before sdcard init!\r\n");

  
  //sdcard_init();

  if (sdcard_is_enabled()) {
    uwrite_print_buff("Tada!\r\n");
  }

  return 0;
}


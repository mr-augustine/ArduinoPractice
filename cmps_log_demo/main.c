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
  statevars.prefix = 0xDADAFEED;
  statevars.suffix = 0xCAFEBABE;

  /*char msg[32];
  memset(msg, 0, sizeof(msg));
  snprintf(msg, sizeof(msg),
    "sizeof(statevars): %d\r\n", sizeof(statevars));
  uwrite_print_buff(msg);*/

  uint32_t iterations = 0;

  while (1) {
    button_update();
    cmps10_update_all();
    statevars.main_loop_counter = iterations;

    sdcard_write_data();

    iterations++;

    if (iterations > 64) {
      break;
    }
  }

  led_turn_off();

  return 0;
}


#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "cmps10.h"
#include "uwrite.h"

int main (void) {
  char msg[64];

  cmps10_init();
  memset(msg, 0, sizeof(msg));

  cmps10_heading = 0;
  cmps10_pitch = 0;
  cmps10_roll = 0;

  UWRITE_init();

  while (1) {
    cmps10_update_all();

    snprintf(msg, sizeof(msg),
      "Heading: %u  Pitch: %d  Roll: %d\r\n",
      cmps10_heading, cmps10_pitch, cmps10_roll);
    UWRITE_print_buff(msg);
  }

  return 0;
}

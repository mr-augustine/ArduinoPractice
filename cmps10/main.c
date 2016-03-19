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

  UWRITE_init();

  while (1) {
    cmps10_update_heading();

    snprintf(msg, sizeof(msg),
      "Heading: %u\r\n", cmps10_heading);
    UWRITE_print_buff(msg);
  }

  return 0;
}

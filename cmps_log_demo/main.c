/*
 *
 */

#include <stdio.h>

#include "cmps10.h"
#include "ledbutton.h"
#include "pins.h"
#include "sd_card.h"
#include "statevars.h"
#include "uwrite.h"

int main(void) {
  UWRITE_init(); 
  button_init();
  spi_init();
  sdcard_init();
  cmps10_init();

  return 0;
}


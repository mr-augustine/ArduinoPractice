/*
 * file:  main.c
 * author: mr-augustine
 * date: 20160313
 *
 * Exercises the ledbutton library. When the button is pressed, the LED
 * is toggled. Regardless of its starting position, the button is treated
 * as being initially unpressed.
 */
#include <stdio.h>
#include "pins.h"
#include "ledbutton.h"

int main(void) {
  button_init();

  while (1) {
    button_update();

    if (button_is_pressed()) {
      led_turn_on();
    } else {
      led_turn_off();
    }
  }

  return 0;
}


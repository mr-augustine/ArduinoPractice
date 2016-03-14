#include <stdio.h>
#include "pins.h"
#include "ledbutton.h"

int main(void) {
  button_init();

  /*if (!button_is_pressed()) {
    led_turn_on();
  }*/

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


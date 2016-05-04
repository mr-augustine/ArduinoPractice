/*
 * file:  main.c
 * author: mr-augustine
 * date: 20160503
 *
 * The file exercises the ledbutton and uwrite components on the Arduino Mega.
 * Whenever the button is pressed, a message is continuously written to the
 * serial port. Whenever the button is unpressed, a '*' is written to the
 * serial port.
 */
#include <stdio.h>
#include "pins.h"
#include "ledbutton.h"
#include "uwrite.h"

int main(void) {
  button_init();
  uwrite_init();

  while (1) {
    button_update();

    if (button_is_pressed()) {
      uwrite_print_buff("The button is pressed! The LED should be on.\r\n");
      led_turn_on();
    } else {
      uwrite_print_buff("*");
      led_turn_off();
    }
  }

  return 0;
}


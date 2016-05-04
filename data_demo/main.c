/*
 * file:  main.c
 * author: mr-augustine
 * date: 20160503
 *
 * The file exercises the ledbutton, cmps10, and uwrite components on the Mega.
 * Whenever the button is pressed, a message is written to the serial port.
 * Continuous updates of the compass are also written to the serial port.
 */
#include <stdio.h>
#include <string.h>
#include "pins.h"
#include "cmps10.h"
#include "ledbutton.h"
#include "statevars.h"
#include "uwrite.h"

statevars_t statevars;

int main(void) {
  char msg[64];

  button_init();
  uwrite_init();
  cmps10_init();

  memset(msg, 0, sizeof(msg));

  while (1) {
    button_update();
    cmps10_update_all();

    if (button_is_pressed()) {
      uwrite_print_buff("The button is pressed! The LED should be on.\r\n");
      led_turn_on();
    } else {
      led_turn_off();
    }

    snprintf(msg, sizeof(msg),
      "Heading: %u  Pitch: %d  Roll: %hhd\r\n",
     //cmps10_heading, cmps10_pitch, cmps10_roll); 
     // Needed to cast these values to display negative pitch and roll values
     (int8_t) statevars.heading_raw, (int8_t) statevars.pitch_deg, (int8_t) statevars.roll_deg); 
    uwrite_print_buff(msg);
  }

  return 0;
}


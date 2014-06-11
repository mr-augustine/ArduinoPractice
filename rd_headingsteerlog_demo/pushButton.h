#ifndef _pushButton_h_
#define _pushButton_h_

#include <avr/interrupt.h>
#include "RobotDevil.h"
#include "globals.h"

////////////////////////////////////////////////////////////////////////////////
// PushButton Constants
#define BUTTON_PIN          (1 << 2)   // PC2 (A2)
#define BUTTON_LED          (1 << 0)   // PB0 ( 8)

////////////////////////////////////////////////////////////////////////////////
// PushButton Variables
volatile uint8_t button_pressed = 0;


////////////////////////////////////////////////////////////////////////////////
// PushButton Functions
void init_pushbutton() {
  button_pressed = 0;

  return;
}

void update_pushbutton() {
  globals.button_press_count = button_pressed;
  
  // if the button was pressed once and the compass reports a valid reading
  // then save this reading as the target heading and turn ON the associated
  // LED
  if (globals.button_press_count == 1 && globals.heading_set == 0 /*&&
      ((globals.status_bits & STATUS_COMPASS_VALID) == STATUS_COMPASS_VALID)*/) {
    globals.target_heading_deg = globals.compass_deg;
    globals.heading_set = 1;
  }
  
  // if the button was pressed a second time, then start the mission and turn
  // ON the associated LED
  if (globals.button_press_count == 2) {
    globals.mission_started = 1;
  }
    
  // turn on the ON_TARGET LED if compass is pointing at the target heading
  if (globals.heading_set == 1 &&
      globals.compass_deg <= globals.target_heading_deg + 2.0 &&
      globals.compass_deg >= globals.target_heading_deg - 2.0) {
  }

  return;
}


#endif

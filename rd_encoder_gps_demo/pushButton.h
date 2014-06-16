#ifndef _pushButton_h_
#define _pushButton_h_

#include <avr/interrupt.h>
#include "RobotDevil.h"
#include "globals.h"

////////////////////////////////////////////////////////////////////////////////
// PushButton Constants
#define BUTTON_PIN          (1 << 2)   // PC2 (A2)

////////////////////////////////////////////////////////////////////////////////
// PushButton Variables
//volatile uint8_t button_pressed = 0;


////////////////////////////////////////////////////////////////////////////////
// PushButton Functions
void init_pushbutton() {
  globals.started = 0;
  globals.start_loop = 0;

  return;
}

void update_pushbutton() {
  if (PINC & BUTTON_PIN) {
    globals.start_button = 0;
  }
  else {
    globals.start_button = 1;
  }
  
  if (globals.started) { return; }
  
  if (globals.start_button == 1) {
    globals.started = 1;
    globals.start_loop = globals.loop_counter;
  }
  
  return;
}


#endif

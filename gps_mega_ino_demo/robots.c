#include "pins.h"
#include "robot.h"
#include "statevars.h"

statevars_t * robot_data;

uint8_t sdcard_init(statevars_t * data, uint8_t chip_select_pin) {
  return 1;
  /*if (!SD.begin(chip_select_pin)) {
    return 0;
  }

  robot_data = data;

  return 1;*/
}

void sdcard_write_data(void) {
  return;
}

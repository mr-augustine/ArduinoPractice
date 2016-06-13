/*
 * file: gps_mega_ino_demo.ino
 * created: 20160609
 * author(s): mr-augustine
 *
 * This file exercises the GPS and SD card libraries. The GPS data are
 * parsed and then stored to the SD card.
 */
#include <SD.h>
#include "statevars.h"

statevars_t statevars;

void setup() {
  //Serial.begin(9600);

  if (!SD.begin(53)) {
    //Serial.println("Uh oh!");
  } else {
    //Serial.println("Good to go!");
  }

  /*if (sdcard_init(&statevars, 53) == 0) {
    Serial.println("There was an error initializing the SD card.");
  } else {
    Serial.println("Ready!");
  }*/
}

void loop() {

}

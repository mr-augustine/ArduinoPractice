/*
 * Defines the pins used on the Arduino Mega.
 *
 * Every physical wire connected to the Arduino is plugged into a pin.
 * And for each occupied pin, we define:
 *   (1) the PORT it is on
 *   (2) the Data Direction Register associated with that PORT
 *   (3) the PORT's Pin Vector
 *   (4) the Pin Vector address for the bit associated with the physical pin
 */
#ifndef _PINS_H_
#define _PINS_H_

#include <avr/io.h>                 // For the pin names (e.g., PB2)

////////////////////////////////////////////////////////////////////////////////
// MOBILITY
#define STEERING_PORT       PORTE
#define STEERING_DDR        DDRE
#define STEERING_PINVEC     PINE
#define STEERING_PIN        PE4     // Mega Digital Pin 2

#define DRIVE_PORT          PORTE
#define DRIVE_DDR           DDRE
#define DRIVE_PINVEC        PINE
#define DRIVE_PIN           PE5     // Mega Digital Pin 3

#endif


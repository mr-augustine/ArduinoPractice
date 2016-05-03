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
// ILLUMINATED PUSHBUTTON
#define BUTTON_PORT         PORTF
#define BUTTON_DDR          DDRF
#define BUTTON_PINVEC       PINF
#define BUTTON_PIN          PF0     // Mega Analog Pin 0

#define BUTTON_LED_PORT     PORTH
#define BUTTON_LED_DDR      DDRH
#define BUTTON_LED_PINVEC   PINH
#define BUTTON_LED_PIN      PH6     // Mega Digital Pin 9

#endif /* _PINS_H_ */


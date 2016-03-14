/*
 * Defines the pins used on the Arduino.
 *
 * Every physical wire connected to the Arduino is plugged into a pin.
 * And for each occupied pin, we define:
 *   (1) the PORT it is on
 *   (2) the Data Direction Register associated with that PORT
 *   (3) the PORT's Pin Vector
 *   (4) the Pin Vector address for the bit associated with the physical pin
 */
#ifndef _PIN_DEFINES_H_
#define _PIN_DEFINES_H_

#include <avr/io.h>                 // For the pin names (e.g., PB2)

////////////////////////////////////////////////////////////////////////////////
// ILLUMINATED PUSHBUTTON
#define BUTTON_PORT         PORTC
#define BUTTON_DDR          DDRC
#define BUTTON_PINVEC       PINC
#define BUTTON_PIN          PC2     // Analog Pin 2

#define BUTTON_LED_PORT     PORTB
#define BUTTON_LED_DDR      DDRB
#define BUTTON_LED_PINVEC   PINB
#define BUTTON_LED_PIN      PB1     // Digital Pin 9

#endif /* _PIN_DEFINES_H_ */


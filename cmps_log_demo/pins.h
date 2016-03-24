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
// COMPASS (CMPS10)
#define COMPASS_PORT        PORTC
#define COMPASS_DDR         DDRC
#define COMPASS_PINVEC      PINC
#define COMPASS_SDA_PIN     PC4     // Analog Pin 4
#define COMPASS_SCL_PIN     PC5     // Analog Pin 5
 
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

////////////////////////////////////////////////////////////////////////////////
// SD Card
#define SPI_CS_PORT     PORTB
#define SPI_CS_DDR      DDRB
#define SPI_CS_PIN      PINB
#define SPI_CS          PB2     /* Digital Pin 10 */

#define SPI_MOSI_PORT   PORTB
#define SPI_MOSI_DDR    DDRB
#define SPI_MOSI_PIN    PINB
#define SPI_MOSI        PB3     /* Digital Pin 11 */

#define SPI_MISO_PORT   PORTB
#define SPI_MISO_DDR    DDRB
#define SPI_MISO_PIN    PINB
#define SPI_MISO        PB4     /* Digital Pin 12 */

#define SPI_SCK_PORT    PORTB
#define SPI_SCK_DDR     DDRB
#define SPI_SCK_PIN     PINB
#define SPI_SCK         PB5     /* Digital Pin 13 */


#endif /* _PIN_DEFINES_H_ */


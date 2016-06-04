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
// SD Card
#define SPI_CS_PORT         PORTB
#define SPI_CS_DDR          DDRB
#define SPI_CS_PIN          PINB
#define SPI_CS              PB0    // Mega Digital Pin 53

#define SPI_MOSI_PORT       PORTB
#define SPI_MOSI_DDR        DDRB
#define SPI_MOSI_PIN        PINB
#define SPI_MOSI            PB2    // Mega Digital Pin 51

#define SPI_MISO_PORT       PORTB
#define SPI_MISO_DDR        DDRB
#define SPI_MISO_PIN        PINB
#define SPI_MISO            PB3    // Mega Digital Pin 50

#define SPI_SCK_PORT        PORTB
#define SPI_SCK_DDR         DDRB
#define SPI_SCK_PIN         PINB
#define SPI_SCK             PB1    // Mega Digital Pin 52

////////////////////////////////////////////////////////////////////////////////
// USART WRITE
// Keep these pins unoccupied       // Mega Digital Pin 0
                                    // Mega Digital Pin 1

#endif /* _PINS_H_ */


/*
  Defines the pins for the sd_card_logger.
  
  Every physical wire connected to the Arduino is plugged into a pin.
  And for each occupied pin, we define:
    (1) the PORT it is on
    (2) the Data Direction Register associated with that PORT
    (3) the PORT's Pin Vector
    (4) the Pin Vector address for the bit associated with the physical pin

*/
#ifndef _PIN_DEFINES_H_
#define _PIN_DEFINES_H_

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
#define SPI_SCK         PB13    /* Digital Pin 13 */


#endif /* _PIN_DEFINES_H_ */

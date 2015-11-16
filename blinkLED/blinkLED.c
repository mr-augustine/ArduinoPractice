                                                       /* Blinker Demo */
     /* This source code is a modified version of the blinkLED program */
                /* given in "Make: AVR Programming" by Elliot Williams */

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */


int main(void) {

  // -------- Inits --------- //
  // We're using the onboard LED on the Arduino UNO
  DDRB |= 0b00100000;            /* Data Direction Register B:
                                   writing a one to the bit
                                   enables output. */

  // ------ Event loop ------ //
  while (1) {

    PORTB = 0b00100000;          /* Turn on first LED bit/pin in PORTB */
    _delay_ms(1000);                                           /* wait */

    PORTB = 0b00000000;          /* Turn off all B pins, including LED */
    _delay_ms(1000);                                           /* wait */

  }                                                  /* End event loop */
  return (0);                            /* This line is never reached */
}

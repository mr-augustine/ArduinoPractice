#include <stdio.h>
#include <avr/io.h>
#include "onboard_logger.h"

int main(void) {
    int some_value = 9;

    // Enable the onboard LED
    DDRB |= 0b00100000;

    init_onboard_logger(&some_value, 2);

    // Turn on the LED if logging initialized successfully,
    // otherwise turn off the LED
    if (onboard_logger_enabled == 1) {
        PORTB = 0b00100000;
    } else {
        PORTB = 0b00000000;
    }

    return 0;
}

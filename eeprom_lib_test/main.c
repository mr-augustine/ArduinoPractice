#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include "onboard_logger.h"

int main(void) {
    //int some_value = 9;

    typedef struct {
        uint32_t unsigned_long;
        uint16_t unsigned_short;
        uint8_t  unsigned_byte;
        char     sentence[84];
        float    float_value;
        double   double_value;
        int8_t   signed_byte;
        int16_t  signed_short;
        int32_t  signed_long;
    } my_data_t;

    // Enable the onboard LED
    DDRB |= 0b00100000;

    my_data_t mydata;
    memset(&mydata, 0, sizeof(my_data_t));

    mydata.unsigned_long = 8675309;
    mydata.unsigned_short = 3560;
    mydata.unsigned_byte = 2;
    strcpy(mydata.sentence, "This is just a test. Nothing interesting to see here.\0");
    mydata.float_value = 500;
    mydata.double_value = 500;
    mydata.signed_byte = -2;
    mydata.signed_short = -3560;
    mydata.signed_long = -8675309;

    init_onboard_logger(&mydata, sizeof(my_data_t));
    write_next_frame();

    // Turn on the LED if logging initialized successfully,
    // otherwise turn off the LED
    if (onboard_logger_enabled == 1) {
        PORTB = 0b00100000;
    } else {
        PORTB = 0b00000000;
    }

    return 0;
}

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include "onboard_logger.h"

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

void init_struct(my_data_t * data);

int main(void) {
    //int some_value = 9;


    // Enable the onboard LED
    DDRB |= 0b00100000;

    my_data_t mydata;
    memset(&mydata, 0, sizeof(my_data_t));

    init_struct(&mydata);
    init_onboard_logger(&mydata, sizeof(my_data_t));

    // For a 114-byte frame and a 512-byte chunk we shoud have one full
    // chunk (with padding) and an adjacent chunk with one frame in it
    int write_index;
    int num_iterations = 5;  

    // Update the unsigned_byte value with the loop index each time a
    // frame is written
    for (write_index = 0; write_index < num_iterations; write_index++) {
        mydata.unsigned_byte = write_index;
        write_next_frame();
    }

    // Turn on the LED if logging initialized successfully,
    // otherwise turn off the LED
    if (onboard_logger_enabled == 1) {
        PORTB = 0b00100000;
    } else {
        PORTB = 0b00000000;
    }

    return 0;
}

void init_struct(my_data_t * data) {
    data->unsigned_long = 8675309;
    data->unsigned_short = 3560;
    data->unsigned_byte = 2;
    strcpy(data->sentence, "This is just a test. Nothing interesting to see here.\0");
    data->float_value = 500;
    data->double_value = 500;
    data->signed_byte = -2;
    data->signed_short = -3560;
    data->signed_long = -8675309;

    return;
}

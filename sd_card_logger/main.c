#include <stdio.h>
#include <string.h>
#include "sd_card.h"
#include "statevars.h"
#include "uwrite.h"

void init_statevars(statevars_t * vars);

statevars_t statevars;

int main(void) {
    memset(&statevars, 0, sizeof(statevars));

    UWRITE_init();

    init_statevars(&statevars);

    UWRITE_print_buff("---Start---\r\n");
    SPI_init();

    //uint8_t spi_register = SPCR;
    //UWRITE_print_byte(&spi_register);

    SDCARD_init();
    SDCARD_write_data();

    // Turn on DEBUG LED
    PORTD |= (1 << 4);
    // Turn on the LED if logging initialized successfully,
    // otherwise turn off the LED
    /*if (SPCR == 0b01010011) {
        PORTD |= (1 << 4);
    } else {
        PORTD = 0b00000000;
    }*/

    return 0;
}

void init_statevars(statevars_t * vars) {
    vars->prefix = 0xDADAFEEDL;
    vars->unsigned_long = 8675309;
    vars->unsigned_short = 3560;
    vars->unsigned_byte = 2;
    snprintf(vars->sentence, sizeof(vars->sentence),
        "This is just a test. Nothing interesting to see here.");
    vars->float_value = 500.0;
    vars->double_value = 500.0;
    vars->signed_byte = -2;
    vars->signed_short = -3560;
    vars->signed_long = -8675309;
    vars->suffix = 0xCAFEBABEL;

    return;
}


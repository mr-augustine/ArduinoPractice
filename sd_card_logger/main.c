#include <stdio.h>
#include <string.h>
#include "sd_card.h"
#include "statevars.h"
#include "uwrite.h"

void init_statevars(statevars_t * vars);
void print_block(uint8_t * block);

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
    //SDCARD_write_data();

    char block_buff[SDCARD_BYTES_PER_BLOCK + 1]; 
    memset(block_buff, 0, sizeof(block_buff));
    SDCARD_read_block(0, block_buff);

    UWRITE_print_buff("print_buff\r\n");
    print_block((uint8_t *)block_buff);
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

void print_block(uint8_t * b) {
    char msg[45];
    memset(msg, 0, sizeof(msg));

    uint16_t i;
    for (i = 0; i < 512/8; i++) {
        snprintf(msg, sizeof(msg),
            "0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \r\n",
            b[8*i], b[8*i+1], b[8*i+2], b[8*i+3],
            b[8*i+4], b[8*i+5], b[8*i+6], b[8*i+7]);
        UWRITE_print_buff(msg);
    }

    return;
}


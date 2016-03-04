#include "sd_card.h"
#include "uwrite.h"

int main(void) {
    UWRITE_init();

    UWRITE_print_buff("---Start---\r\n");
    SPI_init();

    //uint8_t spi_register = SPCR;
    //UWRITE_print_byte(&spi_register);

    init_sd_card();
    // Turn on the LED if logging initialized successfully,
    // otherwise turn off the LED
    /*if (SPCR == 0b01010011) {
        PORTD |= (1 << 4);
    } else {
        PORTD = 0b00000000;
    }*/

    return 0;
}

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>

#include "uwrite.h"

#define RCIE0   7
#define RXEN0   4
#define TXEN0   3
#define UCSZ01  2
#define UCSZ00  1
#define UDRE0   5

#define TX_REG_NOT_READY() (!(UCSR0A & (1 << UDRE0)))

static uint8_t uwrite_initialized;
static char buffer[BUFF_SIZE];

void uwrite_init(void) {
    // Disable interrupts before configuring USART
    cli();

    // Unset the flags, double speed, and comm mode bits
    UCSR0A = 0;

    // Enable receive interrupt, receiving and transmission
    UCSR0B = 0;
    UCSR0B = (1 << RCIE0) | (1 << RXEN0) | (1 << TXEN0);

    // Enable 8-bit character size
    // Asynchronous USART, no partity, 1 stop bit already set (default)
    UCSR0C = 0;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Set baud rate to 115200
    // f_osc / (UBRRn + 1) == 115200
    // See Table 20.7 in the Atmel specs
    UBRR0H = 0;
    UBRR0L = 8;

    // Re-enable interrupts after USART configuration is complete
    sei();

    uwrite_initialized = 1;

    return;
}

// Prints a null-terminated character buffer to the USART port
void uwrite_print_buff(char * character) {
    if (uwrite_initialized) {

        while (*character != 0) {
            // Wait until the transmit data register is ready
            while TX_REG_NOT_READY() {;}

            UDR0 = *character;
            character++;
        }
    }

    return; 
}

// Prints a byte to the USART port as a hex value
void uwrite_print_byte(void * a_byte) {
    if (uwrite_initialized) {
        char * char_ptr = buffer;

        snprintf(buffer, BUFF_SIZE, "0x%02X\r\n", *((char *) a_byte));
        
        while (*char_ptr != 0) {
            while TX_REG_NOT_READY() {;}

            UDR0 = *char_ptr;
            char_ptr++;
        }
    }

    return;
}

// Prints a short to the USART port as a hex value
void uwrite_print_short(void * a_short) {
    if (uwrite_initialized) {
        char * char_ptr = buffer;

        snprintf(buffer, BUFF_SIZE, "0x%02X\r\n", *((uint16_t *) a_short));

        while (*char_ptr != 0) {
            while TX_REG_NOT_READY() {;}

            UDR0 = *char_ptr;
            char_ptr++;
        }
    }

    return;
}

// Prints a long to the USART port as a hex value
void uwrite_print_long(void * a_long) {
    if (uwrite_initialized) {
        char * char_ptr = buffer;

        snprintf(buffer, BUFF_SIZE, "0x%02lX\r\n", *((uint32_t *) a_long));

        while (*char_ptr != 0) {
            while TX_REG_NOT_READY() {;}

            UDR0 = *char_ptr;
            char_ptr++;
        }
    }

    return;
}

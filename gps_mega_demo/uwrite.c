#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>

#include "uwrite.h"

#define TX_REG_NOT_READY() (!(UCSR0A & (1 << UDRE0)))

static uint8_t uwrite_initialized;
static char buffer[BUFF_SIZE];

// TODO: Verify that the registers are set the way you expect them to be
// in case some other library decides to change them.
/* Configures the hardware to enable USART transmission and a baud rate
 * of 115200 bps
 */
uint8_t uwrite_init(void) {
    // Disable interrupts before configuring USART
    cli();

    // Enable transmitting
    UCSR0B = (1 << TXEN0);

    // 8-bit character size, asynchronous USART, no partity,
    // 1 stop bit already set by default in UCSR0C

    // Set baud rate to 115200
    // f_osc / (UBRRn + 1) == 115200
    // See Table 20.7 in the Atmel specs
    UBRR0H = 0;
    UBRR0L = 8;

    // Re-enable interrupts after USART configuration is complete
    sei();

    uwrite_initialized = 1;

    return uwrite_initialized;
}

/*
 * Prints a character buffer to the USART port.
 * Assumes the character buffer is null-terminated.
 *
 * char_buff: a null-terminated character buffer
 */
void uwrite_print_buff(char * char_buff) {
    if (uwrite_initialized) {

        while (*char_buff != 0) {
            // Wait until the transmit data register is ready
            while TX_REG_NOT_READY() {;}

            UDR0 = *char_buff;
            char_buff++;
        }
    }

    return; 
}

/*
 * Prints a byte to the USART port as a hex value with a leading '0x'
 * followed by a carriage return and newline.
 *
 * a_byte: a pointer to a byte value
 */
void uwrite_println_byte(void * a_byte) {
    if (uwrite_initialized) {
        char * char_ptr = buffer;

        snprintf(buffer, BUFF_SIZE, "0x%02X\r\n", *((unsigned char *) a_byte));
        
        while (*char_ptr != 0) {
            while TX_REG_NOT_READY() {;}

            UDR0 = *char_ptr;
            char_ptr++;
        }
    }

    return;
}

/*
 * Prints a short to the USART port as a hex value with a leading '0x'
 * followed by a carriage return and newline.
 *
 * a_short: a pointer to a short value
 */
void uwrite_println_short(void * a_short) {
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

/*
 * Prints a long to the USART port as a hex value with a leading '0x'
 * followed by a carriage return and newline.
 *
 * a_long: a pointer to a long value
 */
void uwrite_println_long(void * a_long) {
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

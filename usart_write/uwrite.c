#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>

#define RCIE0   7
#define RXEN0   4
#define TXEN0   3
#define UCSZ01  2
#define UCSZ00  1
#define UDRE0   5

#define TX_REG_NOT_READY (!(UCSR0A & (1 << UDRE0)))

static uint8_t initialized;

void UWRITE_init(void) {
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

    initialized = 1;

    return;
}

// Prints a null-terminated character buffer to the USART port
void UWRITE_print_buff(char * character) {
    if (initialized) {
        while (*character != 0) {
            // Wait until the transmit data register is ready
            while TX_REG_NOT_READY {;}

            UDR0 = *character;
            character++;
        }
    }

    return;
}

// Prints a byte to the USART port
void UWRITE_print_byte(void * a_byte) {
    if (initialized) {
        while TX_REG_NOT_READY {;}

        UDR0 = *((uint8_t *)a_byte);
    }

    return;
}

// Prints a short to the USART port
void UWRITE_print_short(void * a_short) {
    if (initialized) {
        void * short_ptr = a_short;
        int i;

        // TODO add the option to print byte values as Hex, Bin, or Dec
        for (i = 0; i < 2; i++) {
            UWRITE_print_byte(short_ptr);
            short_ptr = short_ptr + 1;
        }
    }

    return;
}

/*void UWRITE_print_long(void * a_long) {
    return;
}*/

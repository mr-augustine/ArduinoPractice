#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define RCIE0 7
#define RXEN0 4
#define TXEN0 3
#define UCSZ01 2
#define UCSZ00 1
#define UDRE0 5

// RBIT == Register Bit
// Shortcut for grabbing a bit from a register
#define RBIT(REGISTER, BIT) (REGISTER & (1 << BIT))

void init_usart(void);
void usart_write(char * character);

int main(void) {
    char greeting[15] = "Hello, world!\n\0";
    char message[128];

    // Disable interrupts before configuring USART
    cli();

    init_usart();

    // Re-enable interrupts after USART configuration is complete
    sei();

    usart_write(greeting);

    snprintf(message, 128,
        "PORTB: %02X %02X %02X %02X %02X %02X %02X %02X\n",
        RBIT(PORTB, 7), RBIT(PORTB, 6), RBIT(PORTB, 5), RBIT(PORTB, 4),
        RBIT(PORTB, 3), RBIT(PORTB, 2), RBIT(PORTB, 1), RBIT(PORTB, 0));
    usart_write(message);

    return 0;
}

void init_usart(void) {
    // Unset the flags, double speed, and comm mode bits
    UCSR0A = 0;

    // Enable receive interrupt, receiving and transmission
    UCSR0B = 0;
    UCSR0B = (1 << RCIE0) | (1 << RXEN0) | (1 << TXEN0);

    // Enable 8-bit character size
    // Asynchronous USART, no parity, 1 stop bit already set (default)
    UCSR0C = 0;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Set baud rate to 115200
    // f_osc / (UBRRn + 1) = 115200
    // See Table 20.7 in the Atmel specs
    UBRR0H = 0;
    UBRR0L = 8;

    return;
}

void usart_write(char * character) {
    while (*character != 0) {
        // Wait until the transmit data register is ready 
        while (!(UCSR0A & (1 << UDRE0))) {;}

        UDR0 = *character;
        character++;
    }

    return;
}



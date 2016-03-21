#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gps.h"
#include "statevars.h"

typedef struct {
  uint8_t ready;
  char sentence[GPS_SENTENCE_BUFF_SZ];
} gps_buffer_t;

static int8_t buffer_index;
static uint8_t sentence_index;

static gps_buffer_t gps_buffers[NUM_GPS_SENTENCE_BUFFS];

static volatile uint8_t gps_no_avail_buff = 0;
static volatile uint8_t gps_buff_overflow = 0;
static volatile uint8_t gps_unexpected_start = 0;

ISR (USART_RX_vect) {
  char new_char = UDR0;

  // If a gps buffer hasn't been identified to be filled,
  // look for the first available buffer to start writing to
  if (buffer_index == -1) {
    uint8_t i;

    for (i = 0; i < NUM_GPS_SENTENCE_BUFFS; i++) {
      if (gps_buffers[i].ready == 0) {
        buffer_index = i;
        sentence_index = 0;
        break;
      }
    }

    // No available buffers were found
    if (i == NUM_GPS_SENTENCE_BUFFS) {
      gps_no_avail_buff = 1;
      return;
    }
  }

  // If we received a sentence_start character while in the middle
  // of populating a buffer, mark this as unexpected and prepare
  // to overwrite the current buffer starting at the beginning
  if (new_char == GPS_SENTENCE_START && sentence_index != 0) {
    gps_unexpected_start = 1;
    buffer_index = 0;
  }

  // If we received a data character or and unexpected start or
  // a legitimate start, then add the character to the buffer
  // and verify that enough room remains in the buffer
  if (new_char != GPS_SENTENCE_END) {
    gps_buffers[buffer_index].sentence[sentence_index] = new_char; 
    sentence_index = sentence_index + 1;

    // Check whether the buffer has enough room for the newline and
    // null char. If there isn't enough room, roll back the index
    if (sentence_index == GPS_SENTENCE_BUFF_SZ - 1) {
      buffer_index = buffer_index - 1;
      gps_buff_overflow = 1;
    }

    return;
  }

  // We received a newline character, so terminate the current sentence buffer
  gps_buffers[buffer_index].sentence[sentence_index++] = new_char;
  gps_buffers[buffer_index].sentence[sentence_index] = '\0';
  gps_buffers[buffer_index].ready = 1;

  buffer_index = -1;
}

void gps_init(void) {
  buffer_index = -1;
  sentence_index = 0;

  // Disable interrupts before configuring USART
  cli();

  // Unset the flags, double speed, and comm mode bits
  UCSR0A = 0;

  // Enable receive interrupt, receiving and trasmission
  UCSR0B = 0;
  UCSR0B = (1 << RCIE0) | (1 << RXEN0) | (1 << TXEN0);

  // Enable 8-bit character size
  // Asynchronous USART, no parity, 1 stop bit already set (default)
  UCSR0C = 0;
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  // Set baud rate to 115200
  // f_osc / (UBRRn + 1) == 115200
  // See Table 20.7 in the Atmel specs
  UBRR0H = 0;
  UBRR0L = 8;

  // Re-enable interrupts after USART configuration is complete
  sei();

  return;
}

void gps_update(void) {

  // TODO: memset the statevars that will hold the sentences

  // TODO: initialize gps statevars to zero

  if (gps_no_avail_buff == 1) {
    // TODO: update statevars
    gps_no_avail_buff = 0;
  }

  if (gps_buff_overflow == 1) {
    // TODO: update statevars
    gps_buff_overflow = 0;
  }

  if (gps_unexpected_start == 1) {
    // TODO: update statevars
    gps_unexpected_start = 0;
  }

  if (gps_buffers[0].ready == 1) {

  }

}



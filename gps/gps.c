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

static volatile uint8_t gps_no_buff_avail = 0;
static volatile uint8_t gps_buff_overflow = 0;
static volatile uint8_t gps_unexpected_start = 0;

static uint8_t hexchar_to_dec(char c);
static uint8_t validate_checksum(char * s);

/* Interrupt Service Routine that triggers whenever a new character
 * is received from the GPS sensor. Adds the new char to a buffer
 * such that all chars from the same sentence are saved to the same
 * buffer. Each new sentence is saved to the first available buffer.
 */
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
      gps_no_buff_avail = 1;
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
  if (new_char != GPS_SENTENCE_END) {
    gps_buffers[buffer_index].sentence[sentence_index] = new_char; 
    sentence_index = sentence_index + 1;

    // Verify that the buffer has enough room for the carriage return,
    // newline, and null chars. If there isn't enough room, rollback the index.
    if (sentence_index == GPS_SENTENCE_BUFF_SZ - 2) {
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

/* Initializes the GPS by enabling USART RX, setting the baud rate to 115200,
 * and resetting the buffer indexes.
 */
void gps_init(void) {
  buffer_index = -1;
  sentence_index = 0;

  // Disable interrupts before configuring USART
  cli();

  // Unset the flags, double speed, and comm mode bits
  UCSR0A = 0;

  // Enable receive interrupt, receiving and trasmission
  // TODO: Do we really need the ability to transmit?
  UCSR0B = 0;
  UCSR0B = (1 << RCIE0) | (1 << RXEN0);
  //UCSR0B = (1 << RCIE0) | (1 << RXEN0) | (1 << TXEN0);

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

/* Orchestrates the GPS data parsing and error messaging */
void gps_update(void) {
  // No need for redeclaration of buffer_index
  //uint8_t buffer_index;

  // Reset the sentence buffers
  for (buffer_index = 0; buffer_index < NUM_GPS_SENTENCE_BUFFS; buffer_index++) {
    memset(gps_buffers[buffer_index], '\0', sizeof(gps_buffer_t));
  }

  initialize_gps_statevars();

  if (gps_no_buff_avail == 1) {
    statevars.status |= STATUS_GPS_NO_BUFF_AVAIL;
    gps_no_buff_avail = 0;
  }

  if (gps_buff_overflow == 1) {
    statevars.status |= STATUS_GPS_BUFF_OVERFLOW;
    gps_buff_overflow = 0;
  }

  if (gps_unexpected_start == 1) {
    statevars.status |= STATUS_GPS_UNEXPECT_START;
    gps_unexpected_start = 0;
  }

  if (gps_buffers[0].ready == 1) {
    memcpy(statevars.gps_sentence0, gps_buffers[0], GPS_SENTENCE_BUFF_SZ); 
    parse_gps_sentence(statevars.gps_sentence0);
    gps_buffers[0].ready = 0;
  }

  if (gps_buffers[1].ready == 1) {
    memcpy(statevars.gps_sentence1, gps_buffers[1], GPS_SENTENCE_BUFF_SZ); 
    parse_gps_sentence(statevars.gps_sentence1);
    gps_buffers[1].ready = 0;
  }

  if (gps_buffers[2].ready == 1) {
    memcpy(statevars.gps_sentence2, gps_buffers[2], GPS_SENTENCE_BUFF_SZ); 
    parse_gps_sentence(statevars.gps_sentence2);
    gps_buffers[2].ready = 0;
  }

  if (gps_buffers[3].ready == 1) {
    memcpy(statevars.gps_sentence3, gps_buffers[3], GPS_SENTENCE_BUFF_SZ); 
    parse_gps_sentence(statevars.gps_sentence3);
    gps_buffers[3].ready = 0;
  }

  return;
}


/* Resets all GPS-related statevars to zero. */
static void initialize_gps_statevars() {
  memset(statevars.gps_sentence0, '\0', GPS_SENTENCE_LENGTH);
  memset(statevars.gps_sentence1, '\0', GPS_SENTENCE_LENGTH);
  memset(statevars.gps_sentence2, '\0', GPS_SENTENCE_LENGTH);
  memset(statevars.gps_sentence3, '\0', GPS_SENTENCE_LENGTH);
  statevars.gps_latitude = 0.0;
  statevars.gps_longitude = 0.0;
  statevars.gps_hdop = 0.0;
  statevars.gps_altitude_m = 0.0;
  statevars.gps_mag_hdg_deg = 0.0;
  statevars.gps_speed_kmph = 0.0;
  statevars.gps_hours = 0;
  statevars.gps_minutes = 0;
  statevars.gps_seconds = 0.0;
  memset(statevars.date, 0, GPS_DATE_WIDTH);
  statevars.gps_satcount = 0;

  return;
}

/* Parses the specified GPGGA sentence and saves the values of interest
 * to the statevars variable
 *
 * Returns 0 if successful; 1 otherwise
 */
static uint8_t parse_gpgga(char * s) {
  char field_buf[GPS_FIELD_BUFF_SZ];
  memset(field_buf, '\0', GPS_FIELD_BUFF_SZ);

  // $GPGGA header - ignore
  s = strtok(s, ",");

  // UTC Time
  s = strtok(NULL, ",");
  strncpy(field_buf, s, 2);
  statevars.gps_hours = atoi(field_buf);

  memset(field_buf, '\0', GPS_FIELD_BUFF_SZ);
  strncpy(field_buf, s+2, 2);
  statevars.gps_minutes = atoi(field_buf);

  memset(field_buf, '\0', GPS_FIELD_BUFF_SZ);
  strncpy(field_buf, s+4, 6);
  statevars.gps_seconds = atof(field_buf);

  // Latitude
  s = strtok(NULL, ",");
  memset(field_buf, '\0', GPS_FIELD_BUFF_SZ);
  strncpy(field_buf, s, 2);
  uint8_t lat_degrees = atoi(field_buf);

  memset(field_buf, '\0', GPS_FIELD_BUF_SZ);
  strncpy(field_buf, s+2, 7);
  float lat_minutes = atof(field_buf);

  // Latitude Hemisphere
  s = strtok(NULL, ",");
  uint8_t lat_is_south;
  if (*s == 'N') {
    lat_is_north = 0;
  } else if (*s == 'S') {
    lat_is_north = 1;
  } else {
    return 1;
  }

  // Longitude
  s = strtok(NULL, ",");
  memset(field_buf, '\0', GPS_FIELD_BUFF_SZ);
  strncpy(field_buf, s, 3);
  uint8_t long_degrees = atoi(field_buf);

  memset(field_buf, '\0', GPS_FIELD_BUFF_SZ);
  strncpy(field_buf, s+3, 7);
  float long_minutes = atof(field_buf);

  // Longitude Hemisphere
  s = strtok(NULL, ",");
  uint8_t long_is_west;
  if (*s == 'W') {
    long_is_west = 1;
  } else if (*s == 'E') {
    long_is_west = 0;
  } else {
    return 1;
  }

  float latitude = lat_degrees + (lat_minutes / 60.0);
  if (lat_is_south) {
    latitude = -latitude;
  }

  float longitude = long_degrees + (long_minutes / 60.0);
  if (long_is_west) {
    longitude = -longitude;
  }

  statevars.gps_latitude = latitude;
  statevars.gps_longitude = longitude;

  // Position (Fix) Indicator
  s = strtok(NULL, ",");
  // If there is no fix
  if (*s == '0') {
    return 1;
  }

  // Satellite Count
  s = strtok(NULL, ",");
  statevars.gps_satcount = atoi(s);

  // Horizontal Dilution of Precision (HDOP)
  s = strtok(NULL, ",");
  statevars.gps_hdop = atof(s);

  // Altitude
  s = strtok(NULL, ",");
  statevars.gps_altitude = atof(s);

  return 0;
}

/* Parses the specified NMEA sentence and saves the values of interest
 * to the statevars variable
 */
static void parse_gps_sentence(char * sentence) {
  if (strncmp(sentence, GPGGA_START, START_LENGTH) == 0) {
    parse_gpgga(sentence);
    statevars.status |= STATUS_GPS_GPGGA_RCVD;
  } else if (strncmp(sentence, GPVTG_START, START_LENGTH) == 0) {
    statevars.status |= STATUS_GPS_GPVTG_RCVD;
  } else if (strncmp(sentence, GPRMC_START, START_LENGTH) == 0) {
    statevars.status |= STATUS_GPS_GPRMC_RCVD;
  } else {
    // We don't care about the GPGSA and GPGSV sentences
    return;
  } 

  return;
}

/* Returns the decimal value of the specified char if the char is a valid
 * hexadecimal char; returns an error byte if the specified char is invalid.
 */
static uint8_t hexchar_to_dec(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  }

  return GPS_INVALID_HEX_CHAR;
}

/* Validates the checksum of the specified NMEA sentence
 * Returns 1 if calculated checksum matched the received checksum;
 * Returns 0 otherwise
 */
static uint8_t validate_checksum(char * s) {
  uint8_t checksum = 0;
  uint8_t s_cursor;

  // Calculate the checksum of the received characters in the sentence.
  // Note: only the characters between '$' and '*' are used
  for (s_cursor = 1; s_cursor < GPS_SENTENCE_BUFF_SZ; s_cursor++) {
    if (*(s + s_cursor) != '*') {
      checksum ^= *(s + s_cursor);
    } else {
      break;
    }
  }

  // Advance the cursor to the position of the checksum's first char
  s_cursor++;

  // Verify that there was no overflow by checking whether the cursor
  // went beyond the buffer limit, possibly due to not finding a '*'
  if (s_cursor + 2 >= GPS_SENTENCE_BUFF_SZ) {
    return 0;
  }

  // Convert the most significant and least significant nibbles of
  // the checksum byte
  uint8_t chk_upper = hexchar_to_dec(s[s_cursor]);
  uint8_t chk_lower = hexchar_to_dec(s[s_cursor + 1]);

  // Verify no error occurred during the conversion
  if (chk_upper == GPS_INVALID_HEX_CHAR || chk_lower == GPS_INVALID_HEX_CHAR) {
    return 0;
  }

  // Assemble the expected checksum provided by the GPS receiver
  uint8_t expected_checksum = (chk_upper << 4) | chk_lower;

  if (checksum == expected_checksum) {
    return 1;
  }

  return 0;
}


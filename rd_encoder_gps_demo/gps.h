#ifndef _gps_h_
#define _gps_h_

#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "RobotDevil.h"

////////////////////////////////////////////////////////////////////////////////
// GPS Constants
#define SERIAL_BUFFER_SIZE 84
#define NUM_SERIAL_BUFFERS 3

////////////////////////////////////////////////////////////////////////////////
// GPS Variables

// Serial buffers
typedef struct {
  uint8_t ready;
  char data[SERIAL_BUFFER_SIZE];
} rx_buffer_t;

rx_buffer_t rx_buffers[NUM_SERIAL_BUFFERS];
int8_t active_buffer;
uint8_t active_buffer_index;

volatile uint8_t serial_no_free_buffer = 0;
volatile uint8_t serial_buffer_overflow = 0;
volatile uint8_t serial_unexpected_start = 0;


////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routine                                    // GPS MESSAGE
ISR(USART_RX_vect)
{
  char ch = UDR0;

  // Do we need to select a new buffer?
  if (active_buffer == -1)
  {
    uint8_t i = 0;
    for (i = 0; i < NUM_SERIAL_BUFFERS; i++)
    {
      if (rx_buffers[i].ready == 0)
      {
        active_buffer = i;
        active_buffer_index = 0;
        break;
      }
    }
    if (i == NUM_SERIAL_BUFFERS)
    {
      serial_no_free_buffer = 1;
      return;
    }
  }

  if (ch == '$' && active_buffer_index != 0)
  {
    serial_unexpected_start = 1;
    active_buffer_index = 0;
  }
  
  if (ch != '\n')
  {
    rx_buffers[active_buffer].data[active_buffer_index] = ch;
    active_buffer_index += 1;
    // Make sure there is enough data remaining for a newline
    // character and NULL terminator.
    if (active_buffer_index == SERIAL_BUFFER_SIZE-2)
    {
      active_buffer_index -= 1;
      serial_buffer_overflow = 1;
    }
    return;
  }
  


  // Newline character received.  Time to terminate the string, mark 
  // the buffer as ready, and find the next free buffer.  We wait to
  // find the new buffer when the next character is received so that
  // we can handle the no free buffer case properly.

  // When we receive other characters, we always make sure there is
  // enough room for the newline and null character, so we don't have
  // to check here.
  rx_buffers[active_buffer].data[active_buffer_index++] = ch;
  rx_buffers[active_buffer].data[active_buffer_index++] = '\0';
  rx_buffers[active_buffer].ready = 1;
  active_buffer = -1;
}

////////////////////////////////////////////////////////////////////////////////
// GPS Functions
void serial_write(char *cmd)
{
#define SERIAL_OUTPUT 1
#if SERIAL_OUTPUT
  while (*cmd != 0)
  {
    while(!(UCSR0A & 0b00100000));
    UDR0 = *cmd;
    cmd++;
  }
#endif 
}


uint8_t chr_to_hex(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  else
    return 0xFF;
}


uint8_t check_nmea_checksum(char *string)
{
  uint8_t checksum = 0;
  uint8_t i;
  for (i = 1; i < SERIAL_BUFFER_SIZE; ++i)
  {
    if (string[i] == '*')
      break;
    checksum ^= (uint8_t)string[i];
  }

  i += 1;

  if (i+2 >= SERIAL_BUFFER_SIZE)
    return 0;

  uint8_t c1 = chr_to_hex(string[i]);
  uint8_t c2 = chr_to_hex(string[i+1]);

  if (c1 == 0xFF || c2 == 0xFF)
    return 0;

  uint8_t expect_checksum = ((c1 << 4) | (c2));

  if (checksum != expect_checksum)
    return 0;

  return 1;
}


void update_local_xy()
{
  double rlat = globals.gga_latitude * DEG_TO_RAD;
  double rlon = globals.gga_longitude * DEG_TO_RAD;
  double dLat = (rlat - GPS_REF_LATITUDE_RAD) * GPS_RADIUS_LATITUDE;
  double dLon = (rlon - GPS_REF_LONGITUDE_RAD) * GPS_RADIUS_LONGITUDE;

  globals.gga_local_y = dLat * GPS_REF_HEAD_COS + dLon * GPS_REF_HEAD_SIN;
  globals.gga_local_x = -dLat * GPS_REF_HEAD_SIN + dLon * GPS_REF_HEAD_COS;
}

void process_gps_string(char *string)
{
  if (strncmp(string, "$GPGGA", 6) != 0)
    return;

  if (!check_nmea_checksum(string))
    return;

  string = strchr(string, ',')+1; // Time
  // ignore

  uint8_t i;
  char temp[10];
  string = strchr(string, ',')+1; // Latitude

  for (i = 0; i < 10; i++)
  {
    if (string[i+2] == '.')
      break;
    temp[i] = string[i];
  }
  temp[i] = '\0';

  int latitude_degrees = atoi(temp);

  string += i;    
  float latitude_minutes = atof(string);
  
  string = strchr(string, ',')+1; // N/S
  uint8_t south = 0;
  if (*string == 'S')
    south = 1;
  else if (*string == 'N')
    south = 0;
  else
    return;
      
  string = strchr(string, ',')+1; // Longitude

  for (i = 0; i < 10; i++)
  {
    if (string[i+2] == '.')
      break;
    temp[i] = string[i];
  }
  temp[i] = '\0';

  float longitude_degrees = atoi(temp);
  string += i;    
  float longitude_minutes = atof(string);
  
  string = strchr(string, ',')+1; // E/W
  uint8_t west = 0;
  if (*string == 'W')
    west = 1;
  else if (*string == 'E')
    west = 0;
  else
    return;

  string = strchr(string, ',')+1; // quality indicator

  if (*string == '0')
    return; // no fix
  
  string = strchr(string, ',')+1; // num Satellites
  // ignore
  
  string = strchr(string, ',')+1; // hdop
  float hdop = atof(string);
  
  string = strchr(string, ',')+1; // altitude
  float altitude = atof(string);
  
  // ignore the remaining fields

  double latitude = latitude_degrees + latitude_minutes/60.0;
  if (south)
    latitude = -latitude;

  double longitude = longitude_degrees + longitude_minutes/60.0;
  if (west)
    longitude = -longitude;

  globals.gga_latitude = latitude;
  globals.gga_longitude = longitude;
  
  globals.gga_hdop = hdop;
  globals.gga_altitude = altitude;

  update_local_xy();
  
  globals.status_bits |= STATUS_GPS_GGA_VALID;
}


void init_gps(void)
{
  active_buffer = -1;
  active_buffer_index = 0;

  // Setup the UART for 115200 baud, transmit/receive, 8bit, 1stop bit, no parity.
  UCSR0A = 0;
  UCSR0B = 0b10011000;
  // Enable RX Interrupts
  // Enable RX
  // Enable TX
  UCSR0C = 0b00000110;
  // 8bit character size
  
  UBRR0H = 0;
  UBRR0L = 103;  
}

void update_gps(void)
{
  for (uint8_t i = 0; i < SERIAL_BUFFER_SIZE; i++)
  {
    globals.gps1_string[i] = 0;
    globals.gps2_string[i] = 0;
    globals.gps3_string[i] = 0;
  }

  globals.gga_latitude = 0;
  globals.gga_longitude = 0;
  globals.gga_hdop = 0;
  globals.gga_altitude = 0;

  if (serial_no_free_buffer)
  {
    globals.status_bits |= STATUS_SERIAL_NO_BUFFER_ERROR;
    serial_no_free_buffer = 0;
  }

  if (serial_buffer_overflow)
  {
    globals.status_bits |= STATUS_SERIAL_BUFFER_OVERFLOW;
    serial_buffer_overflow = 0;
  }
  
  if (serial_unexpected_start)
  {
    globals.status_bits |= STATUS_SERIAL_UNEXPECTED_START;
    serial_unexpected_start = 0;
  }

  if (rx_buffers[0].ready)
  { 
    memcpy(globals.gps1_string, rx_buffers[0].data, SERIAL_BUFFER_SIZE);
    process_gps_string(globals.gps1_string);
    globals.status_bits |= STATUS_GPS1_RECEIVED;
    rx_buffers[0].ready = 0;
  }
  if (rx_buffers[1].ready)
  {
    memcpy(globals.gps2_string, rx_buffers[1].data, SERIAL_BUFFER_SIZE);
    process_gps_string(globals.gps2_string);
    globals.status_bits |= STATUS_GPS2_RECEIVED;
    rx_buffers[1].ready = 0;
  }
  if (rx_buffers[2].ready)
  {
    memcpy(globals.gps3_string, rx_buffers[2].data, SERIAL_BUFFER_SIZE);
    process_gps_string(globals.gps3_string);
    globals.status_bits |= STATUS_GPS3_RECEIVED;
    rx_buffers[2].ready = 0;
  }
}

#endif

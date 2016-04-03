#ifndef _GPS_H_
#define _GPS_H_

#define GPGGA_START             "$GPGGA"
#define GPGSA_START             "$GPGSA"
#define GPRMC_START             "$GPRMC"
#define GPVTG_START             "$GPVTG"
#define START_LENGTH            6
#define GPS_INVALID_HEX_CHAR    0xFF
#define GPS_FIELD_BUFF_SZ       8
#define GPS_TIME_WIDTH          6
#define GPS_SENTENCE_BUFF_SZ    128
#define GPS_SENTENCE_END        '\n'
#define GPS_SENTENCE_START      '$'
#define LAT_LONG_FIELD_LENGTH   9
#define NUM_GPS_SENTENCE_BUFFS  4

void gps_init(void);

void gps_update(void);

#endif


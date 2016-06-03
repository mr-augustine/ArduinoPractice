/*
 *
 */
#ifndef _STATEVARS_H_
#define _STATEVARS_H_

#define GPS_SENTENCE_LENGTH   84
#define GPS_DATE_WIDTH         8

#define STATUS_GPS_NO_BUFF_AVAIL  (1 << 2);
#define STATUS_GPS_BUFF_OVERFLOW  (1 << 3);
#define STATUS_GPS_UNEXPECT_START (1 << 4);
#define STATUS_GPS_GPGGA_RCVD     (1 << 5);
#define STATUS_GPS_GPVTG_RCVD     (1 << 6);
#define STATUS_GPS_GPRMC_RCVD     (1 << 7);
#define STATUS_GPS_GPGSA_RCVD     (1 << 8);
#define STATUS_GPS_NO_FIX_AVAIL   (1 << 9);
#define STATUS_GPS_UNEXPECT_VAL   (1 << 10);
#define STATUS_GPS_DATA_NOT_VALID (1 << 11);

typedef struct {
    uint32_t prefix;
    uint32_t status;
    char     gps_sentence0[GPS_SENTENCE_LENGTH];
    char     gps_sentence1[GPS_SENTENCE_LENGTH];
    char     gps_sentence2[GPS_SENTENCE_LENGTH];
    char     gps_sentence3[GPS_SENTENCE_LENGTH];
    float    gps_latitude;
    float    gps_longitude;
    float    gps_hdop;
    float    gps_pdop;
    float    gps_vdop;
    float    gps_msl_altitude_m;
    float    gps_true_hdg_deg;
    float    gps_ground_course_deg;
    float    gps_speed_kmph;
    float    gps_ground_speed_kt;
    float    gps_speed_kt;
    uint8_t  gps_hours;
    uint8_t  gps_minutes;
    float    gps_seconds;
    char     gps_date[GPS_DATE_WIDTH];
    uint8_t  gps_satcount;
    uint32_t suffix;
} statevars_t;

extern statevars_t statevars;

#endif /* _STATEVARS_H_ */


#ifndef _GPS_H_
#define _GPS_H_

#define GPS_SENTENCE_BUFF_SZ 128
#define GPS_SENTENCE_END '\n'
#define GPS_SENTENCE_START '$'
#define NUM_GPS_SENTENCE_BUFFS 4

void gps_init(void);

void gps_update(void);

#endif


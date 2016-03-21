/*
 *
 */
#ifndef _STATEVARS_H_
#define _STATEVARS_H_

typedef struct {
    uint32_t prefix;
    uint32_t status;
    char     gps_sentence1[84];
    char     gps_sentence2[84];
    char     gps_sentence3[84];
    float    gps_latitude;
    float    gps_longitude;
    float    gps_hdop;
    uint32_t suffix;
} statevars_t;

extern statevars_t statevars;

#endif /* _STATEVARS_H_ */


/*
 *
 */
#ifndef _STATEVARS_H_
#define _STATEVARS_H_

typedef struct {
    uint32_t prefix;
    uint32_t unsigned_long;
    uint16_t unsigned_short;
    uint8_t  unsigned_byte;
    char     sentence[84];
    float    float_value;
    double   double_value;
    int8_t   signed_byte;
    int16_t  signed_short;
    int32_t  signed_long;
    uint32_t suffix;
} statevars_t;

extern statevars_t statevars;

#endif /* _STATEVARS_H_ */


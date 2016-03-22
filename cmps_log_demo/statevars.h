/*
 *
 */
#ifndef _STATEVARS_H_
#define _STATEVARS_H_

typedef struct {
    uint32_t prefix;
    uint32_t main_loop_counter;
    uint16_t heading_raw;
    float    heading_deg;
    uint8_t  pitch_deg;
    uint8_t  roll_deg;
    char     padding[492];        // 512 bytes - sizeof(other struct bytes)
    uint32_t suffix;
} statevars_t;

extern statevars_t statevars;

#endif /* _STATEVARS_H_ */


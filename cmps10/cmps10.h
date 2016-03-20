#ifndef _CMPS10_H_
#define _CMPS10_H_

#define COMPASS_ADDR  0x60
#define COMPASS_HEADING_REG 2
#define COMPASS_PITCH_REG 4
#define COMPASS_ROLL_REG 5

extern uint16_t cmps10_heading;
extern int8_t cmps10_pitch;
extern int8_t cmps10_roll;

void cmps10_init(void);
void cmps10_update_all(void);

#endif /* _CMPS10_H_ */


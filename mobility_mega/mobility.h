#ifndef _MOBILITY_H_
#define _MOBILITY_H_

#define STEERING_ISR_VECT     TIMER1_COMPA_vect
#define STEERING_COMPARE_REG  OCR1A
#define THROTTLE_ISR_VECT     TIMER1_COMPB_vect
#define THROTTLE_COMPARE_REG  OCR1B

#define FWD_TO_STOP_RATE_US   10
#define REV_TO_STOP_RATE_US   50

#define SPEED_FWD_CREEP       1600
#define SPEED_FWD_CRUISE      1800
#define SPEED_FWD_LUDICROUS   2000
#define SPEED_NEUTRAL         1500
#define SPEED_REV_CREEP       1400
#define SPEED_REV_CRUISE      1200
#define SPEED_REV_LUDICROUS   1100
#define TURN_FULL_LEFT        1900
#define TURN_FULL_RIGHT       1100
#define TURN_NEUTRAL          1500

typedef enum {
  Drive_Creep,
  Drive_Cruise,
  Drive_Ludicrous
} Drive_Speed;

uint8_t mobility_init(void);
void mobility_drive_fwd(Drive_Speed speed);
void mobility_drive_rev(Drive_Speed speed);
void mobility_stop(void);
void steer_to_direction(uint16_t turn_degree);

#endif


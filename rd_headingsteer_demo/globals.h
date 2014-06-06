#ifndef _RobotDevil_Globals_h_
#define _RobotDevil_Globals_h_

#include <avr/pgmspace.h>

////////////////////////////////////////////////////////////////////////////////
// Global Constants
#define LOOP_PERIOD_TICKS          6250
#define LOOP_FREQUENCY_HZ          40.0
#define SPEED_CONTROLLER_DELAY     5.0    // to bypass throttle neutral protect
#define STOP_VEHICLE_TIME          12.0   // hard time limit to cease mission
#define DEG_TO_RAD                 0.0174532925199  // 180/pi; multiply degree
                                                    // value by this to get rad

#define STATUS_COMPASS_ERROR       (1L <<  3)
#define STATUS_COMPASS_VALID       (1L << 13)

#define PID_K_PROP                 9.0   // pwm (usec) per deg error
#define PID_K_DIFF                 4.5  // pwm (usec) per (deg error per sec)


////////////////////////////////////////////////////////////////////////////////
// Global Variables

typedef struct {
  uint32_t status_bits;
  uint32_t loop_counter;
  uint8_t  button_press_count;
  uint8_t  heading_set;
  uint8_t  mission_started;
  
  uint16_t compass_raw;
  float    compass_deg;
  float    compass_rad;
  float    target_heading_deg;
  float    heading_error_deg;     // offset from target heading {+/-}
  
  uint16_t steering_servo_us;
  uint16_t gasbrake_servo_us;
  
} globals_t;

extern globals_t globals;



#endif

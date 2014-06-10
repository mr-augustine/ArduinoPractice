#ifndef _RobotDevil_Globals_h_
#define _RobotDevil_Globals_h_

#include <avr/pgmspace.h>

////////////////////////////////////////////////////////////////////////////////
// Global Constants
#define LOOP_PERIOD_TICKS          6250
#define LOOP_FREQUENCY_HZ          40.0
#define SPEED_CONTROLLER_DELAY     5.0    // to bypass throttle neutral protect
#define STOP_VEHICLE_TIME          15.0   // hard time limit to cease mission
#define DEG_TO_RAD                 0.0174532925199  // 180/pi; multiply degree
                                                    // value by this to get rad
#define STATUS_TIMER1_OVERFLOW     (1L <<  0)
#define STATUS_MAIN_LOOP_SLOW      (1L <<  1)
#define STATUS_COMPASS_ERROR       (1L <<  3)
#define STATUS_COMPASS_VALID       (1L << 13)

#define STEERING_GAIN              2.7777777777777  // 500 pwm_us / 180 degrees

// respective LEDs turn ON when the heading was selected; when the mission
// starts; and when the compass is pointing to the target heading (+/- 2 deg)
#define HEADING_SELECTED_LED               (1 << 2)
#define MISSION_STARTED_LED                (1 << 3)
#define ON_TARGET_LED                      (1 << 4)

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

#ifndef _RobotDevil_Globals_h_
#define _RobotDevil_Globals_h_

#include <avr/pgmspace.h>

////////////////////////////////////////////////////////////////////////////////
// Global Constants
#define LOOP_PERIOD_TICKS          6250
#define LOOP_FREQUENCY_HZ          40.0
#define SPEED_CONTROLLER_DELAY     5.0    // to bypass throttle neutral protect
#define STOP_VEHICLE_TIME          120.0   // hard time limit to cease mission
#define DEG_TO_RAD                 0.0174532925199  // 180/pi; multiply degree
                                                    // value by this to get rad
#define RAD_TO_DEG                 57.2957795

#define GLOBAL_START                     0xBABECAFEL
#define GLOBAL_STOP                      0xDEADBEEFL
#define GLOBAL_PADDING_SIZE              150  // 512 - 362

#define METERS_FROM_ENCODER_TICKS        0.000187987592819  // 1.0/5319.5

#define GPS_RADIUS_LONGITUDE             6383576.31721
#define GPS_RADIUS_LATITUDE              6351161.08104
#define GPS_REF_LATITUDE_RAD             0.699377429551343  // SparkFun AVC
#define GPS_REF_LONGITUDE_RAD            -1.836606289228911 // starting line
#define GPS_REF_HEAD_SIN                 0.0
#define GPS_REF_HEAD_COS                 1.0

#define DEFAULT_ENCODER_COVARIANCE       1.0

#define INITIAL_VEHICLE_ANGLE            0.0
#define INITIAL_VEHICLE_X                0.0
#define INITIAL_VEHICLE_Y                0.0
#define INITIAL_VEHICLE_COVARIANCE       1.0


#define STATUS_TIMER1_OVERFLOW           (1L <<  0)
#define STATUS_MAIN_LOOP_SLOW            (1L <<  1)
#define STATUS_ENCODER_ERROR             (1L <<  2)
#define STATUS_COMPASS_ERROR             (1L <<  3)
#define STATUS_GPS1_RECEIVED             (1L <<  4)
#define STATUS_GPS2_RECEIVED             (1L <<  5)
#define STATUS_GPS3_RECEIVED             (1L <<  6)
#define STATUS_SERIAL_NO_BUFFER_ERROR    (1L <<  7)
#define STATUS_SERIAL_BUFFER_OVERFLOW    (1L <<  8)
#define STATUS_SERIAL_UNEXPECTED_START   (1L <<  9)
#define STATUS_SERIAL_UNKNOWN_STRING     (1L << 10)
#define STATUS_ENCODER_VALID             (1L << 11)
#define STATUS_GPS_GGA_VALID             (1L << 12)
#define STATUS_COMPASS_VALID             (1L << 13)

#define STEERING_GAIN              2.7777777777777  // 500 pwm_us / 180 degrees


////////////////////////////////////////////////////////////////////////////////
// Global Variables

typedef struct {
  uint32_t start_bytes;

  uint32_t loop_counter;  
  uint32_t status_bits;
  int16_t  encoder_ticks;
  uint8_t  start_button;
  char     gps1_string[84];
  char     gps2_string[84];
  char     gps3_string[84];
  float    gga_latitude;
  float    gga_longitude;
  float    gga_hdop;
  float    gga_altitude;
  float    gga_local_x;
  float    gga_local_y;
  
  //uint8_t  button_press_count;
  //uint8_t  heading_set;
  //uint8_t  mission_started;
  
  uint16_t compass_raw;
  float    compass_deg;
  float    compass_rad;
  
  float    vehicle_x;
  float    vehicle_y;
  float    vehicle_angle;
  
//  float    target_heading_deg;
//  float    heading_error_deg;
  
  uint16_t waypoint_index;
  uint16_t waypoint_gasbrake_us;
  float    waypoint_x;
  float    waypoint_y;
  float    waypoint_boundary_x;
  float    waypoint_boundary_y;
  float    waypoint_boundary_u;
  float    waypoint_boundary_v;
  float    waypoint_encoder_covariance;
  float    target_angle;

  uint8_t  started;
  uint32_t start_loop;  
  
  //float    target_heading_deg;
  //float    heading_error_deg;     // offset from target heading {+/-}
  
  uint16_t steering_servo_us;
  uint16_t gasbrake_servo_us;

  char padding[150];
  uint32_t stop_bytes;
} globals_t;

extern globals_t globals;

inline void set_padding(globals_t * globals)
{
    for (uint16_t i = 0; i < GLOBAL_PADDING_SIZE; i++)
        globals->padding[i] = 0xAA;
}

#endif

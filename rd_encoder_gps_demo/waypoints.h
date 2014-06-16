#ifndef _waypoints_h_
#define _waypoints_h_

#include <avr/pgmspace.h>
#include "RobotDevil.h"
#include "globals.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
// Waypoints Constants
typedef struct {
    uint16_t gasbrake_us;
    float x;
    float y;
    float boundary_x;
    float boundary_y;
    float boundary_u;
    float boundary_v;
    float encoder_covariance;
} waypoint_t;


#define NUM_WAYPOINTS 9
const waypoint_t waypoint_data[NUM_WAYPOINTS] PROGMEM =
{
    {1800, 37.50, 0.80, 20.50, 12.50, 0.00930, -0.99996, 1.00000},
    {1800, 39.10, 12.10, 25.10, 11.30, 0.00000, -1.00000, 1.00000},
    {1800, 36.50, 40.10, 20.10, 9.20, 0.99989, -0.01507, 1.00000},
    {1800, 35.00, 61.90, 20.10, 50.00, 1.00000, 0.00000, 1.00000},
    {1800, 30.00, 69.90, 20.10, 60.00, 1.00000, 0.00000, 1.00000},
    {1800, 10.00, 64.70, 20.10, 60.00, 0.70354, 0.71065, 1.00000},
    {1800, -39.80, 64.50, -26.10, 60.00, 0.01000, 0.99995, 1.00000},
    {1800, -35.30, -9.50, -25.10, 10.00, -1.00000, 0.00000, 1.00000},
    {1800, 37.50, 0.80, 0.50, 12.50, 0.00930, -0.99996, 1.00000}
};

////////////////////////////////////////////////////////////////////////////////
// Waypoints Functions
uint8_t waypoint_is_valid(void) {
  if (globals.waypoint_index >= NUM_WAYPOINTS)
    return 1;
  
  float dx1 = globals.waypoint_boundary_u;
  float dy1 = globals.waypoint_boundary_v;
  float dx2 = globals.waypoint_boundary_x - globals.vehicle_x;
  float dy2 = globals.waypoint_boundary_y - globals.vehicle_y;

  float det = dx1*dy2 - dy1*dx2;

  if (det >= 0.0)
    return 0;
  return 1;
}

void update_target_angle(void) {
  float dx1 = cos(globals.vehicle_angle);
  float dy1 = sin(globals.vehicle_angle);
  float dx2 = globals.waypoint_x - globals.vehicle_x;
  float dy2 = globals.waypoint_y - globals.vehicle_y;

  float det = dx1*dy2 - dy1*dx2;
  float length = sqrt(dx2*dx2 + dy2*dy2);

  if (length == 0.0) {    
    globals.target_angle = 0.0;
    return;
  }

  det = det / length;
  globals.target_angle = asin(det);
}

void load_waypoint(uint16_t index) {
  globals.waypoint_index = index;
  
  if (index >= NUM_WAYPOINTS) {
    globals.waypoint_gasbrake_us = GASBRAKE_NEUTRAL;
    globals.waypoint_x = 0.0;
    globals.waypoint_y = 0.0;
    globals.waypoint_boundary_x = 0.0;
    globals.waypoint_boundary_y = 0.0;
    globals.waypoint_boundary_u = 0.0;
    globals.waypoint_boundary_v = 1.0;
    globals.waypoint_encoder_covariance = DEFAULT_ENCODER_COVARIANCE;
  } else {
    globals.waypoint_gasbrake_us = pgm_read_word(
      &(waypoint_data[index].gasbrake_us));
      
    globals.waypoint_x = pgm_read_float(&(waypoint_data[index].x));
    globals.waypoint_y = pgm_read_float(&(waypoint_data[index].y));
    
    globals.waypoint_boundary_x = pgm_read_float(
      &(waypoint_data[index].boundary_x));
    globals.waypoint_boundary_y = pgm_read_float(
      &(waypoint_data[index].boundary_y));
      
    globals.waypoint_boundary_u = pgm_read_float(
      &(waypoint_data[index].boundary_u));
    globals.waypoint_boundary_v = pgm_read_float(
      &(waypoint_data[index].boundary_v));
      
    globals.waypoint_encoder_covariance = pgm_read_float(
      &(waypoint_data[index].encoder_covariance));    
  }
}

void set_next_waypoint(void) {
  while (!waypoint_is_valid())
    load_waypoint(globals.waypoint_index + 1);
}

void init_waypoints(void) {
  load_waypoint(0);
}

void update_waypoints(void)
{
  set_next_waypoint();
  update_target_angle();
}

#endif

#ifndef _state_estimator_h_
#define _state_estimator_h_

#include <math.h>
#include "globals.h"
#include "RobotDevil.h"

////////////////////////////////////////////////////////////////////////////////
// State Estimator Functions
void init_state_estimator(void)
{
  globals.vehicle_x = INITIAL_VEHICLE_X;
  globals.vehicle_y = INITIAL_VEHICLE_Y;
  globals.vehicle_angle = INITIAL_VEHICLE_ANGLE * DEG_TO_RAD;
  //globals.vehicle_covariance = INITIAL_VEHICLE_COVARIANCE;
}

void update_state_estimator(void)
{
  globals.vehicle_angle = globals.compass_rad;

  if (!globals.started)
  {
    globals.vehicle_x = INITIAL_VEHICLE_X;
    globals.vehicle_y = INITIAL_VEHICLE_Y;
    //globals.vehicle_covariance = INITIAL_VEHICLE_COVARIANCE;
  }
  
  if (globals.status_bits & STATUS_GPS_GGA_VALID)
  {
    globals.vehicle_x = globals.gga_local_x;
    globals.vehicle_y = globals.gga_local_y;
  }
  else if (globals.status_bits & STATUS_ENCODER_VALID)
  {
    //globals.vehicle_x += cos(globals.vehicle_angle)*globals.encoder_ticks;
    //globals.vehicle_y += sin(globals.vehicle_angle)*globals.encoder_ticks;
    
    globals.vehicle_x += cos(globals.vehicle_angle) * globals.encoder_ticks * 
                         METERS_FROM_ENCODER_TICKS;
    globals.vehicle_y += sin(globals.vehicle_angle) * globals.encoder_ticks * 
                         METERS_FROM_ENCODER_TICKS;
  }
}

#endif

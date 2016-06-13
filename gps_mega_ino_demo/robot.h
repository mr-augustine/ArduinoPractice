#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "pins.h"
//#include "gps.h"
#include "statevars.h"

#ifdef __cplusplus
extern "C" {
  uint8_t sdcard_init(statevars_t * data);
  void sdcard_write_data(void);
}
#endif

#endif

#ifndef _RobotDevil_h_
#define _RobotDevil_h_

void init_pushbutton(void);
void update_pushbutton(void);

void init_compass(void);
void update_compass(void);

void update_control_values(void);

void update_heading_error(void);

void init_logger(void);
void start_log(void);
void finish_log(void);

#endif

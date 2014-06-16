#ifndef _RobotDevil_h_
#define _RobotDevil_h_

void init_pushbutton(void);
void update_pushbutton(void);

void init_compass(void);
void update_compass(void);

void update_control_values(void);

void init_gps(void);
void update_gps(void);
void serial_write(char * cmd);

void init_encoder(void);
void update_encoder(void);

void init_waypoints(void);
void update_waypoints(void);

void init_state_estimator(void);
void update_state_estimator(void);

//void init_logger(void);
//void start_log(void);
//void finish_log(void);

#endif

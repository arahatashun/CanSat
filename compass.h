#ifndef COMPASS_H
#define COMPASS_H
//C++から呼び出すため
#ifdef __cplusplus
extern "C" {
#endif

int compass_initialize();
double cal_deviated_angle(double angle_of_deviation, double theta_degree);
double readCompassAngle(void);
int read_for_calib();
double read_for_calib2(double x, double y);

/*
int cal_maxmin_compass(Cmps_offset *compass_offset,Cmps *compass_data);
*/

#ifdef __cplusplus
}
#endif

#endif

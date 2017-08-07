#ifndef COMPASS_H
#define COMPASS_H
//C++から呼び出すため
#ifdef __cplusplus
extern "C" {
#endif

int compass_initialize();
double readCompassAngle(void);
int read_for_calib();
/*
int cal_maxmin_compass(Cmps_offset *compass_offset,Cmps *compass_data);
*/

#ifdef __cplusplus
}
#endif

#endif

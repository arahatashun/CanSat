#ifndef MITIBIKI_H
#define MITIBIKI_H

//C++から呼び出すため
#ifdef __cplusplus
extern "C" {
#endif

double calc_target_angle(double lat,double lon);
double dist_on_sphere(double current_lat, double current_lon);
double cal_delta_angle(double going_angle_cld, double gps_angle_cld);
 
#ifdef __cplusplus
}
#endif
#endif

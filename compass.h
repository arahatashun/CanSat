#ifndef COMPASS_H
#define COMPASS_H
//C++から呼び出すため
#ifdef __cplusplus
extern "C" {
#endif

typedef struct cmps_offset {
  double compassx_offset_max;//the values of compassx offset
  double compassx_offset_min;
  double compassy_offset_max;
  double compassy_offset_min;
  double compassx_offset;
  double compassy_offset;
} Cmps_offset;


int compass_initialize();
double readCompassAngle(void);

/*
double cal_deg_acclcompass(double x, double y,
                           double z, double sin_phi, double sin_psi,
                           double cos_phi, double cos_psi);
int cal_maxmin_compass(Cmps_offset *compass_offset,Cmps *compass_data);
*/

#ifdef __cplusplus
}
#endif

#endif

#ifndef COMPASS_H
#define COMPASS_H

//構造体宣言(typedef)
typedef struct cmps {
  double compassx_value;//the values of compassx
  double compassy_value;
  double compassz_value;
} Cmps;

int compass_read(Cmps *compass_data);
int print_compass(Cmps *compass_data);
int compass_initializer();
int compass_value_initialize(Cmps *compass_init);
int compass_get_angle();
double cal_theta(double);
double cal_deviated_angle(double);
double cal_deg_acclcompass(double compassx_value, double compassy_value,
                           double compassz_value, double sin_phi, double sin_psi,
                           double cos_phi, double cos_psi);
#endif

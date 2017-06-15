#ifndef COMPASS_H
#define COMPASS_H

//構造体宣言(typedef)
typedef struct cmps {
  double compassx_value;//the values of compassx
  double compassy_value;
  double compassz_value;
} Cmps;

/*typedef struct cmps_offset {
  double compassx_offset_max;//the values of compassx offset
  double compassx_offset_min;
  double compassy_offset_max;
  double compassy_offset_min;
  double compassx_offset;
  double compassy_offset;
} Cmps_offset;*/

int compass_read(Cmps *compass_data);
int compass_read_scatter(Cmps *compass_data);
int print_compass(Cmps *compass_data);
int compass_initializer();
int compass_value_initialize(Cmps *compass_init);
double calc_compass_angle(double x,double y);
double cal_deg_acclcompass(double compassx_value, double compassy_value,
                           double compassz_value, double sin_phi, double sin_psi,
                           double cos_phi, double cos_psi);
/*int cal_maxmin_compass(Cmps_offset *compass_offset,Cmps *compass_data);*/
#endif

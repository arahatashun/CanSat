#ifndef COMPASS_CALIB_H
#define COMPASS_CALIB_H

//構造体宣言(typedef)
typedef struct cmps_offset {
  double compassx_offset_max;//the values of compassx offset
  double compassx_offset_min;
  double compassy_offset_max;
  double compassy_offset_min;
  double compassz_offset_max;
  double compassz_offset_min;
  double comapssx_offset;
  double compassy_offset;
  double compassz_offset;
} Cmps_offset;

int cal_maxmin_compass(Cmps_offset *compass_offset,Cmps *compass_data);

#endif

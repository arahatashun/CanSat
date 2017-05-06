#include <stdio.h>
#include <math.h>

//note: seikei toukei ni izon
static const double target_latitude = 35.7133352;//ido
static const double target_longitude = 139.7599809;//keido
static const double PI = 3.14159265;

double target_angle(double lat,double lon)
{
  double lat_offset = target_latitude - lat;
  double lon_offset = target_longitude - lon;
  double angle = atan2(-lon_offset,-lat_offset)*(180/PI) + 180;
  printf("target_angle : %f\n",angle);
  return angle;
}

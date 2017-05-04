#include <stdio.h>
#include <math.h>
#include <gps.h>

//note: seikei toukei ni izon
static const double target_latitude = 35.7133352;//ido
static const double target_longitude　= 139.7599809;//keido
static const double pi = 3.14159265;

double target_angle(double lat,double lon)
{
  double lat_offset = target_latitude - lat;
  double lon_offset = target_longitude - lon;
  double angle = atan2(-lat_offset,lon_offset)*(180/pi) + 180;
  return angle;
}

int main()
{
  gps_init();
  loc_t data;
  gps_location(&data);
  double angle = target_angle(data.latitude,data.longitude);
  printf("%f",angle);
}

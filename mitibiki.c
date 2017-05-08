#include <stdio.h>
#include <math.h>
#include <gps.h>
//note: seikei toukei ni izon
static const double target_latitude = 35.7133352;//ido
static const double target_longitude = 139.7599809;//keido
static const double PI = 3.14159265;
static const double EARTH_RADIUS = 6378137;
//構造体で角度と距離返す方がいいかもしれない
loc_t data;

typedef struct cartesian_coordinates{
  double x;
  double y;
  double z;
}cartesian_coord

int mitibiki_initializer()
{
  gps_init();
}

double calc_angle(double lat,double lon)
{
  double lat_offset = target_latitude - lat;
  double lon_offset = target_longitude - lon;
  double angle = atan2(-lon_offset,-lat_offset)*(180/PI) + 180;
  printf("target_angle : %f\n",angle);
  return angle;
}

double target_angle()
{
  gps_location(&data);
  printf("latitude:%f, longitude:%f\n", data.latitude, data.longitude);
  double target_angle = calc_angle(data.latitude,data.longitude);
  return target_angle;
}

cartesian_coord latlng_to_xyz(double lat,double lon)
{
  double rlat = lat*PI/180;
  double rlon = lon*PI/180;
  double coslat = cos(rlat);
  cartesian_coord tmp;
  tmp.x =coslat*cos(rlng);
  tmp.y = coslat*sin(rlng);
  tmp.z = sin(rlat);
  return tmp;
}

double dist_on_sphere(cartesian_coord target, cartesian_coord current_position)
{
  double dot_product_x = target.x*current_position.x;
  double dot_product_y = target.y*current_position.y;
  double dot_product_z = target.z*current_position.z;
  double dot_product_sum =dot_product_x+dot_product_y+dot_product_z;
  double distance = arccos(dot_product_sum)*EARTH_RADIUS;
  printf("distance : %f\n",distance);
  return distance;
}

double get_distace()
{
  gps_location(&data);
  printf("latitude:%f, longitude:%f\n", data.latitude, data.longitude);
  cartesian_coord target = latlng_to_xyz(target_latitude,target_longitude);
  cartesian_coord current_position = latlng_to_xyz(data.latitude, data.longitude);
  double distance = dist_on_sphere(target,current_position);
  return distance;
  //always positive value?
}

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <gps.h>
#include <wiringPi.h>
#include "compass.h"
#include "motor.h"

//note: seikei toukei ni izon
static const int turn_milliseconds = 150;//回転するミリ数
static const int turn_power = 60;//turnするpower
static const double target_latitude = 35.716956;//ido
static const double target_longitude = 139.759936;//keido
static const double PI = 3.14159265;
static const double EARTH_RADIUS = 6378137;

loc_t data;//gpsのデータを確認するものをグローバル変数宣言
//デカルト座標
typedef struct cartesian_coordinates{
  double x;
  double y;
  double z;
}cartesian_coord;

cartesian_coord current_position;
cartesian_coord target_position;

//シグナルハンドラ
void handler(int signum)
{
  motor_stop();
  delay(100);
  exit(1);
}
//経度と緯度をデカルト座標に変換
static cartesian_coord latlng_to_xyz(double lat,double lon)
{
  double rlat = 0;
  double rlng = 0;
  double coslat = 0;
  rlat = lat*PI/180;
  rlng = lon*PI/180;
  coslat = cos(rlat);
  cartesian_coord tmp;
  tmp.x =coslat*cos(rlng);
  tmp.y = coslat*sin(rlng);
  tmp.z = sin(rlat);
  return tmp;
}
//距離を計算
static double dist_on_sphere(cartesian_coord target, cartesian_coord current_position)
{
  double dot_product_x = 0;
  double dot_product_y = 0;
  double dot_product_z = 0;
  double dot_product_sum = 0;
  double distance = 0;
  dot_product_x = target.x*current_position.x;
  dot_product_y = target.y*current_position.y;
  dot_product_z = target.z*current_position.z;
  dot_product_sum =dot_product_x+dot_product_y+dot_product_z;
  distance = acos(dot_product_sum)*EARTH_RADIUS;
  printf("distance : %f\n",distance);
  return distance;
}
/*
    GPSの座標と目的地の座標から進む方向を決める
*/
double calc_target_angle(double lat,double lon)
{
  double lat_offset = 0;
  double lon_offset = 0;
  double angle = 0;
  lat_offset = target_latitude - lat;
  lon_offset = target_longitude - lon;
  angle = atan2(-lon_offset,-lat_offset)*(180/PI) + 180;
  printf("target_angle : %f\n",angle);
  return angle;
}

/*gpsと地磁気のデータを更新する*/
int update_angle()
{
  gps_location(&data);
  printf("latitude:%f\nlongitude:%f\n", data.latitude, data.longitude);
  double angle_to_go = 0;//進むべき方角
  angle_to_go = calc_target_angle(data.latitude,data.longitude);
  double delta_angle = 0;//進むべき方角と現在の移動方向の差の角
  double compass_angle_knd;
  compass_get_angle(&compass_angle_knd);
  delta_angle = compass_angle_knd - angle_to_go;
  printf("delta_angle:%f\n",delta_angle);
  target_position = latlng_to_xyz(target_latitude,target_longitude);
  current_position = latlng_to_xyz(data.latitude, data.longitude);
  double distance = 0;
  distance = dist_on_sphere(target_position,current_position);
  printf("distance :%f\n",distance);
  return delta_angle;
}
/*
  進む方角の差が30以上もしくは-30以下で右回転で調整、30以内もしくは-30以上で前進
*/
int decide_route()
{
  double delta_angle = 0;
  delta_angle=update_angle();
  double min_angle = -30;
  double max_angle = 30;
  if(min_angle <= delta_angle && delta_angle <= max_angle)
  {
    motor_forward(100);
    delay(1000);
    motor_stop();
    delay(5000);
  }

  else
  {
    motor_right(turn_power);
    delay(100);
    motor_stop();
    delay(5000);
  }

  return 0;
}

int main()
{
  pwm_initializer();
  gps_init();
  compass_initializer();
  signal(SIGINT, handler);
  while(1)
  {
    decide_route();
  }
  return 0;
}

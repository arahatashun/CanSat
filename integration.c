#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
#include <gps.h>
#include <wiringPi.h>
#include "motor.h"

//note: seikei toukei ni izon
static const int turn_milliseconds = 150;//回転するミリ数
static const int after_turn_milliseconds = 2000;//回転後直進するミリ数
static const int turn_power = 60;//turnするpower
static const double target_latitude = 35.716956;//ido
static const double target_longitude = 139.759936;//keido
static const double PI = 3.14159265;
static const double EARTH_RADIUS = 6378137;
time_t start_time;//開始時刻のグローバル変数宣言


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

/*gpsのデータを更新する*/
int update_angle()
{
  time_t current_time;//時間を取得
  time(&current_time);
  double delta_time = difftime(current_time,start_time);
  printf("timestamp%f\n",delta_time);
  gps_location(&data);
  printf("latitude:%f\nlongitude:%f\n", data.latitude, data.longitude);
  printf("speed:%f\naltitude:%f\ncourse:%f\n",data.speed,data.altitude,data.course);
  double angle_to_go = 0;//進むべき方角
  angle_to_go = calc_target_angle(data.latitude,data.longitude);
  double delta_angle = 0;//進むべき方角と現在の移動方向の差の角
  delta_angle = data.course - angle_to_go;
  printf("delta_angle:%f\n",delta_angle);
  target_position = latlng_to_xyz(target_latitude,target_longitude);
  current_position = latlng_to_xyz(data.latitude, data.longitude);
  double distance = 0;
  distance = dist_on_sphere(target_position,current_position);
  printf("distance :%f\n",distance);
  return delta_angle;
}
/*
  進む方角の差が30未満もしくは330以上まで調整
*/
int decide_route()
{
  double delta_angle = 0;
  delta_angle=update_angle();

  while((30 <= delta_angle && delta_angle <= 180)||(-330 <= delta_angle && delta_angle <= -180))
  {
    printf("moving left\n");
    motor_right(turn_power);
    delay(turn_milliseconds);
    motor_forward(100);
    delay(after_turn_milliseconds);
    delta_angle=update_angle();
  }

  while((180 < delta_angle && delta_angle <= 30)||(-180 < delta_angle && delta_angle < -30))
  {
    printf("moving right\n");
    motor_left(turn_power);
    delay(turn_milliseconds);
    motor_forward(100);
    delay(after_turn_milliseconds);
    delta_angle=update_angle();
  }

  return 0;
}

int main()
{
  time(&start_time);
  pwm_initializer();
  gps_init();
  signal(SIGINT, handler);
  while(1)
  {
    decide_route();
    printf("moving forward\n");
    motor_forward(100);
    delay(5000);
  }
  return 0;
}

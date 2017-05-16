#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <gps.h>
#include <wiringPi.h>
#include "motor.h"

//note: seikei toukei ni izon

static const double target_latitude = 35.716956;//ido
static const double target_longitude = 139.759936;//keido
static const double PI = 3.14159265;
static const double EARTH_RADIUS = 6378137;
loc_t data;//gpsのデータを確認するものをグローバル変数宣言

//シグナルハンドラ
void handler(int signum)
{
  motor_stop();
  exit(1);
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
  gps_location(&data);
  printf("latitude:%f\nlongitude:%f\n", data.latitude, data.longitude);
  printf("speed:%f\naltitude:%f\ncourse:%f\n",data.speed,data.altitude,data.course);
  double angle_to_go = 0;//進むべき方角
  angle_to_go = calc_target_angle(data.latitude,data.longitude);
  double delta_angle = 0;//進むべき方角と現在の移動方向の差の角
  delta_angle = data.course - angle_to_go;
  printf("%f\n",delta_angle);
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
    motor_left(60);
    delay(500);
    motor_forward(100);
    delay(1000);
    delta_angle=update_angle();
  }

  while((180 < delta_angle && delta_angle <= 360)||(-180 < delta_angle && delta_angle < -30))
  {
    printf("moving right\n");
    motor_right(60);
    delay(500);
    motor_forward(100);
    delay(1000);
    delta_angle=update_angle();
  }

  return 0;
}

int main()
{
  pwm_initializer();
  gps_init();
  signal(SIGINT, handler);
  while(1)
  {
    decide_route();
    printf("moving forward\n");
    motor_forward(100);
    delay(1000);
  }
  return 0;
}

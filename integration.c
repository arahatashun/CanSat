#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
#include <gps.h>
#include <wiringPi.h>
#include "motor.h"

//note: seikei toukei ni izon
static const int turn_milliseconds = 150;//45度回転するミリ秒
static const int after_turn_milliseconds = 1500;//回転後直進するミリ数
static const int turn_power = 60;//turnするpower
static const double target_latitude = 35.716956;//ido
static const double target_longitude = 139.759936;//keido
static const double PI = 3.14159265;
static const double EARTH_RADIUS = 6378137;
static const int gps_latency = 1500;//gps角度取得のための時間感覚
time_t start_time;//開始時刻のグローバル変数宣言


loc_t data;//gpsのデータを確認するものをグローバル変数宣言
//デカルト座標
typedef struct cartesian_coordinates {
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
	printf("GPS distance : %f\n",distance);
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
	printf("GPS target_angle: %f\n",angle);
	return angle;
}
//gpsの緯度経度二回分から角度計算
int angle_gps(double *angle_course)
{
	gps_location(&data);
	double latitude_before = 0;
	double longitude_before = 0;
	latitude_before = data.latitude;
	longitude_before = data.longitude;
	printf("GPS latitude:%f\nGPS longitude:%f\n", latitude_before, longitude_before);
	printf("GPS speed:%f\nGPS altitude:%f\n",data.speed,data.altitude);
	delay(after_turn_milliseconds);
	gps_location(&data);
	double latitude_after = 0;
	double longitude_after = 0;
	latitude_after = data.latitude;
	longitude_after = data.longitude;
	double lat_offset = latitude_after - latitude_before;
	double lon_offset = longitude_after - longitude_before;
	double going_angle = atan2(-lon_offset,-lat_offset)*(180/PI) + 180;//移動中の角度
	printf("GPS going_angle:%f\n",going_angle);
	*angle_course = going_angle;
	return 0;
}

int cal_delta_angle(double going_angle_cld, double gps_angle_cld)
{
    double delta_angle_cld = 0;
    delta_angle_cld = going_angle_cld - gps_angle_cld;
    if(-360 <= delta_angle && delta_angle <= -180)
    {
        delta_angle_cld = 360.0 - going_angle_cld + gps_angle_cld;
    }
    else if(-180 < delta_angle  && delta_angle < 0)
    {
        delta_angle_cld = delta_angle_cld;
    }
    else if(0 <= delta_angle && delta_angle <= 180)
    {
        delta_angle_cld = delta_angle_cld;
    }
    else
    {
        delta_angle_cld = -360.0 + gps_angle_cld - going_angle_cld;
    }
    
    return delta_angle_cld;
}
/*
   gpsのデータを更新する
   delta_angleは現在の角度-進むべき向き
 */
int update_angle()
{
	time_t current_time;//時間を取得
	time(&current_time);
	double delta_time = difftime(current_time,start_time);
	printf("OS timestamp:%f\n",delta_time);
	double angle_course = 0;//移動中の角度
	angle_gps(&angle_course);
	double angle_to_go = 0;//進むべき方角
	angle_to_go = calc_target_angle(data.latitude,data.longitude);
	double delta_angle = 0;//進むべき方角と現在の移動方向の差の角
    cal_delta_angle(angle_course,angle_to_go);
    printf("GPS delta_angle:%f\n",delta_angle);//目的地の方角を0として今のマシンの方角がそれからどれだけずれているかを-180~180で表示 目的方角が右なら値は正
	target_position = latlng_to_xyz(target_latitude,target_longitude);
	current_position = latlng_to_xyz(data.latitude, data.longitude);
	double distance = 0;
	distance = dist_on_sphere(target_position,current_position);
	return delta_angle;
}
/*
   進む方角が-180から-30の時にその角度差に応じて左回転、30~180の時その角度さに応じて右回転
*/
int decide_route()
{
	double delta_angle = 0;
	delta_angle=update_angle();

	while((-180 <= delta_angle && delta_angle <= -30))
	{
		motor_left(turn_power);
		delay((int)((-delta_angle/30)*turn_milliseconds));
		motor_forward(100);
		delta_angle=update_angle();
	}

	while(30 < delta_angle && delta_angle <= 180)
	{
		motor_right(turn_power);
        delay((int)((delta_angle/30)*turn_milliseconds));
		motor_forward(100);
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
		motor_forward(100);
		delay(3000);
		decide_route();
	}
	return 0;
}

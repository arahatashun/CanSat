#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
#include <gps.h>
#include <wiringPi.h>
#include "mitibiki.h"
#include "motor.h"

static const int turn_milliseconds =60;//30度回転する
static const int turn_power = 60;//turnするpower
static const int gps_latency = 3300;//gps角度取得のための時間感覚
static const int forward_power = 50;
static const double PI = 3.14159265;

time_t start_time;//開始時刻のグローバル変数宣言
loc_t data;//gpsのデータを確認するものをグローバル変数宣言

//モーター用シグナルハンドラ
void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}

//gpsの緯度経度二回分から角度計算
int angle_gps(double *angle_course)
{
	printf("gps_on\n");
	gps_init();
	gps_location(&data);
	gps_off();
	printf("gps_off\n");
	double latitude_before = 0;
	double longitude_before = 0;
	latitude_before = data.latitude;
	longitude_before = data.longitude;
	printf("GPS latitude:%f\nGPS longitude:%f\n", latitude_before, longitude_before);
	delay(gps_latency);
	printf("gps_on\n");
	gps_init();
	gps_location(&data);
	gps_off();
	printf("gps_off\n");
	double latitude_after = 0;
	double longitude_after = 0;
	latitude_after = data.latitude;
	longitude_after = data.longitude;
	printf("GPS latitude:%f\nGPS longitude:%f\n", latitude_after, longitude_after);
	double lat_offset = latitude_after - latitude_before;
	double lon_offset = longitude_after - longitude_before;
	double going_angle = atan2(-lon_offset,-lat_offset)*(180/PI) + 180;//移動中の角度
	printf("GPS going_angle:%f\n",going_angle);
	*angle_course = going_angle;
	return 0;
}

/*
   gpsのデータを更新する
   delta_angleは現在の角度-進むべき向きを-180~180になるように調整したもの
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
	delta_angle = cal_delta_angle(angle_course,angle_to_go);
	printf("GPS delta_angle:%f\n",delta_angle);
	/*
		目的地の方角を0として今のマシンの方角がそれから
		どれだけずれているかを-180~180で表示
	  目的方角が右なら値は正
	*/
	double distance = 0;
	distance = dist_on_sphere(data.latitude,data.longitude);
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
		motor_forward(forward_power);
		delta_angle=update_angle();
	}

	while(30 < delta_angle && delta_angle <= 180)
	{
		motor_right(turn_power);
		delay((int)((delta_angle/30)*turn_milliseconds));
		motor_forward(forward_power);
		delta_angle=update_angle();
	}
	return 0;
}

int main()
{
	time(&start_time);
	pwm_initializer();
	signal(SIGINT, handler);
	while(1)
	{
		motor_forward(forward_power);
		decide_route();
	}
	return 0;
}

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
#include <gps.h>
#include <wiringPi.h>
#include "mitibiki.h"
#include "motor.h"
#include "ring_buffer.h"

static const int TURN_MILLISECONDS =10;//30度回転する
static const int TURN_POWER = 60;//turnするpower
static const int GPS_LATENCY = 2500;//gps角度取得のための時間感覚
static const int FORWARD_POWER = 50;
static const double PI = 3.14159265;
static const int GPS_RING_LEN = 2;


Queue *gps_lat_ring = NULL;
Queue *gps_lon_ring = NULL;

//モーター用シグナルハンドラ
void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}

//GPSのデータを取得
//NOTE delayがある
int update_gps(Queue* latring,Queue* lonring)
{
	loc_t data;
	gps_location(&data);
	enqueue(latring,data.latitude);
	enqueue(lonring,data.longitude);
	printf("GPS latitude:%f\nGPS longitude:%f\n", data.latitude, data.longitude);
	delay(GPS_LATENCY);
	return 0;
}

//gpsの緯度経度二回分から角度計算
double calc_gps_angle(Queue* latring,Queue* lonring)
{
	double latitude_before = dequeue(latring);
	double longitude_before = dequeue(lonring);
	double latitude_after= latring->buff[latring->rear];
	double longitude_after= lonring->buff[lonring->rear];
	double lat_offset = latitude_after - latitude_before;
	double lon_offset = longitude_after - longitude_before;
	double angle_course = atan2(-lon_offset,-lat_offset)*(180/PI) + 180;//移動中の角度
	printf("GPS going_angle:%f\n",angle_course);
	return angle_course;
}


int angle_gps(double* angle_course,Queue* latring,Queue* lonring)
{
	//ring_bufferが2回分のデータを保持するまでぶん回す
	while(!is_full(latring))
	{
		update_gps(latring,lonring);
	}
	*angle_course = calc_gps_angle(latring,lonring);
	return 0;
}

/*
   gpsのデータを更新する
   delta_angleは現在の角度-進むべき向きを-180~180になるように調整したもの
 */
double update_angle(double* diffAngle,Queue* latring,Queue *lonring)
{
	time_t current_time;//時間を取得
	time(&current_time);
	printf("%s\n",ctime(&current_time));
	double angle_course = 0;//移動中の角度
	angle_gps(&angle_course,latring,lonring);
	double angle2go = 0;//進むべき方角
	angle2go = calc_target_angle(latring->buff[latring->rear],lonring->buff[lonring->rear]);
	*diffAngle = 0;//進むべき方角と現在の移動方向の差の角
	*diffAngle = cal_delta_angle(angle_course,angle2go);
	printf("GPS delta_angle:%f\n",*diffAngle);
	double distance = 0;
	distance = dist_on_sphere(latring->buff[latring->rear],lonring->buff[lonring->rear]);
	return distance;
}


/*
   進む方角が-180から-30の時にその角度差に応じて左回転、30~180の時その角度さに応じて右回転
 */
int decide_route()
{
	double delta_angle = 0;
	update_angle(&delta_angle,gps_lat_ring,gps_lon_ring);

	while((-180 <= delta_angle && delta_angle <= -30))
	{
		motor_left(TURN_POWER);
		delay((int)((-delta_angle/30)*TURN_MILLISECONDS));
		motor_forward(FORWARD_POWER);
		delta_angle=update_angle(&delta_angle,latring,lonring);
	}

	while(30 < delta_angle && delta_angle <= 180)
	{
		motor_right(TURN_POWER);
		delay((int)((delta_angle/30)*TURN_MILLISECONDS));
		motor_forward(FORWARD_POWER);
		delta_angle=update_angle(&delta_angle,latring,lonring);
	}
	return 0;
}

int main()
{
	gps_init();
	pwm_initializer();
	signal(SIGINT, handler);
	gps_lat_ring = make_queue(GPS_RING_LEN);
	gps_lon_ring = make_queue(GPS_RING_LEN);
	while(1)
	{
		motor_forward(FORWARD_POWER);
		decide_route();
	}
	return 0;
}

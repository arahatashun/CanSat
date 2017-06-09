#include <stdio.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <gps.h>
#include <wiringPi.h>
#include "compass.h"
#include "motor.h"
#include "mitibiki.h"
#include "ring_buffer.h"

static const int turn_power = 60;//turnするpower
static const int turn_milliseconds = 100;//turnするmilliseconds
static const int forward_milliseconds = 1000;//forwardするmilliseconds
static const int stop_milliseconds = 100;//地磁気安定のためにstopするmilliseconds
static const int angle_of_deviation = -7; //地磁気の偏角を考慮
static const double PI = 3.14159265359;
static const int gps_ring_len = 10;//gpsのリングバッファの長さ
static const double stack_threshold = 0.00003; //stack判定するときの閾値

time_t start_time;//開始時刻のグローバル変数宣言
loc_t data;//gpsのデータを確認するものをグローバル変数宣言

Cmps compass_data;      //地磁気の構造体を宣言
Cmps_offset compass_offset;  //地磁気のこう
Queue *gps_lat_ring = NULL; //緯度を格納するキューを用意
Queue *gps_lon_ring = NULL; //経度を格納するキューを用意

//シグナルハンドラ
void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}

/*
   地磁気と６軸センサーからマシンの向いている角度を計算
 */
int cal_compass_theta(double *theta_degree)
{
	double compass_x = 0;
	double compass_y = 0;
	compass_value_initialize(&compass_data);
	print_compass(&compass_data);
	compass_x = compass_data.compassx_value - compass_offset.compassx_offset;
	compass_y = compass_data.compassy_value - compass_offset.compassy_offset;
	*theta_degree = calc_compass_angle(compass_x, compass_y);//偏角を調整
	printf("compass_degree = %f\n", *theta_degree);
	return 0;
}

/*
   gpsと地磁気のデータを一回分更新し、リングバッファに格納
 */
int update_angle(double *delta_angle,double *distance)
{
	time_t current_time;//時間を取得
	time(&current_time);
	double delta_time = difftime(current_time,start_time);
	printf("OS timestamp:%f\n",delta_time);
	gps_location(&data);                   //gpsデータ取得
	enqueue(gps_lat_ring,data.latitude);   //緯度を格納
	enqueue(gps_lon_ring,data.longitude);  //経度を格納
	printf("latitude:%f\nlongitude:%f\n", data.latitude, data.longitude);
	double angle_to_go = 0;//進むべき方角
	double compass_angle = 0;//地磁気から今のマシンの向きを計算して代入するための変数
	angle_to_go = calc_target_angle(data.latitude,data.longitude);
	cal_compass_theta(&compass_angle);
	*delta_angle = 0;
	*delta_angle = cal_delta_angle(compass_angle,angle_to_go);
	printf("delta_angle:%f\n",*delta_angle);
	*distance = 0;
	*distance = dist_on_sphere(data.latitude,data.longitude);
	if(queue_length(gps_lat_ring)==10)
	{
		double delta_movement = 0;
		delta_movement = fabs(data.latitude-dequeue(gps_lat_ring)) +
		                 fabs(data.longitude-dequeue(gps_lon_ring));
		if(delta_movement<stack_threshold)
		{
			motor_stack();
		}
	}
	return 0;
}
/*
   目的方角が自分から見て右に３０度以上ずれていたら右回転、
   目的方角が自分から見て左に３０度以上ずれていたら左回転、
   30以内もしくは-30以上で前進
 */
int decide_route()
{
	double delta_angle = 0;
	double dist_to_goal = 0;
	update_angle(&delta_angle,&dist_to_goal);
	if(dist_to_goal<10)
	{
		printf("==========GOAL==========");
		return -2;        //ゴールに着いた
	}
	if(-180 <= delta_angle && delta_angle <= -30) //ゴールの方角がマシンから見て左に30~180度の場合は左回転
	{
		motor_left(turn_power);
		delay(turn_milliseconds);
		motor_stop();
		delay(stop_milliseconds);
	}
	else if(30 <= delta_angle && delta_angle <= 180)         //ゴールの方角がマシンから見て右に30~180度の場合は右回転
	{
		motor_right(turn_power);
		delay(turn_milliseconds);
		motor_stop();
		delay(stop_milliseconds);
	}
	else
	{
		motor_forward(100);
		delay(forward_milliseconds);
		motor_stop();
		delay(stop_milliseconds);
	}
	return 0;
}

int main()
{
	time(&start_time);
	signal(SIGINT, handler);
	acclgyro_initializer();
	pwm_initializer();
	gps_init();
	compass_initializer();
	cal_maxmin_compass(&compass_offset,&compass_data);
	gps_lat_ring = make_queue(gps_ring_len);
	gps_lon_ring = make_queue(gps_ring_len);
	while(decide_route()!=-2) ;
	return 0;
}

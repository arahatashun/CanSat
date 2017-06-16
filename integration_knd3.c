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
static const int stop_milliseconds = 1000;//地磁気安定のためにstopするmilliseconds
static const int gps_ring_len = 10;//gpsのリングバッファの長さ
static const double stack_threshold = 0.00003; //stack判定するときの閾値
static const double COMPASS_X_OFFSET = 0.0; //ここに手動でキャリブレーションしたoffset値を代入
static const double COMPASS_Y_OFFSET = 0.0;
static const double  GOAL_THRESHOLD = 1;

/*地磁気で得たマシンの向き、GPSで得たゴールまでの方角、そのdelta_angle、ゴールまでの距離を構造体に格納*/
typedef struct dist_and_angle {
	double angle_by_compass;
	double angle_by_gps;
	double dist_to_goal;
	double delta_angle;
}DistAngle;

time_t start_time;//開始時刻のグローバル変数宣言
loc_t data;//gpsのデータを確認するものをグローバル変数宣言

Cmps compass_data;      //地磁気の構造体を宣言
DistAngle distangle_data;
Queue *gps_lat_ring = NULL; //緯度を格納するキューを用意
Queue *gps_lon_ring = NULL; //経度を格納するキューを用意

//シグナルハンドラ
void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}

int DistAngle_initializer(DistAngle *data)
{
	data->angle_by_compass = 0;
	data->angle_by_gps = 0;
	data->dist_to_goal = 100000;
	data->delta_angle = 0;
	return 0;
}
/*
   地磁気とそのオフセット値からマシンの向いている角度を計算
 */
int cal_compass_theta(DistAngle *data)
{
	double compass_x = 0;
	double compass_y = 0;
	compass_value_initialize(&compass_data);
	print_compass(&compass_data);
	compass_x = data.compassx_value - COMPASS_X_OFFSET;
	compass_y = data.compassy_value - COMPASS_Y_OFFSET;
	data->angle_by_compass = calc_compass_angle(compass_x, compass_y);//偏角を調整
	printf("compass_degree = %f\n", distangle_data->angle_by_compass);
	return 0;
}

/*
   gpsと地磁気のデータを一回分更新し、リングバッファに格納
 */
int update_angle(Distangle *distangle_data)
{
	time_t current_time;//時間を取得
	time(&current_time);
	double delta_time = difftime(current_time,start_time);
	printf("OS timestamp:%f\n",delta_time);
	gps_location(&data);                   //gpsデータ取得
	if(data.latitude== 0.0)                //gpsの緯度が0.0の時(衛星を取得していない時)
	{
		printf("GPS satellites not found\n");
		printf("previous GPS_angle=%f\n",distangle_data->angle_by_gps);
		cal_compass_theta(distangle_data); //地磁気だけ取っておく
		distangle_data->delta_angle = cal_delta_angle(distangle_data->angle_by_compass,distangle_data->angle_by_gps);//GPS_angleは元の値を使用
		printf("delta_angle:%f\n",distangle_data->delta_angle);
	}
	else
	{
		enqueue(gps_lat_ring,data.latitude); //緯度を格納
		enqueue(gps_lon_ring,data.longitude); //経度を格納
		printf("latitude:%f\nlongitude:%f\n", data.latitude, data.longitude);
		distangle_data->dist_to_goal = dist_on_sphere(data.latitude,data.longitude);
		distangle_data->angle_by_gps = calc_target_angle(data.latitude,data.longitude);
		cal_compass_theta(distangle_data);
		distangle_data->delta_angle = cal_delta_angle(distangle_data->angle_by_compass,distangle_data->angle_by_gps);
		printf("delta_angle:%f\n",distangle_data->delta_angle);
	}
	if(queue_length(gps_lat_ring)==10)
	{
		double delta_movement = 0;
		delta_movement = fabs(data.latitude-dequeue(gps_lat_ring)) +
		                 fabs(data.longitude-dequeue(gps_lon_ring));
		printf("delta_movement = %f\n", delta_movement);
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
	update_angle(&distangle_data);
	if(distangle_data.dist_to_goal<GOAL_THRESHOLD)
	{
		printf("==========GOAL==========");
		return -2;        //ゴールに着いた
	}
	//ゴールの方角がマシンから見て左に30~180度の場合は左回転
	if(-180 <= distangle_data.delta_angle && distangle_data.delta_angle <= -30)
	{
		motor_left(turn_power);
		delay(turn_milliseconds);
		motor_stop();
		delay(stop_milliseconds);
	}
	//ゴールの方角がマシンから見て右に30~180度の場合は右回転
	else if(30 <= distangle_data.delta_angle && distangle_data.delta_angle <= 180)
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
	printf("\n"); //１つのシーケンスの終わり
	return 0;
}

int main()
{
	time(&start_time);
	signal(SIGINT, handler);
	pwm_initializer();
	gps_init();
	compass_initializer();
	distangle_initializer();
	gps_lat_ring = make_queue(gps_ring_len);
	gps_lon_ring = make_queue(gps_ring_len);
	while(decide_route()!=-2) ;
	return 0;
}
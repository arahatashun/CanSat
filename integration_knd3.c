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

<<<<<<< HEAD
static const int turn_power = 60;//turnするpower
static const int turn_milliseconds = 100;//turnするmilliseconds
static const int forward_milliseconds = 1000;//forwardするmilliseconds
static const int stop_milliseconds = 1000;//地磁気安定のためにstopするmilliseconds
static const int gps_ring_len = 10;//gpsのリングバッファの長さ
static const double stack_threshold = 0.00003; //stack判定するときの閾値
static const double COMPASS_X_OFFSET = 0.0; //ここに手動でキャリブレーションしたoffset値を代入
static const double COMPASS_Y_OFFSET = 0.0;
static const double  GOAL_THRESHOLD = 1;
=======
static const int TURN_POWER = 60;//turnするpower
static const int TURN_MILLISECONDS = 100;//turnするmilliseconds
static const int FORWARD_MILLISECONDS = 1000;//forwardするmilliseconds
static const int STOP_MILLISECONDS = 1000;//地磁気安定のためにstopするmilliseconds
static const int GPS_RING_LEN = 10;//gpsのリングバッファの長さ
static const double STACK_THRESHOLD = 0.00003; //stack判定するときの閾値
static const double COMPASS_X_OFFSET = 0.0; //ここに手動でキャリブレーションしたoffset値を代入
static const double COMPASS_Y_OFFSET = 0.0;
static const int GOAL_THRESHOLD = 2;
>>>>>>> inte

typedef struct dist_and_angle {
	double angle_by_compass;//地磁気による向き
	double angle2goal;//gpsによるゴールまでの角度
	double dist2goal;
	double delta_angle;
}DistAngle;

//グローバル変数
time_t start_time;//開始時刻

<<<<<<< HEAD
Cmps compass_data;      //地磁気の構造体を宣言
DistAngle distangle_data;
Queue *gps_lat_ring = NULL; //緯度を格納するキューを用意
Queue *gps_lon_ring = NULL; //経度を格納するキューを用意
=======
>>>>>>> inte

//シグナルハンドラ
void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}

<<<<<<< HEAD
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
=======
int DistAngle_initialize(DistAngle *data)
{
	data->angle_by_compass = 0;
	data->angle2goal = 0;
	data->dist2goal = 100000;
	data->delta_angle = 0;
	return 0;
}

//地磁気の計測及びとそのオフセット値からマシンの向いている角度を計算
>>>>>>> inte
int cal_compass_theta(DistAngle *data)
{
	Cmps compass_data;
	compass_value_initialize(&compass_data);
	print_compass(&compass_data);
<<<<<<< HEAD
	compass_x = data.compassx_value - COMPASS_X_OFFSET;
	compass_y = data.compassy_value - COMPASS_Y_OFFSET;
	data->angle_by_compass = calc_compass_angle(compass_x, compass_y);//偏角を調整
	printf("compass_degree = %f\n", distangle_data->angle_by_compass);
=======
	double compass_x = 0;
	double compass_y = 0;
	compass_x = compass_data.x_value - COMPASS_X_OFFSET;
	compass_y = compass_data.y_value - COMPASS_Y_OFFSET;
	data->angle_by_compass = calc_compass_angle(compass_x, compass_y);//偏角を調整
	printf("compass_degree = %f\n",data->angle_by_compass);
	return 0;
}

//地磁気の取得及びgpsと地磁気の計算
int calc_all(loc_t coord,DistAngle *data,Queue* latring,Queue* lonring)
{
	enqueue(latring,coord.latitude); //緯度を格納
	enqueue(lonring,coord.longitude); //経度を格納
	printf("latitude:%f\nlongitude:%f\n", coord.latitude, coord.longitude);
	data->dist2goal = dist_on_sphere(coord.latitude,coord.longitude);
	data->angle2goal = calc_target_angle(coord.latitude,coord.longitude);
	cal_compass_theta(data);
	data->delta_angle = cal_delta_angle(data->angle_by_compass,data->angle2goal);
	printf("delta_angle:%f\n",data->delta_angle);
	return 0;
}

//NOTE gps０問題対策
//gps情報をring_bufferに入れずに地磁気によるdataの更新のみ
int handle_gps_zero(DistAngle *data)
{
	printf("GPS return 0 value\n");
	printf("previous GPS_angle=%f\n",data->angle2goal);
	cal_compass_theta(data); //地磁気だけ取っておく
	data->delta_angle = cal_delta_angle(data->angle_by_compass,data->angle2goal);//GPS_angleは元の値を使用
	printf("delta_angle:%f\n",data->delta_angle);
	return 0;
}
//スタック判定をして抜け出す処理まで
int stack(Queue *latring,Queue *lonring)
{
	double delta_movement = 0;
	delta_movement = fabs(latring->buff[latring->rear]-dequeue(latring)) +
									 fabs(lonring->buff[lonring->rear]-dequeue(lonring));
	printf("delta_movement = %f\n", delta_movement);
	if(delta_movement<STACK_THRESHOLD)
	{
		printf("STACK JUDGEMENT\n");
		motor_stack();
	}
>>>>>>> inte
	return 0;
}

//gpsと地磁気のデータを一回分更新し、リングバッファに格納
int update_angle(DistAngle *data,Queue* latring,Queue* lonring)
{
	time_t current_time;//時間を取得
	time(&current_time);
	double delta_time = difftime(current_time,start_time);
	printf("OS timestamp:%f\n",delta_time);
	loc_t coord;
	gps_location(&coord);//gpsデータ取得
	if(coord.latitude != 0.0)
	{
		calc_all(coord,data,latring,lonring);
	}
	else//例外処理
	{
		handle_gps_zero(data);
	}
	if(queue_length(latring)==10)//stack 判定
	{
		stack(latring,lonring);
	}
	return 0;
}

//goal判定で-2を返してそれ以外は0
int decide_route(DistAngle data,Queue *latring,Queue *lonring)
{
<<<<<<< HEAD
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
=======
	update_angle(&data,latring,lonring);
	if(data.dist2goal>GOAL_THRESHOLD)
	{
		if(-180 <= data.delta_angle && data.delta_angle <= -30)
		{
			//ゴールの方角がマシンから見て左に30~180度の場合は左回転
			motor_left(TURN_POWER);
			delay(TURN_MILLISECONDS);
			motor_stop();
			delay(STOP_MILLISECONDS);
		}
		else if(30 <= data.delta_angle && data.delta_angle <= 180)
		{
		 	//ゴールの方角がマシンから見て右に30~180度の場合は右回転
			motor_right(TURN_POWER);
			delay(TURN_MILLISECONDS);
			motor_stop();
			delay(STOP_MILLISECONDS);
		}
		else
		{
			//直進
			motor_forward(100);
			delay(FORWARD_MILLISECONDS);
			motor_stop();
			delay(STOP_MILLISECONDS);
		}
>>>>>>> inte
	}
	else
	{
		printf("==========GOAL==========");
		return -2;//ゴールに着いた
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
	compass_initialize();
	DistAngle DistAngle_data;
	Queue* gps_latring = make_queue(GPS_RING_LEN);
	Queue* gps_lonring = make_queue(GPS_RING_LEN);
	DistAngle_initialize(&DistAngle_data);
	while(decide_route(DistAngle_data,gps_latring,gps_lonring)!=-2) ;
	return 0;
}

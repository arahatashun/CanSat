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
#include "pid.h"
#include "acclgyro.h"

static const int GPS_RING_LEN = 5;//gpsのリングバッファの長さ
static const double STACK_THRESHOLD = 0.000001; //stack判定するときの閾値
static const int GOAL_THRESHOLD = 5;
static const int SETPOINT = 0.0;//delta_angleの目標値
static const double KP_VALUE= 0.4539925;
static const double KI_VALUE = 0.00001453125;
static const double KD_VALUE = 0;
static const int PID_LEN = 12;


typedef struct dist_and_angle {
	double angle_by_compass;//地磁気による向き
	double angle2goal;//gpsによるゴールまでの角度
	double dist2goal;
	double delta_angle;
}DistAngle;


//シグナルハンドラ
void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}


int DistAngle_initialize(DistAngle *data)
{
	data->angle_by_compass = 0;
	data->angle2goal = 0;
	data->dist2goal = 100000;
	data->delta_angle = 0;
	return 0;
}

int printTime()
{
	time_t timer;
	time(&timer);
	printf("%s\n", ctime(&timer));
	return 0;
}

//地磁気の計測及びとそのオフセット値からマシンの向いている角度を計算
int cal_compass_theta(DistAngle *data)
{
	data->angle_by_compass = readCompassAngle();                //偏角を調整
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
	int i;
	delta_movement = fabs(getLast(latring)-dequeue(latring)) +
	                 fabs(getLast(lonring)-dequeue(lonring));
	printf("delta_movement = %f\n", delta_movement);
	if(delta_movement<STACK_THRESHOLD)
	{
		for(i=0; i<2; i++)
		{
			printf("STACK JUDGEMENT\n");
			motor_escape();
		}
		delay(1000);
		gps_off();
		gps_init();

	}
	return 0;
}

//gpsと地磁気のデータを一回分更新し、リングバッファに格納
int update_angle(DistAngle *data,Queue* latring,Queue* lonring)
{
	printTime();
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
	if(queue_length(latring)==GPS_RING_LEN)//stack 判定
	{
		stack(latring,lonring);
	}
	return 0;
}

//goal判定で-2を返してそれ以外は0
int decide_route(DistAngle *data,Queue *latring,Queue *lonring)
{
	Pid pid_data;
	int i;
	pid_initialize(&pid_data);
	pid_const_initialize(&pid_data,SETPOINT,KP_VALUE,KI_VALUE,KD_VALUE);
	for(i=0; i<PID_LEN; i++)
	{
		update_angle(data,latring,lonring);
		if(data->dist2goal>GOAL_THRESHOLD)
		{
			pid_data.input = -(data->delta_angle);
			compute_output(&pid_data);
			printf("pid_output = %d\n",pid_data.output);
			motor_slalom(pid_data.output);
			delay(50);
		}
		else
		{
			printf("COMPASS NAVIGATION FINISHED");
			motor_stop();
			delay(1000);
			return -2;        //ゴールに着いた
		}
	}
	if(isReverse() == -1)
	{
		motor_stop();
		delay(1000);
		motor_forward(100);
		delay(2000);
	}
	printf("integral finish\n");
	printf("\n");  //１つのシーケンスの終わり
	return 0;
}

int main()
{
	signal(SIGINT, handler);
	printTime();
	pwm_initialize();
	gps_init();
	compass_initialize();
	acclGyro_initialize();
	DistAngle DistAngle_data;
	DistAngle_initialize(&DistAngle_data);
	Queue* gps_latring = make_queue(GPS_RING_LEN);
	Queue* gps_lonring = make_queue(GPS_RING_LEN);
	while(decide_route(&DistAngle_data,gps_latring,gps_lonring) != -2) ;
	return 0;
}

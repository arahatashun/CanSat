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

static const int GPS_RING_LEN = 10;//gpsのリングバッファの長さ
static const double STACK_THRESHOLD = 0.000001; //stack判定するときの閾値
static const int GOAL_THRESHOLD = 5;
static const int SETPOINT = 0.0;//delta_angleの目標値
static const double KP_VALUE= 0.50625;
static const double KI_VALUE = 0.00001625;
static const double KD_VALUE = 0;
static const int PID_LEN = 12;
static const int STACK_COUNTER = 50;

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

int printTime()
{
	time_t timer;
	time(&timer);
	printf("%s\n", ctime(&timer));
	return 0;
}

//DistAngleの更新
int updateDistAngle(DistAngle *data,Queue* latring,Queue* lonring)
{
	data->dist2goal = dist_on_sphere(getLast(latring),getLast(lonring));
	data->angle2goal = calc_target_angle(getLast(latring),getLast(lonring));
	data->angle_by_compass = readCompassAngle();
	data->delta_angle = cal_delta_angle(data->angle_by_compass,data->angle2goal);
	printf("delta_angle:%f\n",data->delta_angle);
	return 0;
}

//gps０問題対策
int handleGpsZero(loc_t coord,Queue* latring,Queue* lonring)
{
	printf("GPS return 0 value\n");
	coord.latitude = getLast(latring);
	coord.longitude = getLast(lonring);
	//RING BUFFERの更新はしない(stack判定誤作動のため)
	return 0;
}

int updateCoord(Queue* latring,Queue* lonring)
{
	loc_t coord;
	gps_location(&coord);//gpsデータ取得
	if(coord.latitude == 0.0)
	{
		handleGpsZero(coord,latring,lonring);
	}
	else
	{
		enqueue(latring,coord.latitude); //緯度を格納
		enqueue(lonring,coord.longitude); //経度を格納
		printf("latitude:%f\nlongitude:%f\n", coord.latitude, coord.longitude);
	}
	return 0;
}

int motor_rotate_compass(double angle_to_rotate)
{
	int c = 0;  //行きたい方角に回転できなくても無限ループにならないようにカウンター用意
	double delta_angle = 180;
	double compass_angle_fixed =readCompassAngle();
	double target_angle = cal_deviated_angle(0, compass_angle_fixed + angle_to_rotate);
	while(fabs(delta_angle) > 30 && c <STACK_COUNTER)
	{
		double compass_angle =readCompassAngle();
		delta_angle= cal_delta_angle(compass_angle,target_angle);
		printf("delta_angle: %f\n", delta_angle);
		if(delta_angle>0)
		{
			motor_right(100);
		}
		else
		{
			motor_left(100);
		}
		delay(100);
		c++;
	}
	if(c >= STACK_COUNTER)
	{
		printf("could not escape\n");
	}

	motor_stop();
	delay(2000);
	return 0;
}

int motor_escape()
{
	printf("get stacked\n");
	motor_rotate_compass(90);
	motor_rotate_compass(-90);
	return 0;
}

//スタック判定をして抜け出す処理まで
int stackJudge(Queue* latring,Queue* lonring)
{
	double deltaMovement = 0;
	deltaMovement=fabs(getLast(latring)-dequeue(latring))+fabs(getLast(lonring)-dequeue(lonring));
	printf("deltaMovement = %f\n", deltaMovement);
	if(deltaMovement<STACK_THRESHOLD)
	{
		printf("STACK JUDGEMENT\n");
		motor_escape();
		//gps_off();
		//gps_init();
	}
	return 0;
}

//gpsと地磁気のデータを一回分更新し、リングバッファに格納
int updateAll(DistAngle* data,Queue* latring,Queue* lonring)
{
	printTime();
	updateCoord(latring,lonring);
	updateDistAngle(data,latring,lonring);
	//GPS_RING_LENまでリングバッファが溜まった時から随時動的にstack 判定
	if(queue_length(latring)==GPS_RING_LEN)
	{
		stackJudge(latring,lonring);
	}
	return 0;
}

//goal判定で-2を返してそれ以外は0
int Go2Goal(DistAngle* data,Queue* latring,Queue* lonring)
{
	Pid pid_data;
	pid_initialize(&pid_data);
	pid_const_initialize(&pid_data,SETPOINT,KP_VALUE,KI_VALUE,KD_VALUE);
	int i;
	for(i=0; i<PID_LEN; i++)
	{
		updateAll(data,latring,lonring);
		if(isReverse())
		{
			motor_stop();
			delay(1000);
			motor_forward(100);
			delay(2000);
		}
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
	printf("PID integral finish\n");
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
	DistAngle DistAngle_data = {0,0,100000,0};
	Queue* gps_latring = make_queue(GPS_RING_LEN);
	Queue* gps_lonring = make_queue(GPS_RING_LEN);
	while(Go2Goal(&DistAngle_data,gps_latring,gps_lonring) != -2) ;
	queue_delete(gps_lonring);
	queue_delete(gps_latring);
	return 0;
}

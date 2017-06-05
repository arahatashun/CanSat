#include <stdio.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <gps.h>
#include <wiringPi.h>
#include "compass.h"
#include "motor.h"
#include "acclgyro.h"
#include "mitibiki.h"
#include "ring_buffer.h"

static const int turn_power = 60;//turnするpower
static const int angle_of_deviation = -7; //地磁気の偏角を考慮
static const double PI = 3.14159265359;
static const int gps_ring_len = 10;//gpsのリングバッファの長さ
static const int gps_ring_len = 10;
static const double stack_threshold = 0.00003;

time_t start_time;//開始時刻のグローバル変数宣言
loc_t data;//gpsのデータを確認するものをグローバル変数宣言

Acclgyro acclgyro_data; //６軸センサーの構造体を宣言
Cmps compass_data;      //地磁気の構造体を宣言

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
	acclgyro_value_initialize(&acclgyro_data);
	compass_value_initialize(&compass_data);
	double phi_radian = 0; //ロール角のdegree表示の値を格納
	double psi_radian = 0; //ピッチ角のdegree表示の値を格納
	double phi_degree = 0;
	double psi_degree = 0;
	double y1 = 0;
	double y2 = 0;
	double x1 = 0;
	double x2 = 0;
	double x3 = 0;
	accl_and_rotation_read(&acclgyro_data);
	compass_read(&compass_data);
	acclx = (double) acclgyro_data.acclX_scaled;
	accly = (double) acclgyro_data.acclY_scaled;
	acclz = (double) acclgyro_data.acclZ_scaled;
	xcompass = (double)compass_data.compassx_value;
	ycompass = (double)compass_data.compassy_value;
	zcompass = (double)compass_data.compassz_value;
	printf("acclx = %f\n", acclx);
	printf("accly = %f\n", accly);
	printf("acclz = %f\n", acclz);
	printf("compassx = %f\n", xcompass);
	printf("compassy = %f\n", ycompass);
	printf("compassz = %f\n", zcompass);
	phi_radian = cal_roll(accly, acclz);
	psi_radian = cal_pitch(acclx, accly, acclz, phi_radian);
	phi_degree = phi_radian*180.0/PI;
	psi_degree = psi_radian*180.0/PI;
	printf("phi_degree = %f\n", phi_degree);
	printf("psi_degree = %f\n", psi_degree);
	y1 = zcompass*sin(phi_radian);
	y2 = ycompass*cos(phi_radian);
	x1 = xcompass*cos(psi_radian);
	x2 = ycompass*sin(psi_radian)*sin(phi_radian);
	x3 = zcompass*sin(psi_radian)*cos(phi_radian);
	double theta_degree1 = atan2(y1 - y2,x1 + x2 + x3)*180.0/PI;
	double theta_degree2 = cal_theta(theta_degree1);//値域が0~360になるように計算
	*theta_degree = cal_deviated_angle(theta_degree2);//偏角を調整
	printf("theta_degree = %f\n", *theta_degree);
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
	double angle_to_go = 0;//進むべき方角を代入するための変数
	angle_to_go = calc_target_angle(data.latitude,data.longitude); //緯度と経度から進むべき方角を計算
	double delta_angle = 0;//進むべき方角と現在の移動方向の差の角を代入するための変数
	double compass_angle = 0;//地磁気から今のマシンの向きを計算して代入するための変数
	compass_angle = cal_compass_theta();//地磁気と6軸の値からマシンの向いている方角を計算
	delta_angle = cal_delta_angle(compass_angle,angle_to_go);
	printf("delta_angle:%f\n",delta_angle);
	double distance = 0;
	distance = dist_on_sphere(data.latitude,data.longitude);
	double angle_to_go = 0;//進むべき方角
	angle_to_go = calc_target_angle(data.latitude,data.longitude);
	double delta_angle = 0;//進むべき方角と現在の移動方向の差の角
	double compass_angle = 0;
	cal_compass_theta(&compass_angle);
	*delta_angle= 0;
	*delta_angle = cal_delta_angle(compass_angle,angle_to_go);
	printf("delta_angle:%f\n",delta_angle);
	*distance = 0;
	*distance = dist_on_sphere(data.latitude,data.longitude);
	printf("distance :%f\n",distance);
	if(queue_length(gps_lat_ring)==10)
	{
		double delta_movement;
		delta_movement = fabs(data.latitude-dequeue(gps_lat_ring)) +
		                 fabs(data.longitude-dequeue(gps_lon_ring));
		if(delta_movement<stack_threshold)
		{
			stack_action();
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
	gps_location(&data);
	dist_to_goal =dist_on_sphere(data.latitude, data.longitude);
	if(dist_to_goal < 10) //ゴールまでの距離が10m以下なら100秒停止
		update_angle(&delta_angle,&dist_to_goal);

	if(dist_to_goal<1)
	{
		return -2;        //ゴールに着いた
	}

	delta_angle=update_angle();
	if(-180 <= delta_angle && delta_angle <= -30) //ゴールの方角がマシンから見て左に30~180度の場合は左回転
		if(-180 <= delta_angle && delta_angle <= -30)
		{
			motor_left(turn_power);
			delay(100);
			motor_stop();
			delay(1000);
		}
		else if(30 <= delta_angle && delta_angle <= 180)         //ゴールの方角がマシンから見て右に30~180度の場合は右回転
		{
			motor_right(turn_power);
			delay(100);
			motor_stop();
			delay(1000);
		}
		else
		{
			motor_forward(100);
			delay(1000);
			motor_stop();
			delay(1000);
		}
	return 0;
}

/*
   stack判定した時の行動
 */
int stack_action(double delta_movement)
{
	if(delta_movement < 0.00003)                  //ここのパラメータをいじってスタック判定
	{
		printf("get stacked");
		motor_right(100);
		delay(1000);
		motor_left(100);
		delay(1000);
		motor_forward(100);
		delay(3000);
	}
	return 0;
}

int main()
{
	int i;
	double lati, longi;
	time(&start_time);
	signal(SIGINT, handler);
	acclgyro_initializer();
	pwm_initializer();
	gps_init();
	compass_initializer();
	gps_lat_ring = make_queue(gps_ring_len);
	gps_lon_ring = make_queue(gps_ring_len);
	while(decide_route()!=-2) ;
	return 0;
}

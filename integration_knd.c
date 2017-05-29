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

//note: seikei toukei ni izon
static const int turn_power = 60;//turnするpower
static const double target_latitude = 35.715204;//ido
static const double target_longitude = 139.759198;//keido
static const int angle_of_deviation = -7;
static const double PI = 3.14159265359;
static const double convert_to_G = 16384.0;
static const double EARTH_RADIUS = 6378137;
time_t start_time;//開始時刻のグローバル変数宣言

loc_t data;//gpsのデータを確認するものをグローバル変数宣言
//デカルト座標
/*typedef struct cartesian_coordinates {
        double x;
        double y;
        double z;
   }cartesian_coord;

   cartesian_coord current_position;
   cartesian_coord target_position;*/

typedef struct GPS_stack_decide {
	double latitude;
	double longitude;
}GPS_stack;

GPS_stack GPS_value[10];

Acclgyro acclgyro_data;
Cmps compass_data;

//シグナルハンドラ
void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}
//経度と緯度をデカルト座標に変換
/*static cartesian_coord latlng_to_xyz(double lat,double lon)
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
 */
//距離を計算
/*static double dist_on_sphere(cartesian_coord target, cartesian_coord current_position)
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
        printf("distance : %f\n",distance);
        return distance;
   }*/
/*
    GPSの座標と目的地の座標から進む方向を決める
 */

/*double calc_target_angle(double lat,double lon)
   {
        double lat_offset = 0;
        double lon_offset = 0;
        double angle = 0;
        lat_offset = target_latitude - lat;
        lon_offset = target_longitude - lon;
        angle = atan2(-lon_offset,-lat_offset)*(180/PI) + 180;
        printf("GPS target_angle : %f\n",angle);
        return angle;
   }
 */

/*
   delta_angleを計算
 */
double cal_delta_angle(double going_angle_cld, double gps_angle_cld)
{
	double delta_angle_cld = 0;
	delta_angle_cld = gps_angle_cld - going_angle_cld;
	if(-360 <= delta_angle_cld && delta_angle_cld <= -180)
	{
		delta_angle_cld = 360.0 - going_angle_cld + gps_angle_cld;
	}
	else if(-180 < delta_angle_cld  && delta_angle_cld < 0)
	{
		delta_angle_cld = delta_angle_cld;
	}
	else if(0 <= delta_angle_cld && delta_angle_cld <= 180)
	{
		delta_angle_cld = delta_angle_cld;
	}
	else
	{
		delta_angle_cld = -360.0 + gps_angle_cld - going_angle_cld;
	}

	return delta_angle_cld;
}

/* 地磁気の方角を計算*/
double cal_theta(double theta_atan2)
{
	double theta;
	theta = theta_atan2;
	if(theta < 0)
	{
		theta = 360 + theta;
	}
	else
	{
		theta = theta;
	}
	return theta;
}

/*
   ロール角を計算
 */
double cal_roll(double y,double z)
{
	return atan2(y,z);
}
/*
   ピッチ角を計算
 */
double cal_pitch(double x,double y,double z,double phi)
{
	return atan2(-x, y*sin(phi) + z*cos(phi));
}

double cal_deviated_angle(double theta_degree)
{
	double true_theta = 0;
	true_theta = theta_degree + angle_of_deviation;
	if (true_theta > 360)
	{
		true_theta = true_theta - 360;
	}
	else if(true_theta<0)
	{
		true_theta = true_theta+ 360;
	}
	else
	{
		true_theta = true_theta;
	}
	return true_theta;
}

/*
   地磁気からマシンの向いている角度を計算
 */
double cal_compass_theta()
{
	double acclx = 0;
	double accly = 0;
	double acclz = 0;
	double xcompass = 0;
	double ycompass = 0;
	double zcompass = 0;
	double phi_radian = 0;
	double psi_radian = 0;
	double phi_degree = 0;
	double psi_degree = 0;
	double y1 = 0;
	double y2 = 0;
	double x1 = 0;
	double x2 = 0;
	double x3 = 0;
	double theta_degree = 0;
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
	theta_degree = atan2(y1 - y2,x1 + x2 + x3)*180.0/PI;
	theta_degree = cal_theta(theta_degree);
	theta_degree = cal_deviated_angle(theta_degree);
	printf("theta_degree = %f\n", theta_degree);
	return theta_degree;
}

double get_distance()
{
	double distance = 0;
	gps_location(&data);
	target_position = latlng_to_xyz(target_latitude,target_longitude);
	current_position = latlng_to_xyz(data.latitude, data.longitude);
	distance = dist_on_sphere(target_position,current_position);
	printf("distance :%f\n",distance);
	return distance;
}
/*
   gpsと地磁気のデータを更新する
 */
int update_angle()
{
	time_t current_time;//時間を取得
	time(&current_time);
	double delta_time = difftime(current_time,start_time);
	printf("OS timestamp:%f\n",delta_time);
	gps_location(&data);
	printf("latitude:%f\nlongitude:%f\n", data.latitude, data.longitude);
	double angle_to_go = 0;//進むべき方角
	angle_to_go = calc_target_angle(data.latitude,data.longitude);
	double delta_angle = 0;//進むべき方角と現在の移動方向の差の角
	double compass_angle = 0;
	compass_angle = cal_compass_theta();
	delta_angle = cal_delta_angle(compass_angle,angle_to_go);
	printf("delta_angle:%f\n",delta_angle);
	/*
	   目的地の方角を0として今のマシンの方角がそれからどれだけずれているかを
	   -180~180で表示 目的方角が右なら値は正とする
	 */
	double distance = 0;
	distance = dist_on_sphere(data.latitude,data.longitude);
	printf("distance :%f\n",distance);
	return delta_angle;
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
	dist_to_goal =get_distance();
	if(dist_to_goal < 5)
	{
		motor_stop();
		delay(100000);
	}

	delta_angle=update_angle();
	if(-180 <= delta_angle && delta_angle <= -30)
	{
		motor_left(turn_power);
		delay(100);
		motor_stop();
		delay(1000);
	}
	else if(30 <= delta_angle && delta_angle <= 180)
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
   GPS_の値を10回分確保
 */
int stock_GPS(int n)
{
	gps_location(&data);
	GPS_value[n].latitude = data.latitude;
	GPS_value[n].longitude = data.longitude;
	return 0;
}
/*
   stack判定用
 */
int stack_action()
{
	int c = 0;                    //stackカウンター stackしたらc=0
	int i, j;
	for(i = 0; i < 10; i++)
	{
		for(j = i+1; j<10; j++)
		{
			if(fabs(GPS_value[i].latitude-GPS_value[j].latitude) +
			   fabs(GPS_value[i].longitude-GPS_value[j].longitude) > 0.00001)
			{
				c = 1;
				goto NOSTACK;
			}
		}
	}
NOSTACK:
	if(c==0)
	{
		printf("get stacked");
		motor_right(100);
		delay(1000);
		motor_left(100);
		delay(1000);
		motor_forward(100);
		delay(3000);
	}
}

int main()
{
	int i,j;
	time(&start_time);
	signal(SIGINT, handler);
	acclgyro_initializer();
	pwm_initializer();
	gps_init();
	compass_initializer();
	while(1)
	{
		for(i = 0; i< 10; i++)
		{
			stock_GPS(i);
			decide_route();
		}
		for(i = 0; i< 10; i++)
		{
			printf("%dth latitude :%f\n", i, GPS_value[i].latitude);
			printf("%dth longitude :%f\n", i, GPS_value[i].longitude);
		}
		for(i = 0; i < 10; i++)
		{
			for(j = i+1; j<10; j++)
			{
				printf("delta_movement :%f\n", fabs(GPS_value[i].latitude-GPS_value[j].latitude) +
				       fabs(GPS_value[i].longitude-GPS_value[j].longitude));
			}
		}
		delay(1000);
		stack_action(GPS_value);
	}
	return 0;
}

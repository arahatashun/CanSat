#include <stdio.h>
#include <math.h>
#include <gps.h>
#include <wiringPi.h>
#include "mitibiki.h"
//note: seikei toukei ni izon

static const double target_latitude = 40.879715;//緯度
static const double target_longitude = -119.121768;//経度
static const double PI = 3.14159265;
static const double EARTH_RADIUS = 6378137;

//デカルト座標
typedef struct cartesian_coordinates {
	double x;
	double y;
	double z;
} cartesian_coord;

//GPSの座標と目的地の座標から進む方向を決める
double calc_target_angle(double lat,double lon)
{
	double lat_offset = 0;
	double lon_offset = 0;
	double angle = 0;
	lat_offset = target_latitude - lat;
	lon_offset = target_longitude - lon;
	angle = atan2(-lon_offset,-lat_offset)*(180/PI) + 180;
	printf("GPS_target_angle : %f\n",angle);
	return angle;
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
double dist_on_sphere(double current_lat, double current_lon)
{
	cartesian_coord target = latlng_to_xyz(target_latitude,target_longitude);
	cartesian_coord current_position = latlng_to_xyz(current_lat, current_lon);
	double dot_product_x = 0;
	double dot_product_y = 0;
	double dot_product_z = 0;
	double dot_product_sum = 0;
	double distance = 0;
	dot_product_x = target.x*current_position.x;
	dot_product_y = target.y*current_position.y;
	dot_product_z = target.z*current_position.z;
	dot_product_sum =dot_product_x+dot_product_y+dot_product_z;
	distance = fabs(acos(dot_product_sum)*EARTH_RADIUS);
	printf("distance : %f\n",distance);
	return distance;
}
/*
        マシンの方角を0として目的地の方角がそれから
        どれだけずれているかを-180~180で表示
        目的方角が右なら値は正
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

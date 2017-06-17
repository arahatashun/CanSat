#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "compass.h"
#include "motor.h"

static const int angle_of_deviation = -7.2;
static const int devid = 0x1e; //I2C address
static const int mode_reg = 0x02;
static const int mode_continuous = 0x00;
static const int mode_single = 0x01;
static const int x_msb_reg = 0x03;
static const int x_lsb_reg = 0x04;
static const int z_msb_reg = 0x05;
static const int z_lsb_reg = 0x06;
static const int y_msb_reg = 0x07;
static const int y_lsb_reg = 0x08;
static const double PI = 3.14159265;
static const double k_parameter = 1.0;//地磁気の感度補正パラメータ
static int fd = 0;
static int WPI2CWReg8 = 0;
/*
   //calibration時の回転
   static const int turn_calib_power = 25;//地磁気補正時turnするpower
   static const int turn_calib_milliseconds = 75;//地磁気補正時turnするmilliseconds
 */

int compass_initialize()
{
	//I2c setup
	fd = wiringPiI2CSetup(devid);
	if(fd == -1)
	{
		printf("WARNING! compass wiringPiI2CSetup error\n");
		printf("fd = %d, errno=%d: %s\n", fd, errno, strerror(errno));
		return -1;
	}
	else
	{
		printf("compass wiringPiI2CSetup success\n");
		printf("fd = %d, errno=%d: %s\n", fd, errno, strerror(errno));
	}
	return 0;
}

static short read_out(int file,int msb_reg, int lsb_reg)
{
	uint8_t msb = 0;
	uint8_t lsb = 0;
	short i = 0;
	msb = wiringPiI2CReadReg8(file, msb_reg);
	lsb = wiringPiI2CReadReg8(file, lsb_reg);
	i = msb << 8| lsb;
	return i;
}

int compass_read(Cmps *compass_data)
{
	WPI2CWReg8 = wiringPiI2CWriteReg8(fd,mode_reg,mode_single);
	if(WPI2CWReg8 == -1)
	{
		printf("Compass write error register mode_reg\n");
		printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
		errno = -WPI2CWReg8;
		printf("errno=%d: %s\n", errno, strerror(errno));
	}
	else
	{
		printf("Compass write register:mode_reg\n");
	}
	uint8_t status_val = wiringPiI2CReadReg8(fd, 0x09);
	printf("1st bit of status resister = %d\n", (status_val >> 0) & 0x01); //地磁気が正常ならここは1(死んでも1?)
	printf("2nd bit of status resister = %d\n", (status_val >> 1) & 0x01); //地磁気が正常ならここは0(死んだら1)
	compass_data->x_value = read_out(fd, x_msb_reg, x_lsb_reg);
	compass_data->y_value = read_out(fd, y_msb_reg, y_lsb_reg);
	compass_data->z_value = read_out(fd, z_msb_reg, z_lsb_reg);
	return 0;
}

/*地磁気のキャリブレーション補正用のログを取るためにprintをコメントアウトしたもの*/
int compass_read_scatter(Cmps *data)
{
	//WriteReg8
	WPI2CWReg8 = wiringPiI2CWriteReg8(fd,mode_reg,mode_single);
	/*if(WPI2CWReg8 == -1)
	   {
	        printf("Compass write error register mode_reg\n");
	        printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
	        errno = -WPI2CWReg8;
	        printf("errno=%d: %s\n", errno, strerror(errno));
	   }
	   else
	   {
	        printf("Compass write register:mode_reg\n");
	   }*/
	uint8_t status_val = wiringPiI2CReadReg8(fd, 0x09);
	printf("1st bit of status resister = %d\n", (status_val >> 0) & 0x01); //地磁気が正常ならここは1(死んでも1?)
	printf("2nd bit of status resister = %d\n", (status_val >> 1) & 0x01); //地磁気が正常ならここは0(死んだら1)
	data->x_value = read_out(fd, x_msb_reg, x_lsb_reg);
	data->y_value = read_out(fd, y_msb_reg, y_lsb_reg);
	data->z_value = read_out(fd, z_msb_reg, z_lsb_reg);
	return 0;
}

//NOTE printだけじゃなくて値の取得もしてる
int print_compass(Cmps *data)
{
	compass_read(data);
	printf("compassx = %f\n", data->x_value);
	printf("compassy = %f\n", data->y_value);
	printf("compassz = %f\n", data->z_value);
	return 0;
}

int compass_value_initialize(Cmps *compass_init)
{
	compass_init->x_value = 0;
	compass_init->y_value = 0;
	compass_init->z_value = 0;
}


//偏角を考慮を考慮して計算
static double cal_deviated_angle(double theta_degree)
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


//地磁気のxy座標から方角を計算
double calc_compass_angle(double x,double y)
{
	double cal_theta = 0;
	cal_theta = atan2(-y*k_parameter,x)*(180/PI);
	if(cal_theta  < -90)  //詳しい計算方法はkndまで
	{
		cal_theta = -cal_theta - 90;
	}
	else
	{
		cal_theta = 270 - cal_theta;
	}
	return cal_deviated_angle(cal_theta);
}


//6軸を用いた方角の計算
double cal_deg_acclcompass(double x, double y,double z,
                           double sin_phi, double sin_psi,
                           double cos_phi, double cos_psi)
{
	double y1 = 0;//y1~x3は見やすさと計算のために用意した物理的に意味はない変数
	double y2 = 0;
	double x1 = 0;
	double x2 = 0;
	double x3 = 0;
	double cal_theta = 0;
	y1 = z*sin_phi;
	y2 = y*cos_phi;
	x1 = x*cos_psi;
	x2 = y*sin_psi*sin_phi;
	x3 = z*sin_psi*cos_phi;
	cal_theta = atan2((y1 - y2)*k_parameter,x1 + x2 + x3)*180.0/PI;
	if(cal_theta  < -90)  //詳しい計算方法はkndまで
	{
		cal_theta = -cal_theta - 90;
	}
	else
	{
		cal_theta = 270 - cal_theta;
	}
	return cal_deviated_angle(cal_theta);
}

/*
   //以下は地磁気calibration用
   static int compass_offset_initialize(Cmps_offset *compass_offset, Cmps *compass_data)
   {
        compass_value_initialize(compass_data);
        compass_read(compass_data);
        compass_offset->compassx_offset_max = compass_data->x_value;
        compass_offset->compassx_offset_min = compass_data->x_value;
        compass_offset->compassy_offset_max = compass_data->y_value;
        compass_offset->compassy_offset_min = compass_data->y_value;
        compass_offset->compassx_offset = 0;
        compass_offset->compassy_offset = 0;
        return 0;
   }

   static int maxmin_compass(Cmps_offset *compass_offset, Cmps *compass_data)
   {
        if(compass_data->x_value > compass_offset->compassx_offset_max)
        {
                compass_offset->compassx_offset_max = compass_data->x_value;
        }
        else if(compass_data->x_value < compass_offset->compassx_offset_min)
        {
                compass_offset->compassx_offset_min = compass_data->x_value;
        }

        if(compass_data->y_value > compass_offset->compassy_offset_max)
        {
                compass_offset->compassy_offset_max = compass_data->y_value;
        }
        else if(compass_data->y_value < compass_offset->compassy_offset_min)
        {
                compass_offset->compassy_offset_min = compass_data->y_value;
        }
        return 0;
   }

   static int mean_compass_offset(Cmps_offset *compass_offset)
   {
        compass_offset->compassx_offset = (compass_offset->compassx_offset_max + compass_offset->compassx_offset_min)/2;
        compass_offset->compassy_offset = (compass_offset->compassy_offset_max + compass_offset->compassy_offset_min)/2;
        printf("x_offset=%f, y_offset=%f\n", compass_offset->compassx_offset
               ,compass_offset->compassy_offset);
        return 0;
   }

   static int rotate_to_calib(Cmps *compass_data)
   {
        compass_value_initialize(compass_data);
        motor_right(turn_calib_power);
        delay(turn_calib_milliseconds);
        motor_stop();
        delay(2000);
        compass_read(compass_data);
        printf( "compass_x= %f, compass_y= %f\n",compass_data->x_value
                ,compass_data->y_value);
        delay(50);
        return 0;
   }

   int cal_maxmin_compass(Cmps_offset *compass_offset,Cmps *compass_data)
   {
        int i = 0;
        compass_offset_initialize(compass_offset,compass_data);
        for(i = 0; i<25; i++)
        {
                rotate_to_calib(compass_data);
                maxmin_compass(compass_offset,compass_data);
        }
        mean_compass_offset(compass_offset);
        return 0;
   }*/

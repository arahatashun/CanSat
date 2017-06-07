#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "compass.h"
#include  "motor.h"

static const int angle_of_deviation = -7;
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
static int fd = 0;
static int WPI2CWReg8 = 0;

//calibration時の回転
static const int turn_calib_power = 40;//地磁気補正時turnするpower
static const int turn_calib_milliseconds = 100;//地磁気補正時turnするmilliseconds

int compass_initializer()
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
	//WriteReg8
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

	short x = 0;
	short y = 0;
	short z = 0;
	compass_data->compassx_value = read_out(fd, x_msb_reg, x_lsb_reg);
	compass_data->compassy_value = read_out(fd, y_msb_reg, y_lsb_reg);
	compass_data->compassz_value = read_out(fd, z_msb_reg, z_lsb_reg);
	return 0;
}

int print_compass(Cmps *compass_data)
{
	compass_read(compass_data);
	printf("compassx = %f\n", compass_data->compassx_value);
	printf("compassy = %f\n", compass_data->compassy_value);
	printf("compassz = %f\n", compass_data->compassz_value);
	return 0;
}

int compass_value_initialize(Cmps *compass_init)
{
	compass_init->compassx_value = 0;
	compass_init->compassy_value = 0;
	compass_init->compassz_value = 0;
}

static double calc_compass_angle(short x,short y)
{
	double angle_calc1 = 0;
	double angle_calc2 = 0;
	double angle_return = 0;
	angle_calc1 = atan2((double)-y, (double)-x)*(180/PI) + 180;
	angle_calc2 = angle_calc1 + angle_of_deviation;
	if (angle_calc2 > 360)
	{
		angle_return = angle_calc2 - 360;
	}
	else if(angle_calc2<0)
	{
		angle_return = angle_calc2 + 360;
	}
	else
	{
		angle_return = angle_calc2;
	}
	return angle_return;
}

//ポインタで角度を渡す
int compass_get_angle(double *compass_angle)
{
	//WriteReg8
	WPI2CWReg8 = wiringPiI2CWriteReg8(fd,mode_reg,mode_single);
	if(WPI2CWReg8 == -1)
	{
		printf("write error register mode_reg\n");
		printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
		errno = -WPI2CWReg8;
		printf("errno=%d: %s\n", errno, strerror(errno));
	}
	else
	{
		printf("write register:mode_reg\n");
	}
	short x = 0;
	short y = 0;
	short z = 0;
	double angle = 0;
	x = read_out(fd, x_msb_reg, x_lsb_reg);
	y = read_out(fd, y_msb_reg, y_lsb_reg);
	z = read_out(fd, z_msb_reg, z_lsb_reg);
	*compass_angle = calc_compass_angle(x,y);
	printf("COMPASS x:%d,y:%d,z:%d,angle:%f\n",x,y,z,*compass_angle);
	return 0;
}


/*
   integrationで6軸から地磁気の向きを出すときの調整
 */
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
   偏角を考慮を考慮して計算
 */
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

double cal_deg_acclcompass(double compassx_value, double compassy_value,
                           double compassz_value, double sin_phi, double sin_psi,
                           double cos_phi, double cos_psi)
{
	double y1 = 0;//y1~x3は見やすさと計算のために用意した物理的に意味はない変数
	double y2 = 0;
	double x1 = 0;
	double x2 = 0;
	double x3 = 0;

	y1 = compassz_value*sin_phi;
	y2 = compassy_value*cos_phi;
	x1 = compassx_value*cos_psi;
	x2 = compassy_value*sin_psi*sin_phi;
	x3 = compassz_value*sin_psi*cos_phi;
	return atan2(y1 - y2,x1 + x2 + x3)*180.0/PI;
}

//以下は地磁気calibration用
int compass_offset_initialize(Cmps_offset *compass_offset)
{
	compass_offset->compassx_offset_max = 0;
	compass_offset->compassx_offset_min = 0;
	compass_offset->compassy_offset_max = 0;
	compass_offset->compassy_offset_min = 0;
	compass_offset->compassz_offset_max = 0;
	compass_offset->compassz_offset_min = 0;
	compass_offset->compassx_offset = 0;
	compass_offset->compassx_offset = 0;
	compass_offset->compassy_offset = 0;
	return 0;
}

static int rotate_to_calib(Cmps *compass_data)
{
	compass_value_initialize(compass_data);
	motor_right(turn_calib_power);
	delay(turn_calib_milliseconds);
	motor_stop();
	compass_read(compass_data);
	printf( "compass_x= %f, compass_y= %f, compass_z=%f",compass_data->compassx_value
	        ,compass_data->compassy_value
	        ,compass_data->compassz_value);
	return 0;
}

int cal_maxmin_compass(Cmps_offset *compass_offset,Cmps *compass_data)
{
	compass_offset_initialize(compass_offset);
	for(i = 0; i<10; i++)
	{
		rotate_to_calib(ompass_data);
		maxmin_compass(compass_offset,compass_data);
	}
	mean_compass_offset(compass_offset);
	return 0;
}

static int maxmin_compass(Cmps *compass_offset, Cmps *compass_data)
{
	if(compass_data->compassx_value > compass_offset->compassx_offset_max)
	{
		compass_offset->compassx_offset_max = compass_data->compassx_value;
	}
	else if(compass_data->compassx_value < compass_offset->compassx_offset_min)
	{
		compass_offset->compassx_offset_min = compass_data->compassx_value;
	}
	if(compass_data->compassy_value > compass_offset->compassy_offset_max)
	{
		compass_offset->compassy_offset_max = compass_data->compassy_value;
	}
	else if(compass_data->compassy_value < compass_offset->compassy_offset_min)
	{
		compass_offset->compassy_offset_min = compass_data->compassy_value;
	}
	if(compass_data->compassz_value > compass_offset->compassz_offset_max)
	{
		compass_offset->compassz_offset_max = compass_data->compassz_value;
	}
	else if(compass_data->compassz_value < compass_offset->compassz_offset_min)
	{
		compass_offset->compassz_offset_min = compass_data->compassz_value;
	}
	return 0;
}

static int mean_compass_offset(Cmps *compass_offset)
{
	compass_offset->compassx_offset = (compass_offset->compassx_offset_max + compass_offset->compassx_offset_min)/2;
	compass_offset->compassy_offset = (compass_offset->compassy_offset_max + compass_offset->compassy_offset_min)/2;
	compass_offset->compassz_offset = (compass_offset->compassz_offset_max + compass_offset->compassz_offset_min)/2;
	printf("x_offset=%f, y_offset=%f, z_offset=%f", compass_offset->compassx_offset
	       ,compass_offset->compassy_offset
	       ,compass_offset->compassz_offset);
	return 0;
}

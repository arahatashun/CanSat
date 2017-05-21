#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "compass.h"

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
int fd = 0;
int WPI2CWReg8 = 0;

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

int compass_initializer()
{
	/* WHO AM I */
	fd = wiringPiI2CSetup(devid);
	if(fd == -1)
	{
		printf("WARNING! compass wiringPiI2CSetup error\n");
		printf("fd = %d, errno=%d: %s\n", fd, errno, strerror(errno));
		return -1;
	}
	else
	{
			printf("wiringPiI2CSetup success\n");
	}

	/* start senser */
	WPI2CWReg8 = wiringPiI2CWriteReg8(fd,mode_reg,mode_continuous);
	if(WPI2CWReg8 == -1)
	{
		printf("write error register mode_reg\n");
		printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
		return -1;
	}
	else
	{
		printf("write register:mode_reg\n");
	}
	return 0;
}

int compass_get_angle(double *compass_angle)
//ポインタで角度を渡す
{
	/* read X_MSB */
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

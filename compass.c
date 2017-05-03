#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

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
static const double pi = 3.14159265;

short read_out(int file,int msb_reg, int lsb_reg)
{
	uint8_t msb = wiringPiI2CReadReg8(file, msb_reg);
	uint8_t lsb = wiringPiI2CReadReg8(file, lsb_reg);
	short i = msb << 8| lsb;

	return i;
}

double get_angle(short x,short y)
{
	double angle_calc;

	if((double)x < -DBL_EPSILON) {
		angle_calc = 90 + atan2((double)y,(double)x)*(180/pi);
	}
	if((double)x > DBL_EPSILON) {
		angle_calc = 270 + atan2((double)y,(double)x)*(180/pi);
	}
	else{
		angle_calc = 0;
	}

	return angle_calc;
}

int main()
{
/* WHO AM I */
	int fd = wiringPiI2CSetup(devid);
/* start senser */
	if((wiringPiI2CWriteReg8(fd,mode_reg,mode_continuous))<0) {
		printf("write error register mode_reg");
	}
	printf("write register:mode_reg");

/* read X_MSB */
	short x = read_out(fd, x_msb_reg, x_lsb_reg);
	short y = read_out(fd, y_msb_reg, y_lsb_reg);
	short z = read_out(fd, z_msb_reg, z_lsb_reg);
//arctan(x/y)*pi/180
	double angle=get_angle(x,y);

	printf("x:%d,y:%d,z:%d -angle %f¥n",x,y,z,angle);
	return 0;
}

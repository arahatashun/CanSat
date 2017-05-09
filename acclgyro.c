// acclgyro.c

#include <stdio.h>
#include <math.h>
#include <wiringPiI2C.h>
#include "acclgyro.h"

typedef struct acclgyro{
	double acclX_scaled, acclY_scaled, acclZ_scaled;	//the values of accleration 
	double gyroX_scaled, gyroY_scaled, gyroZ_scaled;    //the values of gyroscope
	double x_rotation,y_rotation;    //XY-axis rotation
	} Acclgyro;
	
int read_word_2c(int addr); 

/*   
double dist(double a,double b);
double get_y_rotation(double x,double y,double z);
double get_x_rotation(double x,double y,double z);
*/

void accl_and_rotation_read(Acclgyro *data);    //acgは構造体オブジェクトをさすポインタ
void gyro_read(Acclgyro *data);

void set_acclgyro(Acclgyro *data);    //integrate accl_read,gyro_read,rotation_read
void print_acclgyro(Acclgyro *data);    //print acclgyro parameter

static int fd;
static int acclX, acclY, acclZ;
static int gyroX, gyroY, gyroZ;

static const int devid = 0x68;    //I2C adress manual p45
static const int power_management_reg = 0x6B;    //manual p40

static const int acclX_reg = 0x3B;    //manual p7
static const int acclY_reg = 0x3D;
static const int acclZ_reg = 0x3F;
static const int gyroX_reg = 0x43;    //manual p7
static const int gyroY_reg = 0x45;
static const int gyroZ_reg = 0x47;

static const double convert_to_G = 16384.0;
static const double convert_to_degpers = 131.0;

int read_word_2c(int addr)
{
	int val;
	val = wiringPiI2CReadReg8(fd, addr);
	val = val << 8;
	val += wiringPiI2CReadReg8(fd, addr+1); //隣り合う2つのレジスタの数値を足し合わせる
	if (val >= 0x8000) val = -(65536 - val); //0x8000=32768以上になったら値を減らしていく
	return val;
}
/*
double dist(double a, double b)
{
	return sqrt((a*a) + (b*b));
}

double get_y_rotation(double x, double y, double z)
{
	double radians;
	radians = atan2(x, dist(y, z));
	return -(radians * (180.0 / M_PI));
}

double get_x_rotation(double x, double y, double z)
{
	double radians;
	radians = atan2(y, dist(x, z));
	return (radians * (180.0 / M_PI));
}
*/

void accl_and_rotation_read(Acclgyro *data)
{
	int acclX = read_word_2c(acclX_reg);
	int acclY = read_word_2c(acclY_reg);
	int acclZ = read_word_2c(acclZ_reg);

	data->acclX_scaled = acclX / convert_to_G;
	data->acclY_scaled = acclY / convert_to_G;
	data->acclZ_scaled = acclZ / convert_to_G;
/*
	data->x_rotation = get_x_rotation(acg->acclX_scaled, acg->acclY_scaled, acg->acclZ_scaled);
	data>y_rotation = get_y_rotation(acg->acclX_scaled, acg->acclY_scaled, acg->acclZ_scaled);
*/
}


void gyro_read(Acclgyro *data)
{
	int gyroX = read_word_2c(gyroX_reg);
	int gyroY = read_word_2c(gyroY_reg);
	int gyroZ = read_word_2c(gyroZ_reg);
	data->gyroX_scaled = gyroX / convert_to_degpers;
	data->gyroY_scaled = gyroY / convert_to_degpers;
	data->gyroZ_scaled = gyroZ / convert_to_degpers;
}


void acclgyro_initializer()
{
	fd = wiringPiI2CSetup(devid);
	wiringPiI2CWriteReg8(fd,power_management_reg,0x00); //disable sleep mode
}

void set_acclgyro(Acclgyro *data)
{
	//set value
	accl_and_rotation_read(data);
	gyro_read(data);
}

/*
void print_acclgyro(Acclgyro *data)
{
	set_acclgyro(acg);
	printf("acclX_scaled: %f\n", data->acclX_scaled);
	printf("acclY_scaled: %f\n", data->acclY_scaled);
	printf("acclZ_scaled: %f\n", data->acclZ_scaled);
	printf("X rotation: %f\n", data->x_rotation);
	printf("Y rotation: %f\n", data->y_rotation);
	printf("gyroX_scaled: %f\n", data->gyroX_scaled);
	printf("gyroY_scaled: %f\n", data->gyroY_scaled);
	printf("gyroZ_scaled: %f\n", data->gyroZ_scaled);
}
*/

//if reverse,return 1
int z_posture(Acclgyro *data)
{
	int p;
	set_acclgyro(data);
	if(acg->acclZ_scaled < 0)
	{
		printf("G:%f z_posture:reverse\n",data->acclZ_scaled);
		return 1;
	}
	else
	{
		printf("G:%f z_posture:normal\n",data->acclZ_scaled);
		return 0;
	}
}
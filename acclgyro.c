#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "acclgyro.h"

static const int devid = 0x68;    //I2C adress manual p45
static const int power_management_reg = 0x6B;    //manual p40
static const int mode_continuous = 0x00;
static const int mode_single = 0x01;
static const int acclX_reg = 0x3B;    //manual p7
static const int acclY_reg = 0x3D;
static const int acclZ_reg = 0x3F;
static const int gyroX_reg = 0x43;    //manual p7
static const int gyroY_reg = 0x45;
static const int gyroZ_reg = 0x47;
static const double convert_to_G = 16384.0;
static const double convert_to_degpers = 131.0;
static int fd = 0;
static int WPI2CWReg8 = 0;

//関数プロトタイプ宣言(static)
static int read_word_2c(int addr);
static double dist(double a,double b);
static double get_y_rotation(double x,double y,double z);
static double get_x_rotation(double x,double y,double z);
int accl_and_rotation_read(Acclgyro *acclgyro_data);    //acgは構造体オブジェクトをさすポインタ
int gyro_read(Acclgyro *acclgyro_data);
static int set_acclgyro(Acclgyro *acclgyro_data);    //integrate accl_read,gyro_read,rotation_read

/*
   ６軸センサー初期化
 */
int acclgyro_initializer()
{
	fd = wiringPiI2CSetup(devid);
	if(fd == -1)
	{
		printf("WARNING! acclgyro wiringPiI2CSetup error\n");
		printf("fd = %d, errno=%d: %s\n", fd, errno, strerror(errno));
		return -1;
	}
	else
	{
		printf("acclgyro wiringPiI2CSetup success\n");
		printf("fd = %d, errno=%d: %s\n", fd, errno, strerror(errno));
	}
	return 0;
}

static int read_word_2c(int addr)  //レジスタの値を読み取る
{
	int val = 0;
	val = wiringPiI2CReadReg8(fd, addr);
	val = val << 8;
	val += wiringPiI2CReadReg8(fd, addr+1); //隣り合う2つのレジスタの数値を足し合わせる
	if (val >= 0x8000) val = -(65536 - val); //0x8000=32768以上になったら値を減らしていく
	return val;
}


static double dist(double a, double b)
{
	return sqrt((a*a) + (b*b));
}

static double get_y_rotation(double x, double y, double z)
{
	double radians;
	radians = atan2(x, dist(y, z));
	return -(radians * (180.0 / M_PI));
}

static double get_x_rotation(double x, double y, double z)
{
	double radians;
	radians = atan2(y, dist(x, z));
	return (radians * (180.0 / M_PI));
}


int accl_and_rotation_read(Acclgyro *acclgyro_data)  //加速度とx,y方向の回転角を読む
{
	WPI2CWReg8 = wiringPiI2CWriteReg8(fd,power_management_reg,mode_single);
	if(WPI2CWReg8 == -1)
	{
		printf("acclgyro write error register power_management_reg\n");
		printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
		errno = -WPI2CWReg8;
		printf("errno=%d: %s\n", errno, strerror(errno));
	}
	else
	{
		printf("acclgyro write register:power_management_reg\n");
	}
	int acclX = 0;
	int acclY = 0;
	int acclZ = 0;
	acclX = read_word_2c(acclX_reg);
	acclY = read_word_2c(acclY_reg);
	acclZ = read_word_2c(acclZ_reg);

	acclgyro_data->acclX_scaled = acclX / convert_to_G;
	acclgyro_data->acclY_scaled = acclY / convert_to_G;
	acclgyro_data->acclZ_scaled = acclZ / convert_to_G;

	acclgyro_data->x_rotation = get_x_rotation(acclgyro_data->acclX_scaled, acclgyro_data->acclY_scaled, acclgyro_data->acclZ_scaled);
	acclgyro_data->y_rotation = get_y_rotation(acclgyro_data->acclX_scaled, acclgyro_data->acclY_scaled, acclgyro_data->acclZ_scaled);

	return 0;
}

/*
   データが格納されているAcclgyro型の構造体acclgyro_dataにアクセス
 */
int gyro_read(Acclgyro *acclgyro_data)
{
	WPI2CWReg8 = wiringPiI2CWriteReg8(fd,power_management_reg,mode_single);
	if(WPI2CWReg8 == -1)
	{
		printf("acclgyro write error register power_management_reg\n");
		printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
		errno = -WPI2CWReg8;
		printf("errno=%d: %s\n", errno, strerror(errno));
	}
	else
	{
		printf("acclgyro write register:power_management_reg\n");
	}
	int gyroX=0;
	int gyroY=0;
	int gyroZ=0;
	gyroX = read_word_2c(gyroX_reg);
	gyroY = read_word_2c(gyroY_reg);
	gyroZ = read_word_2c(gyroZ_reg);
	acclgyro_data->gyroX_scaled = gyroX / convert_to_degpers;
	acclgyro_data->gyroY_scaled = gyroY / convert_to_degpers;
	acclgyro_data->gyroZ_scaled = gyroZ / convert_to_degpers;
}

static int set_acclgyro(Acclgyro *acclgyro_data)  //acclgyroの値を全て読み取る
{
	//set value
	accl_and_rotation_read(acclgyro_data);
	gyro_read(acclgyro_data);
	return 0;
}

int print_acclgyro(Acclgyro *acclgyro_data) //六軸センサーの値を画面に出力
{
	set_acclgyro(acclgyro_data);
	printf("acclX_scaled: %f\n", acclgyro_data->acclX_scaled);
	printf("acclY_scaled: %f\n", acclgyro_data->acclY_scaled);
	printf("acclZ_scaled: %f\n", acclgyro_data->acclZ_scaled);
	printf("X rotation: %f\n", acclgyro_data->x_rotation);
	printf("Y rotation: %f\n", acclgyro_data->y_rotation);
	printf("gyroX_scaled: %f\n", acclgyro_data->gyroX_scaled);
	printf("gyroY_scaled: %f\n", acclgyro_data->gyroY_scaled);
	printf("gyroZ_scaled: %f\n", acclgyro_data->gyroZ_scaled);
	return 0;
}

//if reverse,return 1
int is_reverse(Acclgyro *acclgyro_data)
{
	set_acclgyro(acclgyro_data);
	if(acclgyro_data->acclZ_scaled < 0)
	{
		printf("G:%f z_posture:reverse\n",acclgyro_data->acclZ_scaled);
		return 1;
	}
	else
	{
		printf("G:%f z_posture:normal\n",acclgyro_data->acclZ_scaled);
		return 0;
	}
}

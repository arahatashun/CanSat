#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "acclgyro.h"

static const int MPU6050_ADDRESS = 0x68;
static const int POWER_MANAGEMENT_REG = 0x6B;//manual p40
static const int MODE_CONTINUOUS = 0x00;
static const int MODE_SINGLE = 0x01;
//各々のデータのアドレス
static const int ACCLX_REG = 0x3B;//manual p7
static const int ACCLY_REG = 0x3D;
static const int ACCLZ_REG = 0x3F;
static const int GYROX_REG = 0x43;//manual p7
static const int GYROY_REG = 0x45;
static const int GYROZ_REG = 0x47;
//取得した加速度値を分解能で割って加速度(G)に変換する
static const double CONVERT2G = 16384.0;
static const double CONVERT2DEGREES = 131.0;
static int fd = 0;

int acclGyro_initialize(void)
{
	fd = wiringPiI2CSetup(MPU6050_ADDRESS);
	if(fd == -1)
	{
		printf("WARNING! acclGyro wiringPiI2CSetup error\n");
		printf("fd = %d, errno=%d: %s\n", fd, errno, strerror(errno));
		return -1;
	}
	else
	{
		printf("acclGyro wiringPiI2CSetup success\n");
	}
	int WPI2CWReg8 = wiringPiI2CWriteReg8(fd,MODE_REG,MODE_CONTINUOUS);
	if(WPI2CWReg8 == -1)
	{
		printf("compass write error register MODE_CONTINUOUS\n");
		printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
		printf("errno=%d: %s\n", errno, strerror(errno));
	}
	return 0;
}

//ロック対策用の関数
static int acclGyroModeChange()
{
	int WPI2CWReg8 = wiringPiI2CWriteReg8(fd,POWER_MANAGEMENT_REG,MODE_SINGLE);
	if(WPI2CWReg8 == -1)
	{
		printf("acclGyro write error register POWER_MANAGEMENT_REG\n");
		printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
		printf("errno=%d: %s\n", errno, strerror(errno));
	}
	WPI2CWReg8 = wiringPiI2CWriteReg8(fd,POWER_MANAGEMENT_REG,MODE_CONTINUOUS);
	if(WPI2CWReg8 == -1)
	{
		printf("acclGyro write error register POWER_MANAGEMENT_REG\n");
		printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
		printf("errno=%d: %s\n", errno, strerror(errno));
	}
	return 0;
}

static short read_out(int addr)  //レジスタの値を読み取る
{
	acclGyroModeChange();//ロック対策用の関数
	uint8_t msb = 0;
	msb = wiringPiI2CReadReg8(fd, addr);
	uint8_t lsb = 0;
	lsb = wiringPiI2CReadReg8(fd, addr+1);
	short value = 0;
	value = msb << 8 | lsb;
	return value;
}

//short型用の比較関数
static int sCmp (const void* p, const void* q)
{
	return *(short*)p - *(short*)q;
}

//角速度を測定する
int readGyro(Gyro *data)
{
	short xList[10] = {};//0で初期化
	short yList[10] = {};
	short zList[10] = {};
	int i;
	for(i=0; i<10; i++)
	{
		xList[i] = read_out(GYROX_REG);
		yList[i] = read_out(GYROY_REG);
		zList[i] = read_out(GYROZ_REG);
	}
	qsort(xList,10, sizeof(short), sCmp);
	qsort(yList,10, sizeof(short), sCmp);
	qsort(zList,10, sizeof(short), sCmp);
	data->gyroX_scaled = xList[4] / CONVERT2DEGREES;//中央値を取る
	data->gyroY_scaled = yList[4] / CONVERT2DEGREES;
	data->gyroZ_scaled = zList[4] / CONVERT2DEGREES;
	return 0;
}

int readAccl(Accl*data)
{
	short xList[10] = {};//0で初期化
	short yList[10] = {};
	short zList[10] = {};
	int i;
	for(i=0; i<10; i++)
	{
		xList[i] = read_out(ACCLX_REG);
		yList[i] = read_out(ACCLY_REG);
		zList[i] = read_out(ACCLZ_REG);
	}
	qsort(xList,10, sizeof(short), sCmp);
	qsort(yList,10, sizeof(short), sCmp);
	qsort(zList,10, sizeof(short), sCmp);
	data->acclX_scaled = xList[4] / CONVERT2G;
	data->acclY_scaled = yList[4] / CONVERT2G;
	data->acclZ_scaled = zList[4] / CONVERT2G;
	return 0;
}

//if reverse,return -1
int isReverse(void)
{
	Accl data;
	readAccl(&data);
	if(data.acclZ_scaled < -0.2)
	{
		printf("G:%f z_posture:reverse\n",data.acclZ_scaled);
		return -1;
	}
	else
	{
		printf("G:%f z_posture:normal\n",data.acclZ_scaled);
		return 0;
	}
}


//ロール角を計算
double cal_roll(Accl* data)
{
	double phi = atan2(data->acclY_scaled,data->acclZ_scaled);
	return phi;
}

//ピッチ角を計算
double cal_pitch(Accl* data)
{
	double phi = cal_roll(data);
	double psi = atan2(-data->acclX_scaled,
	                   data->acclY_scaled*sin(phi)+data->acclZ_scaled*cos(phi));
	return psi;
}

//XとYの加速度の大きさを計算
static double calc_norm (Accl* data)
{
	return sqrt((data->acclX_scaled*data->acclX_scaled) +
	            (data->acclY_scaled*data->acclY_scaled));
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
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
static const double REVERSE_THRESHOLD = -0.8;//TODO 調整
static const int LOCK_MAX = 5;
static int fd = 0;

//accl raw data格納
typedef struct accl_raw {
	short xList[10];
	short yList[10];
	short zList[10];
} Accl_Raw;

//gyro raw data格納
typedef struct Gyro_raw {
	short xList[10];
	short yList[10];
	short zList[10];
} Gyro_Raw;


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
	int WPI2CWReg8 = wiringPiI2CWriteReg8(fd,POWER_MANAGEMENT_REG,MODE_CONTINUOUS);
	if(WPI2CWReg8 == -1)
	{
		printf("acclGyro write error register MODE_CONTINUOUS\n");
		printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
		printf("errno=%d: %s\n", errno, strerror(errno));
	}
	return 0;
}

/*
   //ロック対策用の関数
   static int acclGyro_mode_change()
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
 */

static short read_out(int addr)  //レジスタの値を読み取る
{
	uint8_t msb = 0;
	uint8_t lsb = 0;
	msb = wiringPiI2CReadReg8(fd, addr);
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
static int GyroReadRaw(Gyro_Raw *data)
{
	int i;
	for(i=0; i<10; i++)
	{
		data->xList[i] = read_out(GYROX_REG);
		data->yList[i] = read_out(GYROY_REG);
		data->zList[i] = read_out(GYROZ_REG);
		delay(10);
	}
	qsort(data->xList,10, sizeof(short), sCmp);
	qsort(data->yList,10, sizeof(short), sCmp);
	qsort(data->zList,10, sizeof(short), sCmp);
	return 0;
}

static int AcclReadRaw(Accl_Raw *data)
{
	int i;
	for(i=0; i<10; i++)
	{
		data->xList[i] = read_out(ACCLX_REG);
		data->yList[i] = read_out(ACCLY_REG);
		data->zList[i] = read_out(ACCLZ_REG);
		delay(10);
	}
	qsort(data->xList,10, sizeof(short), sCmp);
	qsort(data->yList,10, sizeof(short), sCmp);
	qsort(data->zList,10, sizeof(short), sCmp);
	return 0;
}

//acclgyro-1にLockした時に使う
static int handleAcclErrorOne(Accl_Raw* accl_raw)
{
	acclGyro_initialize();//NOTE initialize
	printf("acclGyro reinitialized\n");
	AcclReadRaw(accl_raw);
	printf("\n");
	return 0;
}
//acclgyro-1にLockした時に使う
static int handleGyroErrorOne(Gyro_Raw* gyro_raw)
{
	acclGyro_initialize();//NOTE initialize
	printf("acclGyro reinitialized\n");
	GyroReadRaw(gyro_raw);
	printf("\n");
	return 0;
}

//lock用、指定した値にlockされてたらreturn1する
static int checkLock(short* values,const int lock)
{
	int len = 10; //配列の要素数を取得
	int lock_count = 0;
	int i;
	for (i = 0; i < len; i++)
	{
		if (values[i] ==lock) lock_count++;
	}
	if (lock_count == len) return 1;
	return 0;
}

int Accl_read(Accl* data)
{
	Accl_Raw rawdata;
	AcclReadRaw(&rawdata);
	int LockCounter = 0;
	while((checkLock(rawdata.xList,-1)||checkLock(rawdata.yList,-1)||checkLock(rawdata.zList,-1))&&(LockCounter<100))
	{
		printf("WARNING accl -1 lock\n");
		printf("LockCounter %d\n",LockCounter);
		handleAcclErrorOne(&rawdata);
		LockCounter++;
	}
	while((checkLock(rawdata.xList,rawdata.xList[0])&&checkLock(rawdata.yList,rawdata.yList[0])&&checkLock(rawdata.zList,rawdata.zList[0]))&&(LockCounter<100))
	{
		printf("WARNING accl lock\n");
		printf("LockCounter %d\n",LockCounter);
		handleAcclErrorOne(&rawdata);
		LockCounter++;
	}

	if(LockCounter>=LOCK_MAX)
	{
		printf("Lock Counter Max\n");
		data->acclX_scaled = (double)rawdata.xList[4]/CONVERT2G;
		data->acclY_scaled = (double)rawdata.yList[4]/CONVERT2G;
		data->acclZ_scaled = 1;
	}

	data->acclX_scaled = (double)rawdata.xList[4]/CONVERT2G;
	data->acclY_scaled = (double)rawdata.yList[4]/CONVERT2G;
	data->acclZ_scaled = (double)rawdata.zList[4]/CONVERT2G;
	return 0;
}

int Gyro_read(Gyro* data)
{
	Gyro_Raw rawdata;
	GyroReadRaw(&rawdata);
	int LockCounter = 0;
	while((checkLock(rawdata.xList,-1)||checkLock(rawdata.yList,-1)||checkLock(rawdata.zList,-1))&&(LockCounter<100))
	{
		printf("WARNING gyro -1 lock\n");
		printf("LockCounter %d\n",LockCounter);
		handleGyroErrorOne(&rawdata);
		LockCounter++;
	}
	while((checkLock(rawdata.xList,rawdata.xList[0])&&checkLock(rawdata.yList,rawdata.yList[0])&&checkLock(rawdata.zList,rawdata.zList[0]))&&(LockCounter<100))
	{
		printf("WARNING gyro lock\n");
		printf("LockCounter %d\n",LockCounter);
		handleGyroErrorOne(&rawdata);
		LockCounter++;
	}

	if(LockCounter>=100)
	{
		printf("Lock Counter Max\n");
		;      //TODO 再起動?
	}
	data->gyroX_scaled = (double)rawdata.xList[4]/CONVERT2G;
	data->gyroY_scaled = (double)rawdata.yList[4]/CONVERT2G;
	data->gyroZ_scaled = (double)rawdata.zList[4]/CONVERT2G;
	return 0;
}

//if reverse,return 1
int isReverse(void)
{
	Accl data;
	Accl_read(&data);
	if(data.acclZ_scaled < REVERSE_THRESHOLD)
	{
		printf("G:%f z_posture:reverse\n",data.acclZ_scaled);
		return 1;
	}
	else
	{
		printf("G:%f z_posture:normal\n",data.acclZ_scaled);
		return 0;
	}
}

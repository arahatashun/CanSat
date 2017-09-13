#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "compass.h"
#include "motor.h"

static const double ANGLE_OF_DEVIATION = 13.5;
static const int HMC5883L_ADDRESS = 0x1e; //I2C address
static const int MODE_REG = 0x02;
static const int MODE_CONTINUOUS = 0x00;
static const int MODE_SINGLE = 0x01;
static const int X_MSB_REG = 0x03;
static const int X_LSB_REG = 0x04;
static const int Z_MSB_REG = 0x05;
static const int Z_LSB_REG = 0x06;
static const int Y_MSB_REG = 0x07;
static const int Y_LSB_REG = 0x08;
static const double PI = 3.14159265;

///キャリブレーション関係のパラメーター
static const double K_PARAMETER = 1.0;//地磁気の感度補正パラメータ

/*************
calibrationするとき
*************/
/*
static const double COMPASS_X_OFFSET = 0;
static const double COMPASS_Y_OFFSET = 0;
*/
/*************
ver3.1のoffset
*************/

static const double COMPASS_X_OFFSET = 2.5;
static const double COMPASS_Y_OFFSET = 116.0;

/*************
ver3.3のoffset
*************/
/*
static const double COMPASS_X_OFFSET = ;
static const double COMPASS_Y_OFFSET = ;
*/

//周囲に強磁場がある時の退避
static const int MAX_PWM_VAL = 100;
static const int ESCAPE_TIME = 1000;

//calibration時の回転
static const int TURN_CALIB_POWER = 25;//地磁気補正時turnするpower
static const int TURN_CALIB_MILLISECONDS = 75;//地磁気補正時turnするmilliseconds

static int fd = 0;

//compass raw data格納
typedef struct raw {
	short xList[10];
	short yList[10];
	short zList[10];
} Raw;

typedef struct cmps {
	double x_value;//the values of compassx
	double y_value;
	double z_value;
} Cmps;

//構造体の初期化
static int compass_value_initialize(Cmps *compass_init)
{
	compass_init->x_value = 0;
	compass_init->y_value = 0;
	compass_init->z_value = 0;
	return 0;
}

int compass_initialize()
{
	//I2c setup
	fd = wiringPiI2CSetup(HMC5883L_ADDRESS);
	if(fd == -1)
	{
		printf("WARNING! compass wiringPiI2CSetup error\n");
		printf("fd = %d, errno=%d: %s\n", fd, errno, strerror(errno));
		return -1;
	}
	else
	{
		printf("compass wiringPiI2CSetup success\n");
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

/*
   //地磁気ロック対策のmode_change関数(error時のみ表示)
   static int compass_mode_change()
   {
        int WPI2CWReg8 = wiringPiI2CWriteReg8(fd,MODE_REG,MODE_SINGLE);
        if(WPI2CWReg8 == -1)
        {
                printf("compass write error register MODE_SINGLE\n");
                printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
                printf("errno=%d: %s\n", errno, strerror(errno));
        }
        WPI2CWReg8 = wiringPiI2CWriteReg8(fd,MODE_REG,MODE_CONTINUOUS);
        if(WPI2CWReg8 == -1)
        {
                printf("compass write error register MODE_CONTINUOUS\n");
                printf("wiringPiI2CWriteReg8 = %d\n", WPI2CWReg8);
                printf("errno=%d: %s\n", errno, strerror(errno));
        }
        return 0;
   }
 */

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


//short型用の比較関数
static int sCmp (const void* p, const void* q)
{
	return *(short*)p - *(short*)q;
}

//compassのraw_data読み取り関数
static int compassReadRaw(Raw* data)
{
	int i;
	for(i=0; i<10; i++)
	{
		//compass_mode_change();
		data->xList[i] = read_out(fd, X_MSB_REG, X_LSB_REG);
		/*printf("x%d\n",data->xList[i]);*/
		data->yList[i] = read_out(fd, Y_MSB_REG, Y_LSB_REG);
		/*printf("y%d\n",data->yList[i]);*/
		data->zList[i] = read_out(fd, Z_MSB_REG, Z_LSB_REG);
		/*
		   uint8_t status_val = wiringPiI2CReadReg8(fd, 0x09);//とりあえずコメントアウトしておきます
		   printf("1st bit of status resister = %d\n", (status_val >> 0) & 0x01);//地磁気が正常ならここは1(死んでも1?)
		   printf("2nd bit of status resister = %d\n", (status_val >> 1) & 0x01);//地磁気が正常ならここは0(死んだら1)
		 */
		delay(10);
	}
	qsort(data->xList,10, sizeof(short), sCmp);
	qsort(data->yList,10, sizeof(short), sCmp);
	//qsort(zList,10, sizeof(short), sCmp);
	return 0;
}

//地磁気が-1もしくは任意の値にLockなった時に使う
static int handleCompassErrorOne(Raw* data)
{
	compass_initialize();//NOTE initialize
	printf("compass reinitialized\n");
	//compass_mode_change();
	compassReadRaw(data);
	printf("\n");
	return 0;
}

//地磁気が-4096になった時使う モーター回して近くの磁場を一応避ける
static int handleCompassErrorTwo(Raw *data)
{
	compass_initialize();
	printf("compass reinitialized\n");
	//compass_mode_change();
	motor_forward(MAX_PWM_VAL);
	delay(ESCAPE_TIME);
	compassReadRaw(data);
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

static int compass_read(Cmps* data)
{
	Raw rawdata;
	compassReadRaw(&rawdata);
	int LockCounter = 0;
	while((checkLock(rawdata.xList,-1)||checkLock(rawdata.yList,-1))&&(LockCounter<100))
	{
		printf("WARNING compass -1 lock\n");
		printf("LockCounter %d\n",LockCounter);
		handleCompassErrorOne(&rawdata);
		LockCounter++;
	}
	while((checkLock(rawdata.xList,-4096)||checkLock(rawdata.yList,-4096))&&(LockCounter<100))
	{
		handleCompassErrorTwo(&rawdata);
		printf("WARNING compass -4096 lock\n");
		printf("LockCounter %d\n",LockCounter);
		LockCounter++;
	}
	while((checkLock(rawdata.xList,rawdata.xList[0])&&checkLock(rawdata.yList,rawdata.yList[0]))&&(LockCounter<100))
	{
		printf("WARNING compass lock\n");
		printf("LockCounter %d\n",LockCounter);
		handleCompassErrorOne(&rawdata);
		LockCounter++;
	}

	if(LockCounter>=100)
	{
		printf("Lock Counter Max\n");//TODO
		sync();
		if(reboot(LINUX_REBOOT_CMD_RESTART)== -1)
		{
			printf("WARNING! REBOOT FAILED\n");
			printf("errno=%d: %s\n",errno, strerror(errno));
		}
	}

	data->x_value = (double)rawdata.xList[4] - COMPASS_X_OFFSET;
	data->y_value = (double)rawdata.yList[4] - COMPASS_Y_OFFSET;
	data->z_value = (double)rawdata.zList[4];
	return 0;
}

//偏角を考慮を考慮して計算
double cal_deviated_angle(double angle_of_deviation, double theta_degree)
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
static double calc_compass_angle(Cmps data)
{
	double cal_theta = atan2(-data.y_value*K_PARAMETER,data.x_value)*(180/PI);
	if(cal_theta  < -90)  //詳しい計算方法はkndまで
	{
		cal_theta = -cal_theta - 90;
	}
	else
	{
		cal_theta = 270 - cal_theta;
	}
	return cal_deviated_angle(ANGLE_OF_DEVIATION, cal_theta);
}

//コンパスで測った角度を返す
double readCompassAngle(void)
{
	Cmps data;
	compass_value_initialize(&data);
	compass_read(&data);
	double angle = 0;
	angle = calc_compass_angle(data);
	printf("Compass Angle:%f\n",angle);
	return angle;
}

//キャリブレーション用にxyの生データ
int read_for_calib()
{
	Cmps data;
	compass_value_initialize(&data);
	compass_read(&data);
	printf("%f\n",data.x_value);
	printf("%f\n",data.y_value);
	return 0;
}

/*******************************************/
/***以下はマシンによる自動地磁気calibration用****/
/*******************************************/

typedef struct cmps_offset {
	double x_offset_max;
	double x_offset_min;
	double y_offset_max;
	double y_offset_min;
	double x_offset;
	double y_offset;
} Cmps_offset;

static int compass_offset_initialize(Cmps_offset *compass_offset, Cmps *compass_data)
{
	compass_value_initialize(compass_data);
	compass_read(compass_data);
	compass_offset->x_offset_max = compass_data->x_value;
	compass_offset->x_offset_min = compass_data->x_value;
	compass_offset->y_offset_max = compass_data->y_value;
	compass_offset->y_offset_min = compass_data->y_value;
	compass_offset->x_offset = 0;
	compass_offset->y_offset = 0;
	return 0;
}

static int maxmin_compass(Cmps_offset *compass_offset, Cmps *compass_data)
{
	if(compass_data->x_value > compass_offset->x_offset_max)
	{
		compass_offset->x_offset_max = compass_data->x_value;
	}
	else if(compass_data->x_value < compass_offset->x_offset_min)
	{
		compass_offset->x_offset_min = compass_data->x_value;
	}

	if(compass_data->y_value > compass_offset->y_offset_max)
	{
		compass_offset->y_offset_max = compass_data->y_value;
	}
	else if(compass_data->y_value < compass_offset->y_offset_min)
	{
		compass_offset->y_offset_min = compass_data->y_value;
	}
	return 0;
}

static int mean_compass_offset(Cmps_offset *compass_offset)
{
	compass_offset->x_offset = (compass_offset->x_offset_max + compass_offset->x_offset_min)/2;
	compass_offset->y_offset = (compass_offset->y_offset_max + compass_offset->y_offset_min)/2;
	printf("x_offset=%f, y_offset=%f\n", compass_offset->x_offset
	       ,compass_offset->y_offset);
	return 0;
}

static int rotate_to_calib(Cmps *compass_data)
{
	compass_value_initialize(compass_data);
	motor_right(TURN_CALIB_POWER);
	delay(TURN_CALIB_MILLISECONDS);
        compass_read(compass_data);
	printf( "compass_x= %f, compass_y= %f\n",compass_data->x_value
	        ,compass_data->y_value);
	delay(50);
	return 0;
}

int cal_maxmin_compass(double *x_offset,double *y_offset)
{
	int i = 0;
        Cmps data;
        Cmps_offset offset;
	compass_offset_initialize(&offset,&data);
	for(i = 0; i<50; i++)
	{
		rotate_to_calib(&data);
		maxmin_compass(&offset,&data);
	}
	motor_stop();
	delay(1000);
	mean_compass_offset(&offset);
        *x_offset = offset.x_offset;
        *y_offset = offset.y_offset;
	return 0;
}

//キャリブレーション用にxyの生データ
double read_for_calib2(double x, double y)
{
    Cmps data;
    compass_value_initialize(&data);
    compass_read(&data);
    double cal_theta = atan2(-(data.y_value- y)*K_PARAMETER,data.x_value-x)*(180/PI);
    if(cal_theta  < -90)  //詳しい計算方法はkndまで
    {
        cal_theta = -cal_theta - 90;
    }
    else
    {
        cal_theta = 270 - cal_theta;
    }
    return cal_deviated_angle(ANGLE_OF_DEVIATION, cal_theta);
}

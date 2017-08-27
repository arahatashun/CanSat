
/***************************************************************************
   Modified BSD License
   ====================
   Copyright © 2016, Andrei Vainik
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   3. Neither the name of the organization nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   This piece of code was combined from several sources
   https://github.com/adafruit/Adafruit_BME280_Library
   https://cdn-shop.adafruit.com/datasheets/BST-BME280_DS001-10.pdf
   https://projects.drogon.net/raspberry-pi/wiringpi/i2c-library/
   Compensation functions and altitude function originally from:
   https://github.com/adafruit/Adafruit_BME280_Library/blob/master/Adafruit_BME280.cpp
 ***************************************************************************
   https://github.com/andreiva/raspberry-pi-bme280
   This is a library for the BME280 humidity, temperature & pressure sensor
   Designed specifically to work with the Adafruit BME280 Breakout
   ----> http://www.adafruit.com/products/2650
   These sensors use I2C or SPI to communicate, 2 or 4 pins are required
   to interface.
   Adafruit invests time and resources providing this open source code,
   please support Adafruit andopen-source hardware by purchasing products
   from Adafruit!
   Written by Limor Fried & Kevin Townsend for Adafruit Industries.
   BSD license, all text above must be included in any redistribution
 ***************************************************************************
 ****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include "bme280.h"
//ADRESS
static const int BME280_ADDRESS = 0x76;
static const int BME280_REGISTER_DIG_T1 = 0x88;
static const int BME280_REGISTER_DIG_T2 = 0x8A;
static const int BME280_REGISTER_DIG_T3 = 0x8C;
static const int BME280_REGISTER_DIG_P1 = 0x8E;
static const int BME280_REGISTER_DIG_P2 = 0x90;
static const int BME280_REGISTER_DIG_P3 = 0x92;
static const int BME280_REGISTER_DIG_P4 = 0x94;
static const int BME280_REGISTER_DIG_P5 = 0x96;
static const int BME280_REGISTER_DIG_P6 = 0x98;
static const int BME280_REGISTER_DIG_P7 = 0x9A;
static const int BME280_REGISTER_DIG_P8 = 0x9C;
static const int BME280_REGISTER_DIG_P9 = 0x9E;
static const int BME280_REGISTER_DIG_H1 = 0xA1;
static const int BME280_REGISTER_DIG_H2 = 0xE1;
static const int BME280_REGISTER_DIG_H3 = 0xE3;
static const int BME280_REGISTER_DIG_H4 = 0xE4;
static const int BME280_REGISTER_DIG_H5 = 0xE5;
static const int BME280_REGISTER_DIG_H6 = 0xE7;
static const int BME280_REGISTER_CHIPID = 0xD0;
static const int BME280_REGISTER_VERSION = 0xD1;
static const int BME280_REGISTER_SOFTRESET = 0xE0;
static const int BME280_RESET = 0xB6;
static const int BME280_REGISTER_CAL26 = 0xE1;
static const int BME280_REGISTER_CONTROLHUMID = 0xF2;
static const int BME280_REGISTER_CONTROL = 0xF4;
static const int BME280_REGISTER_CONFIG = 0xF5;
static const int BME280_REGISTER_PRESSUREDATA = 0xF7;
static const int BME280_REGISTER_TEMPDATA = 0xFA;
static const int BME280_REGISTER_HUMIDDATA = 0xFD;
//海水面気圧
//TODO 計算値の書き換え
static const float MEAN_SEA_LEVEL_PRESSURE = 1005.6;
static const int LOCL_COUNTER_MAX = 50;
static const double INF_ALTITUDE = 1000000000;
static const int SAMPLING_INTERVAL = 10;//milliseconds
static int fd = 0;
/*
 * Immutable calibration data read from bme280
 */
typedef struct
{
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;

	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;

	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;
} bme280_calib_data;

static bme280_calib_data cal;
/*
 * Raw sensor measurement data from bme280
 */
typedef struct
{
	uint8_t pmsb;
	uint8_t plsb;
	uint8_t pxsb;

	uint8_t tmsb;
	uint8_t tlsb;
	uint8_t txsb;

	uint8_t hmsb;
	uint8_t hlsb;

	uint32_t temperature;
	uint32_t pressure;
	uint32_t humidity;

} bme280_raw_data;

typedef struct
{
	uint32_t temperatureList[10];
	uint32_t pressureList[10];
	uint32_t humidityList[10];
} bme280_data_list;

typedef struct
{
	uint32_t temperature;
	uint32_t pressure;
	uint32_t humidity;
} bme280_processed_data;

static void readCalibrationData(bme280_calib_data *data);


int bme280_initialize()
{
	fd = wiringPiI2CSetup(BME280_ADDRESS);
	if(fd < 0)
	{
		printf("WARNING! BME280 wiringPiI2CSetup error\n");
		printf("fd = %d, errno=%d: %s\n", fd, errno, strerror(errno));
		return -1;
	}
	else
	{
		printf("BME280 wiringPiI2CSetup success\n");
		readCalibrationData(&cal);
		wiringPiI2CWriteReg8(fd, 0xf2, 0x01); // humidity oversampling x 1
		wiringPiI2CWriteReg8(fd, 0xf4, 0x25); // pressure and temperature oversampling x 1, mode normal
	}
	return 0;
}


static void readCalibrationData(bme280_calib_data *data)
{
	data->dig_T1 = (uint16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_T1);
	data->dig_T2 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_T2);
	data->dig_T3 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_T3);

	data->dig_P1 = (uint16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P1);
	data->dig_P2 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P2);
	data->dig_P3 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P3);
	data->dig_P4 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P4);
	data->dig_P5 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P5);
	data->dig_P6 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P6);
	data->dig_P7 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P7);
	data->dig_P8 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P8);
	data->dig_P9 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P9);

	data->dig_H1 = (uint8_t)wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H1);
	data->dig_H2 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_H2);
	data->dig_H3 = (uint8_t)wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H3);
	data->dig_H4 = (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H4) << 4) | (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H4+1) & 0xF);
	data->dig_H5 = (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H5+1) << 4) | (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H5) >> 4);
	data->dig_H6 = (int8_t)wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H6);
}


static int32_t getTemperatureCalibration(int32_t adc_T)
{
	int32_t var1  = ((((adc_T>>3) - ((int32_t)cal.dig_T1 <<1))) *
	                 ((int32_t)cal.dig_T2)) >> 11;

	int32_t var2  = (((((adc_T>>4) - ((int32_t)cal.dig_T1)) *
	                   ((adc_T>>4) - ((int32_t)cal.dig_T1))) >> 12) *
	                 ((int32_t)cal.dig_T3)) >> 14;

	return var1 + var2;
}

static float compensateTemperature(int32_t t_fine) {
	float T  = (t_fine * 5 + 128) >> 8;
	return T/100;
}

static float compensatePressure(int32_t adc_P, int32_t t_fine) {
	int64_t var1, var2, p;

	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)cal.dig_P6;
	var2 = var2 + ((var1*(int64_t)cal.dig_P5)<<17);
	var2 = var2 + (((int64_t)cal.dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)cal.dig_P3)>>8) +
	       ((var1 * (int64_t)cal.dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)cal.dig_P1)>>33;

	if (var1 == 0) {
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p<<31) - var2)*3125) / var1;
	var1 = (((int64_t)cal.dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)cal.dig_P8) * p) >> 19;

	p = ((p + var1 + var2) >> 8) + (((int64_t)cal.dig_P7)<<4);
	return (float)p/256;
}

/**********************************
   static float compensateHumidity(int32_t adc_H,int32_t t_fine)
   {
        int32_t v_x1_u32r;

        v_x1_u32r = (t_fine - ((int32_t)76800));

        v_x1_u32r = (((((adc_H << 14) - (((int32_t)cal.dig_H4) << 20) -
                        (((int32_t)cal.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                     (((((((v_x1_u32r * ((int32_t)cal.dig_H6)) >> 10) *
                          (((v_x1_u32r * ((int32_t)cal.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                        ((int32_t)2097152)) * ((int32_t)cal.dig_H2) + 8192) >> 14));

        v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                                   ((int32_t)cal.dig_H1)) >> 4));

        v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
        v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
        float h = (v_x1_u32r>>12);
        return h / 1024.0;
   }
 *****************************************/

static void getRawData(bme280_raw_data *raw)
{
	wiringPiI2CWrite(fd,0xf7);

	raw->pmsb = wiringPiI2CRead(fd);
	raw->plsb = wiringPiI2CRead(fd);
	raw->pxsb = wiringPiI2CRead(fd);

	raw->tmsb = wiringPiI2CRead(fd);
	raw->tlsb = wiringPiI2CRead(fd);
	raw->txsb = wiringPiI2CRead(fd);

	raw->hmsb = wiringPiI2CRead(fd);
	raw->hlsb = wiringPiI2CRead(fd);

	raw->temperature = 0;
	raw->temperature = (raw->temperature | raw->tmsb) << 8;
	raw->temperature = (raw->temperature | raw->tlsb) << 8;
	raw->temperature = (raw->temperature | raw->txsb) >> 4;

	raw->pressure = 0;
	raw->pressure = (raw->pressure | raw->pmsb) << 8;
	raw->pressure = (raw->pressure | raw->plsb) << 8;
	raw->pressure = (raw->pressure | raw->pxsb) >> 4;

	raw->humidity = 0;
	raw->humidity = (raw->humidity | raw->hmsb) << 8;
	raw->humidity = (raw->humidity | raw->hlsb);

}

double calcAltitude(float pressure,float temperature)
{
	// Equation taken from BMP180 datasheet (page 16):
	//  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

	// Note that using the equation from wikipedia can give bad results
	// at high altitude.  See this thread for more information:
	//  http://forums.adafruit.com/viewtopic.php?f=22&t=58064
	double altitude = (temperature + 273.15)
	                  * (pow(MEAN_SEA_LEVEL_PRESSURE/pressure, 0.190294957)-1.0) / 0.0065;

	return altitude;
}

//lock用、指定した値にlockされてたらreturn1する
static int isLocked(uint32_t* values,const uint32_t lock)
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

//uint32_t型用の比較関数
static int uint32_tCmp(const void* p, const void* q)
{
	if( *(uint32_t*)p > *(uint32_t*)q ) return 1;
	if( *(uint32_t*)p < *(uint32_t*)q ) return -1;
	return 0;
}

int getRawList(bme280_data_list* data)
{
	int i;
	for(i=0; i<10; i++)
	{
		bme280_raw_data raw;
		getRawData(&raw);
		printf("%dth pressure %d\n",i,raw.pressure);
		data->temperatureList[i] = raw.temperature;
		data->pressureList[i] = raw.pressure;
		data->humidityList[i]= raw.humidity;
		delay(SAMPLING_INTERVAL);
	}
	qsort(data->temperatureList,10, sizeof(uint32_t), uint32_tCmp);
	qsort(data->pressureList,10, sizeof(uint32_t), uint32_tCmp);
	qsort(data->humidityList,10, sizeof(uint32_t), uint32_tCmp);
	return 0;
}

int getProcessedData(bme280_processed_data* data)
{
	bme280_data_list list;
	getRawList(&list);
	int LockCounter = 0;
	while(isLocked(list.pressureList,list.pressureList[0])&&(LockCounter<LOCL_COUNTER_MAX))
	{
		printf("raw int pressure %d\n",list.pressureList[0]);
		printf("WARNING Pressure lock\n");
		printf("LockCounter %d\n",LockCounter);
		bme280_initialize();
		printf("BME280 reinitialized\n");
		getRawList(&list);
		LockCounter++;
	}
	if(LockCounter>=LOCL_COUNTER_MAX)
	{
		printf("LockCounter MAX\n");
		return -1;
	}
	data->temperature = list.temperatureList[4];
	data->pressure = list.pressureList[4];
	data->humidity = list.humidityList[4];
	return 0;
}

double readAltitude(void)
{
	bme280_processed_data data;
	if(getProcessedData(&data)==-1)
	{
		printf("CANNNOT READ ALTITUDE\n");
		printf("SET ALTITUDE MAX\n");
		return INF_ALTITUDE;
	}
	int32_t t_fine = getTemperatureCalibration(data.temperature);
	float t = compensateTemperature(t_fine); // C
	float p = compensatePressure(data.pressure,t_fine) / 100;// hPa
	//float h = compensateHumidity(data.humidity,t_fine);// %
	double a = calcAltitude(p,t); // meters
	printf("pressure:%f\naltitude:%f\n",p,a);
	return a;
}

float getSealevelPressure(float altitude)
{
	bme280_processed_data data;
	getProcessedData(&data);
	int32_t t_fine = getTemperatureCalibration(data.temperature);
	float t = compensateTemperature(t_fine); // C
	float p = compensatePressure(data.pressure,t_fine) / 100;// hPa
	//float h = compensateHumidity(data.humidity,t_fine);// %
	float sealevelPressure = p* pow(1-0.0065*altitude/(0.0065*altitude + t + 273.15),-5.257);
	printf("%f\n",sealevelPressure);
	return sealevelPressure;
}

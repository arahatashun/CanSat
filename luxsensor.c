#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "luxsensor.h"
/*#include "xbee_uart.h"
 #include "xbee.h"*/

//change adress:default 0x39
static const int TSL2561_ADDR_LOW = 0x29;
static const int TSL2561_ADDR_FLOAT = 0x39;     //default adress
static const int TSL2561_ADDR_HIGH  = 0x49;

static const int TSL2561_CONTROL_POWERON = 0x03;
static const int TSL2561_CONTROL_POWEROFF = 0x00;

static const int TSL2561_GAIN_0X = 0x00;   //No gain
static const int TSL2561_GAIN_AUTO = 0x01;
static const int TSL2561_GAIN_1X = 0x02;
static const int TSL2561_GAIN_16X = 0x02; // (0x10)
static const int TSL2561_INTEGRATIONTIME_13MS = 0x00;   // 13.7ms
static const int TSL2561_INTEGRATIONTIME_101MS = 0x01; // 101ms
static const int TSL2561_INTEGRATIONTIME_402MS = 0x02; // 402ms

static const int TSL2561_READBIT = 0x01;
static const int TSL2561_COMMAND_BIT = 0x80;  //Must be 1
static const int TSL2561_CLEAR_BIT = 0x40;  //Clears any pending interrupt (write 1 to clear)
static const int TSL2561_WORD_BIT = 0x20;  // 1 = read/write word (rather than byte)
static const int TSL2561_BLOCK_BIT = 0x10;   // 1 = using block read/write

static const int TSL2561_REGISTER_CONTROL = 0x00;
static const int TSL2561_REGISTER_TIMING = 0x81;

/*xbee用宣言*/
/*
   static const int TSL2561_REGISTER_THRESHHOLDL_LOW = 0x02;
   static const int TSL2561_REGISTER_THRESHHOLDL_HIGH = 0x03;
   static const int TSL2561_REGISTER_THRESHHOLDH_LOW = 0x04;
   static const int TSL2561_REGISTER_THRESHHOLDH_HIGH = 0x05;
   static const int TSL2561_REGISTER_INTERRUPT = 0x06;
   static const int TSL2561_REGISTER_CRC = 0x08;
   static const int TSL2561_REGISTER_ID = 0x0A;
 */
static const int TSL2561_REGISTER_CHAN0_LOW = 0x8C;
static const int TSL2561_REGISTER_CHAN0_HIGH = 0x8D;
static const int TSL2561_REGISTER_CHAN1_LOW = 0x8E;
static const int TSL2561_REGISTER_CHAN1_HIGH = 0x8F;
//Delay getLux function
static const int LUXDELAY = 500;
static const int LIGHT_THRESHOLD = 50;  //光センサー閾値


//グローバルデータ宣言(not const)
static int fd = 0;   //output of wiringPi setup
static int WPI2CWReg8 = 0;
static uint16_t visible_and_ir;        //CH0 photodiode:sensitive to both visible and infrared light
static uint16_t ir;     //CH1 photodiode:sensitive primarily to infared light


//関数プロトタイプ宣言(static)
static double getLux();


int luxsensor_initializer()
{
	//I2c setup
	fd = wiringPiI2CSetup(TSL2561_ADDR_FLOAT);
	if(fd == -1)
	{
		printf("WARNING! luxsensor wiringPiI2CSetup error\n");
		printf("fd = %d, errno=%d: %s\n", fd, errno, strerror(errno));
		return -1;
	}
	else
	{
		printf("luxsensor wiringPiI2CSetup success\n");
		printf("fd = %d, errno=%d: %s\n", fd, errno, strerror(errno));
	}
	wiringPiI2CWriteReg8(fd, TSL2561_COMMAND_BIT, TSL2561_CONTROL_POWERON);
	return 0;
}

int luxsensor_close()
{
	wiringPiI2CWriteReg8(fd, TSL2561_COMMAND_BIT, TSL2561_CONTROL_POWEROFF);
	return 0;
}

static double getLux()
{
	double a, b;
	// Set timing (101 mSec)
	wiringPiI2CWriteReg8(fd, TSL2561_REGISTER_TIMING, TSL2561_GAIN_AUTO);
	//Wait for the conversion to complete
	delay(LUXDELAY);
	//Reads visible + IR diode from the I2C device auto
	visible_and_ir = wiringPiI2CReadReg16(fd, TSL2561_REGISTER_CHAN0_LOW);
	ir = wiringPiI2CReadReg16(fd, TSL2561_REGISTER_CHAN1_LOW);
	a = (double)visible_and_ir;
	b = (double)ir;
	// Disable the device
	return b / a;
}

double calculateLux()
{
	double ratio =0;
	double lux =0;
	double p =0;
	ratio = getLux();
	p = pow(ratio,1.4);
	if ((ratio >= 0) & (ratio <= 0.52)) {
		lux = (0.0315 * visible_and_ir) - (0.0593 * visible_and_ir * p);
	}
	else if (ratio <= 0.65) {
		lux = (0.0229 * visible_and_ir) - (0.0291 * ir);
	}
	else if (ratio <= 0.80) {
		lux = (0.0157 * visible_and_ir) - (0.018 * ir);
	}
	else if (ratio <= 1.3) {
		lux = (0.00338 * visible_and_ir) - (0.0026 * ir);
	}
	else if (ratio > 1.3) {
		lux = 0;
	}
	return lux;
}

/*xbee用関数　*/
/*int islight(){
        double lux=0;
        lux = calculateLux();
        if(lux>LIGHT_THRESHOLD) {
                xbee_printf("light");
                return 1;
        }
        else{
                xbee_printf("dark");
                return 0;
        }
   }*/

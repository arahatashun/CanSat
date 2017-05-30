#include <wiringPi.h>
#include <stdio.h>
#include "luxsensor.h"

int main()
{
	int lux_value = 0;
	while(1)
	{
		luxsensor_initializer();
		lux_value = getLux();
		printf("lux :%d", lux_value);
		delay(1000);
		wiringPiI2CWriteReg8(fd, TSL2561_COMMAND_BIT, TSL2561_CONTROL_POWEROFF);
	}
}

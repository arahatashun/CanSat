#include <wiringPi.h>
#include <stdio.h>
#include "luxsensor.h"

int main()
{
	int lux_value = 0;
	while(1)
	{
		luxsensor_initialize();
		lux_value = getLux();
		printf("lux :%d\n", lux_value);
		delay(1000);
		luxsensor_close();
	}
}

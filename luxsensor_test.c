#include <wiringPi.h>
#include <stdio.h>
#include "luxsensor.h"

int main()
{
	luxsensor_initializer();
	int lux_value = 0;
	while(1)
	{
		lux_value = getLux();
		printf("lux :%d", lux_value);
		delay(1000);
	}
}

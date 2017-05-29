#include <wiringPi.h>
#include <stdio.h>
#include "luxsensor.h"

int main()
{
	luxsensor_initializer();
	double lux_value = 0;
	while(1)
	{
		lux_value = alculateLux();
		printf("lux :%f", lux_value);
		delay(1000);
	}
}

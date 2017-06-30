#include <stdio.h>
#include "../compass.h"
#include <wiringPi.h>

int main()
{
	int compass_initialize();
	while(1)
	{
		double angle =  readCompassAngle();
		printf("%f\n",angle);
		delay(500);
	}
}

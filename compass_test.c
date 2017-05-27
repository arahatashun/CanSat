#include "compass.h"
#include <wiringPi.h>

int main()
{
	compass_initializer_2();
	while (1)
	{
		double angle;
		compass_get_angle(&angle);
		delay(1000);
	}
}

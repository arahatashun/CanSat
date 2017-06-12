#include <wiringPi.h>
#include "compass.h"
#include <stdio.h>

int main()
{
	compass_initializer();
	Cmps compass_data;
	double compass_angle = 0;
	while(1)
	{
		compass_read_scatter(&compass_data);
		printf("%f\n", compass_data.compassx_value);
		printf("%f\n", compass_data.compassy_value);
		delay(200);
	}
	return 0;
}

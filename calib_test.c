#include <wiringPi.h>
#include "compass.h"

int main()
{
	compass_initializer();
	Cmps compass_data;
	Cmps_offset compass_offset;
	while(1)
	{
		cal_maxmin_compass(&compass_offset,&compass_data);
		delay(2000);
	}
	return 0;
}

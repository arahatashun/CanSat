#include <wiringPi.h>
#include "compass.h"

int main()
{
	compass_initializer();
	Cmps compass_data;
	Cmps_offset compass_offset;
	cal_maxmin_compass(&compass_offset,&compass_data);
	delay(2000);
	double compass_x = 0;
	double compass_y = 0;
	while(1)
	{
		compass_read(&compass_data);
		compass_x = compass_data.compassx_value - compass_offset.comapssx_offset;
		compass_y = compass_data.compassy_value - compass_offset.comapssy_offset;
		calc_compass_angle(compass_x, compass_y);
	}
	return 0;
}

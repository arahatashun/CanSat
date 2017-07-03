/*マシンを自動で回転させながら地磁気のログを取るプログラム
   今のところ使う予定はないが、一応残しておく*/
#include <wiringPi.h>
 #include "../compass.h"
 #include <stdio.h>

int main()
{
	pwm_initialize();
	compass_initialize();
	Cmps compass_data;
	Cmps_offset compass_offset;
	cal_maxmin_compass(&compass_offset,&compass_data);
	delay(2000);
	printf("compassx_offset = %f\n", compass_offset.compassx_offset);
	printf("compassy_offset = %f\n", compass_offset.compassy_offset);
	while(1)
	{
		compass_read(&compass_data);
		double compass_x = compass_data.x_value - compass_offset.compassx_offset;
		double compass_y = compass_data.y_value - compass_offset.compassy_offset;
		double compass_angle = calc_compass_angle(compass_x, compass_y);
		printf("compass_angle=%f\n",compass_angle);
		delay(1000);
	}
	return 0;
}

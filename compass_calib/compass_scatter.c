#include <wiringPi.h>
#include "compass.h"
#include <stdio.h>

int main()
{
	compass_initialize();
	Cmps compass_data;
	FILE *fp=fopen("compasslog.txt","w");//新規新規書き込み
	while(1)
	{
		compass_read_scatter(&compass_data);
		printf("%f\n", compass_data.x_value);
		printf("%f\n", compass_data.y_value);
		delay(200);
	}
	return 0;
}

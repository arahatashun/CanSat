/*マシンを自動で回転させながら地磁気のログを取るプログラム
   今のところ使う予定はないが、一応残しておく*/
#include <wiringPi.h>
#include "../compass.h"
#include <stdio.h>
#include <signal.h>

void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}

int main()
{
	pwm_initialize();
	compass_initialize();
	signal(SIGINT, handler);
        double x_fuck = 0;
        double y_fuck = 0;
	cal_maxmin_compass(&x_fuck, &y_fuck);
	delay(2000);
	printf("compassx_offset = %f\n", x_fuck);
	printf("compassy_offset = %f\n", y_fuck);
	while(1)
	{
        double compass_angle = read_for_calib2(x_fuck,y_fuck);
        printf("compass_angle=%f\n",compass_angle);
		delay(1000);
	}
	return 0;
}

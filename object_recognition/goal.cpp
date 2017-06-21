#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <wiringPi.h>
#include "camera.hpp"
#include "../motor.h"

#pragma comment(lib,"opencv_world320.lib")

static const int ROTATE_POWER = 20;
static const int ROTATE_MILLISECONDS = 200;
static const int LEFT_MAX = -100;
static const int RIGHT_MAX = 100;
static const int CENTER_THRESHOLD = 30;//-30~30で直進するようにする

int main(void)
{
	pwm_initialize();
while(1)
{
	cv::Mat red = Mred(takePhoto());
	countArea(red);
	int center = getCenter(red);
	printf("%d\n",center);
	if(LEFT_MAX<center<-CENTER_THRESHOLD)
	{
		motor_right(ROTATE_POWER);
		delay(ROTATE_MILLISECONDS);
		motor_stop();
		delay(10);
	}
	else if(CENTER_THRESHOLD<center<RIGHT_MAX)
	{
		motor_left(ROTATE_POWER);
		delay(ROTATE_MILLISECONDS);
		motor_stop();
		delay(10);
	}
	else
	{
		motor_forward(100);
		delay(1000);
		motor_stop();
	}
 }
return 0;
}

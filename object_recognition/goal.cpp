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
	cv::Mat red = Mred(takePhoto());
	countArea(red);
	pwm_initialize();
while(1)
{
	if(LEFT_MAX<getCenter(red)<-CENTER_THRESHOLD)
	{
		motor_right(ROTATE_POWER);
		delay(ROTATE_MILLISECONDS);
	}
	if(CENTER_THRESHOLD<getCenter(red)<RIGHT_MAX)
	{
		motor_left(ROTATE_POWER);
		delay(ROTATE_MILLISECONDS);
	}
	if(-CENTER_THRESHOLD<getCenter(red)<CENTER_THRESHOLD)
	{
		motor_forward(100);
		delay(200);
	}
 }
return 0;
}

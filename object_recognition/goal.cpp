#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <wiringPi.h>
#include "camera.hpp"
#include "../motor.h"

#pragma comment(lib,"opencv_world320.lib")

static const int ROTATE_POWER = 40;
static const int ROTATE_MILLISECONDS = 200;
static const int LEFT_MAX = -100;
static const int RIGHT_MAX = 100;
static const int CENTER_THRESHOLD = 30;//-30~30で直進するようにする
static const double THRESHOLD_PRECENTAGE = 0.09//ゴール存在判定

//TODO turn millisecondどんどん大きくしていくPI制御にする
int main(void)
{
	pwm_initialize();
	while(1)
	{
		cv::Mat red = Mred(takePhoto());
		double count = countArea(red);
		if(count < 0.09)
		{
			motor_right(50);
			delay(100);
			motor_stop();
			delay(10)
		}
		else
		{
			int center = getCenter(red);
			printf("%d\n",center);
			if(LEFT_MAX<center && center<-CENTER_THRESHOLD)
			{
				motor_right(ROTATE_POWER);
				delay(ROTATE_MILLISECONDS);
				motor_stop();
				delay(10);
			}
			if(CENTER_THRESHOLD<center && center<RIGHT_MAX)
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
				delay(10);
			}
		}
	}
	return 0;
}

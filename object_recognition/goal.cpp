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
static const double EXIST_THRESHOLD = 0.07;//ゴール存在判定 パーセンテージ

//TODO turn millisecondどんどん大きくしていくPI制御にする
int main (void)
{
	pwm_initialize();
	while(1)
	{
		cv::Mat red = Mred();
		double count = countArea(red);
		if(count < EXIST_THRESHOLD)
		{
			//回転するだけ
			motor_right(ROTATE_POWER);
			delay(ROTATE_MILLISECONDS);
			motor_stop();
			delay(10);
		}
		else
		{
			//見つけれたら前進
			motor_forward(100);
			delay(200);
			motor_stop();
			delay(10);
		}
	}
	return 0;
}

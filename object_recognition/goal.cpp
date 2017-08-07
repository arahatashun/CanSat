#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <time.h>
#include <wiringPi.h>
#include "camera.hpp"
#include "../motor.h"

#pragma comment(lib,"opencv_world320.lib")

static const int ROTATE_POWER = 100;
static const int ROTATE_MILLISECONDS = 200;
static const int LEFT_MAX = -100;
static const int RIGHT_MAX = 100;
static const int CENTER_THRESHOLD = 30;//-30~30で直進するようにする
static const double EXIST_THRESHOLD = 0.1;//ゴール存在判定 パーセンテージ
static const int TIME_LIMIT = 900;//10分

void handler(int signum)
{
	motor_stop();
	delay(100);
	exit(1);
}


int main (void)
{
	signal(SIGINT, handler);
	time_t startTime;
	time(&startTime);
	time_t lastTime;
	time(&lastTime);
	pwm_initialize();
	Camera camera;
	while(lastTime-startTime<TIME_LIMIT)
	{
		printf("lastTime - startTime %d\n",lastTime - startTime);
		time(&lastTime);
		camera.takePhoto();
	  camera.binarize();
	  double count = camera.countArea();
		if(count < EXIST_THRESHOLD)
		{
			//回転するだけ
			motor_right(ROTATE_POWER);
			delay(ROTATE_MILLISECONDS);
			motor_stop();
			delay(150);
		}
		else
		{
			//見つけれたら前進
			motor_forward(100);
			delay(400);
			motor_stop();
			delay(150);
		}
	}
	printf("TIME IS OUT\n");
	return 0;
}

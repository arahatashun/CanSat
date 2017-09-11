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
static const int MINIMUM_TIMUOUT = 400;//seconds
static const int MAXIMUM_TIMEOUT = 600;//seconds
static const int DELAY_MILLIS = 50;
static const int CONTINUOUS_FORWARD = 3;

void handler(int signum);

Camera camera;

int main (void)
{
	signal(SIGINT, handler);
	time_t startTime;
	time(&startTime);
	time_t lastTime;
	time(&lastTime);
	pwm_initialize();
	int forward_count = 0;//連続してforwardした回数
	while(lastTime-startTime<MAXIMUM_TIMEOUT)
	{
		printf("lastTime - startTime %d\n",lastTime - startTime);
		time(&lastTime);
		camera.takePhoto();
	  camera.binarize();
	  double count = camera.countArea();
		if(count < EXIST_THRESHOLD)
		{
			//回転するだけ
			forward_count=0;
			motor_right(ROTATE_POWER);
			delay(ROTATE_MILLISECONDS);
			motor_stop();
			delay(DELAY_MILLIS);
		}
		else
		{
			//見つけれたら前進
			forward_count++;
			motor_forward(100);
			delay(400);
			motor_stop();
			delay(DELAY_MILLIS);
		}
		if(lastTime-startTime>MINIMUM_TIMUOUT && forward_count>=CONTINUOUS_FORWARD)
		{
			printf("NORMAL TIMEOUT\n");
			return 0;
		}
	}
	printf("MAXIMUM_TIMEOUT\n");
	return 0;
}

void handler(int signum)
{
	motor_stop();
	camera.~Camera();
	delay(100);
	exit(1);
}

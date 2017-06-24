#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "motor.h"

static const int LEFT_MOTOR1 = 23;//GPIO23
static const int LEFT_MOTOR2 = 24;//GPIO24
static const int RIGHT_MOTOR1 = 27;//GPIO27
static const int RIGHT_MOTOR2 = 17;//GPIO17
static const int PWM_RANGE = 100;
static const int INITIAL_PWM_VAL = 0;
static const int ZERO_PWM_VAL = 0;
static const int MAX_PWM_VAL = 100;


int pwm_initialize()
{
	//wiring Pi initialize
	if(wiringPiSetupGpio()!=0)
	{
		printf("motor_setup_failed\n");
	}
	pwmSetMode(PWM_MODE_MS);
	//soft pwm initialize
	softPwmCreate(RIGHT_MOTOR1,INITIAL_PWM_VAL,PWM_RANGE);
	softPwmCreate(RIGHT_MOTOR2,INITIAL_PWM_VAL,PWM_RANGE);
	softPwmCreate(LEFT_MOTOR1,INITIAL_PWM_VAL,PWM_RANGE);
	softPwmCreate(LEFT_MOTOR2,INITIAL_PWM_VAL,PWM_RANGE);
	return 0;
}

/*
    pwm_valueは0~100の値をとる。
    内部でthreadingしてることに注意
 */
int motor_stop()
{
	printf("MOTOR stop\n");
	softPwmWrite(RIGHT_MOTOR1,ZERO_PWM_VAL);
	softPwmWrite(RIGHT_MOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFT_MOTOR1,ZERO_PWM_VAL);
	softPwmWrite(LEFT_MOTOR2,ZERO_PWM_VAL);
	return 0;
}

int motor_forward(int pwm_value)
{
	printf("MOTOR forward\n");
	softPwmWrite(RIGHT_MOTOR1,pwm_value);
	softPwmWrite(RIGHT_MOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFT_MOTOR1,pwm_value);
	softPwmWrite(LEFT_MOTOR2,ZERO_PWM_VAL);
	return 0;
}

int motor_back(int pwm_value)
{
	printf("MOTOR back\n");
	softPwmWrite(RIGHT_MOTOR1,ZERO_PWM_VAL);
	softPwmWrite(RIGHT_MOTOR2,pwm_value);
	softPwmWrite(LEFT_MOTOR1,ZERO_PWM_VAL);
	softPwmWrite(LEFT_MOTOR2,pwm_value);
	return 0;
}

int motor_right(int pwm_value)
{
	printf("MOTOR right\n");
	softPwmWrite(RIGHT_MOTOR1,ZERO_PWM_VAL);
	softPwmWrite(RIGHT_MOTOR2,pwm_value);
	softPwmWrite(LEFT_MOTOR1,pwm_value);
	softPwmWrite(LEFT_MOTOR2,ZERO_PWM_VAL);
	return 0;
}

int motor_left(int pwm_value)
{
	printf("MOTOR left\n");
	softPwmWrite(RIGHT_MOTOR1,pwm_value);
	softPwmWrite(RIGHT_MOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFT_MOTOR1,ZERO_PWM_VAL);
	softPwmWrite(LEFT_MOTOR2,pwm_value);
	return 0;
}


//pwm_valueを-100~100に拡張したもの
//負の時にleft,正の時にright

int motor_rotate(int pwm_value)
{
	printf("motor rotate\n");
	if(pwm_value>0)
	{
		motor_right(pwm_value);
	}
	else
	{
		motor_left(pwm_value);
	}
	return 0;
}

int motor_slalom(int delta_pwm)
{
	if(delta_pwm>=0)
	{
		//右に曲がる
		softPwmWrite(RIGHT_MOTOR1,MAX_PWM_VAL-delta_pwm);
		softPwmWrite(RIGHT_MOTOR2,ZERO_PWM_VAL);
		softPwmWrite(LEFT_MOTOR1,MAX_PWM_VAL);
		softPwmWrite(LEFT_MOTOR2,ZERO_PWM_VAL);
	}
	else if(delta_pwm<0)
	{
		//左に曲がる
		softPwmWrite(RIGHT_MOTOR1,MAX_PWM_VAL);
		softPwmWrite(RIGHT_MOTOR2,ZERO_PWM_VAL);
		softPwmWrite(LEFT_MOTOR1,MAX_PWM_VAL-delta_pwm);
		softPwmWrite(LEFT_MOTOR2,ZERO_PWM_VAL);
	}
	return 0;
}

int motor_escape() //delayは適当
{
	printf("get stacked\n");
	motor_back(100);
	delay(500);
	motor_right(100);
	delay(500);
	motor_forward(100);
	delay(2000);
	motor_stop();
	delay(100);
	return 0;
}

#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>


static const int RIGHTMOTOR1 = 17;//GPIO17
static const int RIGHTMOTOR2 = 27;//GPIO27
static const int LEFTMOTOR1 = 23;//GPIO23
static const int LEFTMOTOR2 = 24;//GPIO24
static const int PWM_RANGE = 100;
static const int INITIAL_PWM_VAL = 0;
static const int ZERO_PWM_VAL = 0;

int pwm_initializer()
{
	//wiring Pi initialize
	if(wiringPiSetupGpio() != 0)
	{
		printf("setup faied");
	}
	//soft pwm initialize
	softPwmCreate(RIGHTMOTOR1,INITIAL_PWM_VAL,PWM_RANGE);
	softPwmCreate(RIGHTMOTOR2,INITIAL_PWM_VAL,PWM_RANGE);
	softPwmCreate(LEFTMOTOR1,INITIAL_PWM_VAL,PWM_RANGE);
	softPwmCreate(LEFTMOTOR2,INITIAL_PWM_VAL,PWM_RANGE);
	return 0;
}

int motor_stop(int milliseconds)
{
	softPwmWrite(RIGHTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(RIGHTMOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR2,ZERO_PWM_VAL);
	delay(milliseconds);
	return 0;
}

int motor_forward(int milliseconds,int pwm_value)
{
	softPwmWrite(RIGHTMOTOR1,pwm_value);
	softPwmWrite(RIGHTMOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR1,pwm_value);
	softPwmWrite(LEFTMOTOR2,ZERO_PWM_VAL);
	delay(milliseconds);
	motor_stop(10);
	return 0;
}

int motor_back(int milliseconds,int pwm_value)
{
	softPwmWrite(RIGHTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(RIGHTMOTOR2,pwm_value);
	softPwmWrite(LEFTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR2,pwm_value);
	delay(milliseconds);
	motor_stop(10);
	return 0;
}

int motor_right(int milliseconds,int pwm_value)
{
	softPwmWrite(RIGHTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(RIGHTMOTOR2,pwm_value);
	softPwmWrite(LEFTMOTOR1,pwm_value);
	softPwmWrite(LEFTMOTOR2,ZERO_PWM_VAL);
	delay(milliseconds);
	motor_stop(10);
	return 0;
}

int motor_left(int milliseconds,int pwm_value)
{
	softPwmWrite(RIGHTMOTOR1,pwm_value);
	softPwmWrite(RIGHTMOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR2,pwm_value);
	delay(milliseconds);
	motor_stop(10);
	return 0;
}

#include <wiringPi.h>
#include <softPwm.h>
#include<cstdio>

static const int RIGHTMOTOR1 = 17;//GPIO17
static const int RIGHTMOTOR2 = 27;//GPIO27
static const int LEFTMOTOR1 = 23;//GPIO23
static const int LEFTMOTOR2 = 24;//GPIO24
static const int PWM_RANGE = 100;
static const int INITIAL_PWM_VAL = 0;
static const int ZERO_PWM_VAL = 0;

int pwm_initializer(){
	//wiring Pi initialize
	if(wiringPiSetupGpio() != 0) {
		printf("setup faied");
	}
	//soft pwm initialize
	softPwmCreate(RIGHTMOTOR1,INITIAL_PWM_VAL,PWM_RANGE);
	softPwmCreate(RIGHTMOTOR2,INITIAL_PWM_VAL,PWM_RANGE);
	softPwmCreate(LEFTMOTOR1,INITIAL_PWM_VAL,PWM_RANGE);
	softPwmCreate(LEFTMOTOR2,INITIAL_PWM_VAL,PWM_RANGE);
	return 0;
}

int motor_stop(int seconds)
{
	softPwmWrite(RIGHTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(RIGHTMOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR2,ZERO_PWM_VAL);
	int milliseconds=seconds*1000;
	delay(milliseconds);
	return 0;
}

int motor_cease()
{
	softPwmWrite(RIGHTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(RIGHTMOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR2,ZERO_PWM_VAL);
	return 0;
}

int motor_forward(int seconds,int pwm_value)
{
	softPwmWrite(RIGHTMOTOR1,pwm_value);
	softPwmWrite(RIGHTMOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR1,pwm_value);
	softPwmWrite(LEFTMOTOR2,ZERO_PWM_VAL);
	int milliseconds=seconds*1000;
	delay(milliseconds);
	motor_cease();
	return 0;
}

int motor_back(int seconds,int pwm_value)
{
	softPwmWrite(RIGHTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(RIGHTMOTOR2,pwm_value);
	softPwmWrite(LEFTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR2,pwm_value);
	int milliseconds=seconds*1000;
	delay(milliseconds);
	motor_cease();
	return 0;
}

int motor_right(int seconds,int pwm_value)
{
	softPwmWrite(RIGHTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(RIGHTMOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR1,pwm_value);
	softPwmWrite(LEFTMOTOR2,ZERO_PWM_VAL);
	int milliseconds=seconds*1000;
	delay(milliseconds);
	motor_cease();
	return 0;
}

int motor_left(int seconds,int pwm_value)
{
	softPwmWrite(RIGHTMOTOR1,pwm_value);
	softPwmWrite(RIGHTMOTOR2,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR1,ZERO_PWM_VAL);
	softPwmWrite(LEFTMOTOR2,ZERO_PWM_VAL);
	int milliseconds=seconds*1000;
	delay(milliseconds);
	motor_cease();
	return 0;
}

int main()
{
	pwm_initializer();
	motor_forward(10,40);
	motor_right(4,40);
	motor_stop(10);
	motor_left(4,40);
	motor_back(10,40);
	motor_forward(5,40);
	return 0;
}

#include "motor_run.h"

int main()
{
	pwm_initializer();
	motor_forward(100,70);
	motor_right(100,40);
	motor_stop(100);
	motor_left(100,40);
	motor_back(100,40);
	motor_forward(100,80);
	motor_stop(5);
	return 0;
}

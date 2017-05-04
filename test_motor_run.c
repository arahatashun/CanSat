#include "motor_run.h"

int main()
{
	pwm_initializer();
	motor_forward(10,70);
	motor_right(10,40);
	motor_stop(10);
	motor_left(10,40);
	motor_back(10,40);
	motor_forward(10,80);
	motor_stop(5);
	return 0;
}

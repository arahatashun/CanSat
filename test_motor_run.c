#include "motor_run.h"

int main()
{
	pwm_initializer();
	motor_forward(10,70);
	motor_right(4,40);
	motor_stop(5);
	motor_left(4,40);
	motor_back(10,40);
	motor_forward(5,80);
	return 0;
}

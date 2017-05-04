#include "motor_run.h"

int main()
{
	pwm_initializer();
	motor_forward(100,70);
	motor_right(400,40);
	motor_stop(500);
	motor_left(400,40);
	motor_back(100,40);
	motor_forward(500,80);
	motor_stop(5);
	return 0;
}

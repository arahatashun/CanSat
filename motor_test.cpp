// motor_test.cpp

#include<cstdio>
#include"motor_softpwm.hpp"

int main()
{
	MotorSoftPwm fwrd(1,70,70,10);
	MotorSoftPwm bwrd(2,70,70,10);
	MotorSoftPwm stop(0,70,70,5);
	MotorSoftPwm right(3,70,70,10);
	MotorSoftPwm left(4,70,70,10);
	fwrd.motorMove();
	bwrd.motorMove();
	stop.motorMove();
	right.motorMove();
	left.motorMove();
	return 0;
}

	
	
	
	
// motor_test.cpp

#include<cstdio>
#include"motor_softpwm.cpp"

int main()
{
	MotorSoftPwm test1(1,70,70,10);
	test1.motorMove();
    test1.setpwm(2,70,70,10);
    test1.motorMove();
    test1.setpwm(3,70,70,10);
	test1.motorMove();
	test1.setpwm(4,70,70,10);
	test1.motorMove();
	test1.setpwm(5,70,70,10);
	test1.motorMove();
	return 0;
}

	
	
	
	
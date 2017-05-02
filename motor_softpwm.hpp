#ifndef MOTOR_SPWM
#define MOTOR_SPWM

//motor_softpwm.hpp
//motor softpwm class header file

#include<cstdio>
#include<wiringPi.h>
#include<softPwm.h>

const int RIGHTMOTOR1 17    //GPIO17
const int RIGHTMOTOR2 27    //GPIO27
const int LEFTMOTOR1 23    //GPIO23
const int LEFTMOTOR2 24    //GPIO24
const int RANGE 100
const int STOP 0
const int FWRD 1
const int BACK 2
const int RIGHT 3
const int LEFT 4

class MotorSoftPwm {
	int mode, pwmvalue_r, pwmvalue_l, pwmsecond;
public:
	MotorSoftPwm(int m,int pr,int pl,int ps);    //constructor function
	int motorMove();
	int setpwm(int m,int pr,int pl,int ps);motor
};

#endif	  		
    		



	 
	
	    
	  
	

	

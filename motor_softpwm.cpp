//motor_softpwm.cpp
//motor softpwm class source file

#include"motor_softpwm.hpp"
#include<cstdio>
#include<wiringPi.h>
#include<softPwm.h>

MotorSoftPwm::MotorSoftPwm(int m,int pr,int pl,int ps)		//constuctor function
{
	//wiring Pi initialize
	if(wiringPiSetupGpio() == -1){
	    std::printf("setup faied");
	    return;
	   }
	 //creates &initialize software controlled PWM pin
	 softPwmCreate(RIGHTMOTOR1,0,RANGE);
	 softPwmCreate(RIGHTMOTOR2,0,RANGE);
	 softPwmCreate(LEFTMOTOR1,0,RANGE);
	 softPwmCreate(LEFTMOTOR2,0,RANGE);
	 //constructor argument
	 mode =m;
	 pwmvalue_r = pr;
	 pwmvalue_l = pl;
	 pwmsecond = ps;
}

int MotorSoftPwm::motorMove()
{
    switch(mode)
    {
      case STOP:    //stop
    		softPwmWrite(RIGHTMOTOR1,0);    
    		softPwmWrite(RIGHTMOTOR2,0);
    		softPwmWrite(LEFTMOTOR1,0);
    		softPwmWrite(LEFTMOTOR2,0);
			break;
    	case FWRD:    //forward
    		softPwmWrite(RIGHTMOTOR1,pwmvalue_r);
    		softPwmWrite(RIGHTMOTOR2,0);
    		softPwmWrite(LEFTMOTOR1,pwmvalue_l);
    		softPwmWrite(LEFTMOTOR2,0);
			break;
     	case BACK:    //back
    		softPwmWrite(RIGHTMOTOR1,0);
    		softPwmWrite(RIGHTMOTOR2,pwmvalue_r);
    		softPwmWrite(LEFTMOTOR1,0);
    		softPwmWrite(LEFTMOTOR2,pwmvalue_l);
			break;
    	case RIGHT:    //right
    		softPwmWrite(RIGHTMOTOR1,pwmvalue_r);
    		softPwmWrite(RIGHTMOTOR2,0);
    		softPwmWrite(LEFTMOTOR1,0);
    		softPwmWrite(LEFTMOTOR2,pwmvalue_l);
			break;
    	case LEFT:    //left
    		softPwmWrite(RIGHTMOTOR1,0);
    		softPwmWrite(RIGHTMOTOR2,pwmvalue_r);
    		softPwmWrite(LEFTMOTOR1,pwmvalue_l);
    		softPwmWrite(LEFTMOTOR2,0);
			break;
    	default:  //else
    		softPwmWrite(RIGHTMOTOR1,0);
    		softPwmWrite(RIGHTMOTOR2,0);
    		softPwmWrite(LEFTMOTOR1,0);
    		softPwmWrite(LEFTMOTOR2,0);
			break;
	   }
	delay(pwmsecond *1000);
	  
	  return 0;
}
	
int MotorSoftPwm::motorMove(int m,int pr, int pl,int ps)
{
	 mode =m;
	 pwmvalue_r = pr;
	 pwmvalue_l = pl;
	 pwmsecond = ps;
}

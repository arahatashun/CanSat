//motor_softpwm.hpp

#include<cstdio>
#include<wiringPi.h>
#include<softPwm.h>

#define RIGHTMOTOR1 17    //GPIO17
#define RIGHTMOTOR2 27    //GPIO27
#define LEFTMOTOR1 23    //GPIO23
#define LEFTMOTOR2 24    //GPIO24
#define RANGE 100
#define FWRD 1
#define BACK 2
#define RIGHT 3
#define LEFT 4
#define STOP 0

class MotorSoftPwm {
	int mode, pwmvalue_r, pwmvalue_l, pwmsecond;
public:
	MotorSoftPwm(int m,int pr,int pl,int ps);    //constructor function
	int motorMove();
};

MotorSoftPwm::MotorSoftPwm(int m,int pr,int pl,int ps)		//constuctor function
{
	//wiring Pi initialize
	if(wiringPiSetupGpio() == -1){
	    std::printf("setup faied");
	    return;
	   }
	 //creates software controlled PWM pin
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
    	default:
    		softPwmWrite(RIGHTMOTOR1,0);
    		softPwmWrite(RIGHTMOTOR2,0);
    		softPwmWrite(LEFTMOTOR1,0);
    		softPwmWrite(LEFTMOTOR2,0);
			break;
	   }
	delay(pwmsecond *1000);
	  
	  return 0;
}
	
   		
    		



	 
	
	    
	  
	

	

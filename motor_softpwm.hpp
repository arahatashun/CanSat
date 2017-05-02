#ifndef MOTOR_SPWM
#define MOTOR_SPWM

//motor_softpwm.hpp
//motor softpwm class header file

class MotorSoftPwm {
	int mode, pwmvalue_r, pwmvalue_l, pwmsecond;
public:
	MotorSoftPwm(int m,int pr,int pl,int ps);    //constructor function
	int motorMove();
	int setpwm(int m,int pr,int pl,int ps);
};

#endif	  		
    		



	 
	
	    
	  
	

	

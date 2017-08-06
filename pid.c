#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "pid.h"

/*
   pwmの値を~100から100に調整
 */
int pid_limiter(Pid* pid_limits)
{
	if(pid_limits->output > 100)
	{
		pid_limits->output = 100;//pwm出力の最大値
	}
	else if(pid_limits->output < -100)
	{
		pid_limits->output = -100;//pwm出力の最小値(負)
	}
	return 0;
}

//outputを計算
int compute_output(Pid* pid_comp)
{
	unsigned int now = millis();
	double error = pid_comp->setpoint - pid_comp->input;//目標値ー入力値:偏差

	if(pid_comp->lastTime==0)//初回
	{
		int deltaT = 0;
		pid_comp->differential = 0;
		pid_comp->integral += error * deltaT;
	}
	else
	{
		int deltaT = now - pid_comp->lastTime;
		pid_comp->differential = (error-pid_comp->prev_error)/deltaT;
		pid_comp->integral += error * deltaT;
	}

	pid_comp->prev_error = error;//prev_errorを更新
	pid_comp->lastTime = now;//最後の時刻を更新
	double Kp_output = pid_comp->Kp * error;
	double Ki_output = pid_comp->Ki * pid_comp->integral;
	double Kd_output = pid_comp->Kd * pid_comp->differential;
	printf("Kp_output:%f\n",Kp_output);
	printf("Ki_output:%f\n",Ki_output);
	printf("Kd_output:%f\n",Kd_output);
	pid_comp->output = (int)(Kp_output+Ki_output+Kd_output);
	pid_limiter(pid_comp);
	return 0;
}

int pid_initialize(Pid* pid_init)
{
	pid_init->lastTime = 0;
	pid_init->prev_error = 0;
	pid_init->integral = 0;
	pid_init->differential = 0;
	return 0;
}

int pid_const_initialize(Pid* pid_init, double setpoint, double kp_value, double ki_value, double kd_value)
{
	pid_init->setpoint = setpoint;
	pid_init->Kp = kp_value;
	pid_init->Ki = ki_value;
	pid_init->Kd = kd_value;
	return 0;
}

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
	pid_comp->output = (int)(pid_comp->Kp * error +//propotional
	                         pid_comp->Ki * pid_comp->integral+//integral
	                         pid_comp->Kd * pid_comp->differential);//differential

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
	pid_data->setpoint = setpoint;
	pid_data->Kp = kp_value;
	pid_data->Ki = ki_value;
	pid_data->Kd = kd_value;
	return 0;
}
Pid *make_pid(void)
{
	Pid *pid = malloc(sizeof(Pid));
	if (pid != NULL)
	{
		pid_initialize(pid);
	}
	else
	{
		free(pid);
		printf("メモリ不足\n");
		return NULL;//segmantaion faultの可能性
	}
	return pid;
}

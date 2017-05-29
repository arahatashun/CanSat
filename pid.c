#include <wiringPi.h>
#include "pid.h"

/*
  pwmの値を~100から100に調整
*/
int pid_limiter(pid* pid_limits)
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
int compute_output(pid* pid_comp)
{
  unsigned int now = millis();
  error = pid_comp->setpoint - pid_comp->input;//目標値ー入力値:偏差

  if(pid_comp->lastTime==0)//初回
  {
    int deltaT = 0;
    pid_comp->differential = 0;
  }
  else
  {
    int deltaT = now -pid_comp->lastTime;
    pid_comp->differential = (error-pid_comp->prev_error)/deltaT;
  }

  pid_comp->integral += error * deltaT;
  pid_comp->prev_error = error;//prev_errorを更新
  pid_comp->lastTime = now;//最後の時刻を更新
  pid_comp->output = pid_comp->Kp * error +//propotional
                      pid_comp->Ki * pid_comp->integral+//integral
                        pid_comp->Kd * pid_comp->differential;//differential

  pid_limiter(*pid_comp);
  return 0;
}

int pid_initialize(pid* pid_init)
{
  pid_init->lastTime = 0;
  pid_init->prev_error = 0;
  pid_init->integral = 0;
  pid_init->differential = 0;
}

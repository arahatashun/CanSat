#include "ring_buffer.h"


typedef struct {
    double input;//入力値
    double setpoint;//目標値
    int output; //操作量(milliseconds)
    int dt; //時間差
    double Kp;//propotional constant
    double Ki; //integral constant
    double Kd;//differential constant
    double integral;
    double differential;
    Queue *delta_queue = make_queue(6);
} pid;

int compute_output(pid* pid_comp)
{
  double delta = pid_comp->setpoint - pid_comp->input;//目標値ー入力値
  enqueue(pid_comp->delta_queue, delta);
  pid_comp->integral = 0;
  int i;
  for (i=0;i<=queue_length(pid_comp->delta_queue)-1;i++)
  {
    pid_comp->integral += pid_comp->delta_queue->buff[pid_comp->delta_queue->front+i];
  }

  pid_comp->differential = delta - pid_comp->delta_queue->buff[pid_comp->delta_queue->front];
  /*
  pid_comp->integral = pid_comp->integral + delta;
  pid_comp->differential = delta - pid_comp->differential;
  */
  pid_comp->output = pid_comp->Kp * pid_comp->dt +
                      pid_comp->Ki * pid_comp->integral * pid_comp->dt +
                      + pid_comp->Kd * pid_comp->differential / pid_comp->dt;
  if (is_full(pid_comp->delta_queue))
  {
    dequeue(pid_comp->delta_queue);
  }
  return 0;
}

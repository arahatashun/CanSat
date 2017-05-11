#include "motor.h"
#include <signal.h>
#include <stdlib.h>

void handler(int signum)
{
  motor_stop(10);
  exit(1);
}


int main()
{
  signal(SIGINT, handler);
  pwm_initializer();
  motor_forward(10000, 60);
  motor_left(500,50);
  motor_right(500,50);
}

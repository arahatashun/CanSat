
#include <signal.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "motor.h"

void handler(int signum)
{
  motor_stop();
  exit(1);
}


int main()
{
  signal(SIGINT, handler);
  pwm_initializer();
  motor_forward(60);
  delay(50000)
  motor_left(50);
  delay(50000)
  motor_right(50);
  delay(50000)
}

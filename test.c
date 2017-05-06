#include <math.h>
#include "motor.h"
#include "compass.h"
#include "mitibiki.h"

double course;//current angle
double angle_to_go;
double delta_angle;//the difference between current angle and angle to go

int sensor_initializer()
{
  mitibiki_initializer();
  pwm_initializer();
  compass_initializer();
  return 0;
}

int update_angle()
{
  course = compass_get_angle();
  angle_to_go = target_angle();
  delta_angle = course - angle_to_go;
  return delta_angle;
}

int decide_route()
{
  while(0<=update_angle() && update_angle()<=180)
  {
    motor_left(50,40);
  }
  while(180<update_angle() && update_angle()<=360)
  {
    motor_right(50,40);
  }
  while(-180<update_angle() && update_angle()<0)
  {
    motor_right(50,40);
  }
  while(-360<=update_angle() && update_angle()<=-180)
  {
    motor_left(50,40);
  }
  return 0;
}

int main()
{
  sensor_initializer();
  while (1)
  {
    decide_route();
  }
  return 0;
}

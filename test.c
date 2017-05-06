#include <math.h>
#include "motor.h"
#include "compass.h"
#include "mitibiki.h"

double course;//current angle
double target_angle;
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
  course = get_angle();
  target_angle = target_angle();

  return 0;
}

int decide_route()
{

}

int main()
{
  sensor_initializer();
  while (1)
  {

  }
}

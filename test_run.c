#include <math.h>
#include "motor.h"
#include "compass.h"

static const int target_angle = 180;


int main()
{
  pwm_initializer();
  compass_initializer();
  while (1)
  {
    while (compass_get_angle()-target_angle<30 && compass_get_angle()-target_angle>-30)
    {
      motor_forward(5000,70);
    }
    while(compass_get_angle()-target_angle>=30)
    {
      motor_left(50,40);
    }
    while(compass_get_angle()-target_angle<=-30)
    {
      motor_right(50,40);
    }
  }
  return 0;
}

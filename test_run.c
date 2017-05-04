#include <math.h>
#include "motor_run.h"
#include "compass.h"

static const int target_angle = 180;


int main()
{
  pwm_initializer();
  compass_initializer();
  while (true)
  {
    while (get_angle()-target_angle<30 && get_angle()-target_angle>-30)
    {
      motor_forward(10000,70);
    }
    while(get_angle()-target_angle>=30)
    {
      motor_left(50,40);
    }
    while(get_angle()-target_angle<=-30)
    {
      motor_right(50,40);
    }
  }
  return 0;
}

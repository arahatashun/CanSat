#include "motor.h"

int main()
{
  pwm_initializer();
  motor_forward(100, 60);
}

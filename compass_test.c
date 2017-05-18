#include "compass.h"

int main()
{
	compass_initializer();
	while (1)
  {
		double angle;
		compass_get_angle(&angle);
	}
}

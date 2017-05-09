#include "compass.h"

int main()
{
	compass_initializer();
	while (1)
  {
		compass_get_angle();
	}
}

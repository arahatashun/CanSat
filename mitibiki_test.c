#include "mitibiki.h"

int main()
{
  mitibiki_initializer();
  while (1)
  {
    target_gps_angle();
    get_distace();
  }
}

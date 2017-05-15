#include "mitibiki.h"

int main()
{
  mitibiki_initializer();
  for(i=0;i<100;i++)
  {
    target_gps_angle();
    get_distace();
  }
}

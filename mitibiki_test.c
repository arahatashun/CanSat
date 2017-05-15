#include "mitibiki.h"

int main()
{
  mitibiki_initializer();
  int i=0;
  for(i=0;i<100;i++)
  {
    target_gps_angle();
    get_distace();
  }
}

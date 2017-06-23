#include <wiringPi.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "../gut.h"

void gutHandler(int signum)
{
	cutInterupt();
	delay(100);
	exit(1);
}


int main()
{
	signal(SIGINT, gutHandler);
	cut_initialize();
	cut();
	return 0;
}

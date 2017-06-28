#include <stdio.h>
#include <stdlib.h>

int main()
{
	int value = system("raspistill -o hoge.jpg");
	printf("%d\n",value);
	return 0;
}

#include <unistd.h>
#include <linux/reboot.h>
#include <errno.h>

int main()
{
	sync();
	if(reboot(LINUX_REBOOT_CMD_RESTART)== -1)
	{
		printf("WARNING! REBOOT FAILED\n");
		printf("errno=%d: %s\n",errno, strerror(errno));
		return -1;
	}
	return 0;
}

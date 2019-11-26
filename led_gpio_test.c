#include "stdio.h"
#include "fcntl.h"
#include "unistd.h"

int main(int argc, char *argv[])
{
	int ret;
	int fd;
	int val = 0;

	if(argc != 2)
	{
		printf("error, Please input 1 argument\n");
		return -1;
	}
	fd = open("/dev/led0", O_WRONLY);
	if(fd < 0)
	{
		printf("open failed\n");
		return -1;
	}

	if(strcmp(argv[1], "on") == 0)
	{
		val = 1;
		printf("led on\n");
	}
	else if( strcmp(argv[1],"off") == 0 )
	{
		val = 0;
		printf("led off\n");
	}
	write(fd, &val, 4);

	close(fd);

	return 0;
}

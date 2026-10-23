#include <stdio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define LED_ON _IO('f',1)
#define LED_OFF _IO('f',2)

int main(int argc, char *argv[])
{
	int fd = open("/dev/testchar", O_RDWR);
	int val = atoi(argv[1]);
	if(fd < 0) {
		printf("open: %s\n", strerror(errno));
		return -1;
	}

	if(1 == val){
        if(ioctl(fd, LED_ON, 1) < 0) {
                printf("ioctl: %s\n", strerror(errno));
                return -1;
        }
	}
	
	if(0 == val){
        if(ioctl(fd, LED_OFF, 0) < 0) {
                printf("ioctl: %s\n", strerror(errno));
                return -1;
        }
	}


	printf("ioctl success\n");
	close(fd);
	return 0;
}

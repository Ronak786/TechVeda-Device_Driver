#include <stdio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define  CMD1 _IO('f',1)
#define  CMD2 _IO('f',2)

int main()
{
	int fd = open("/dev/testchar3", O_RDWR);
	if(fd < 0) {
		printf("open: %s\n", strerror(errno));
		return -1;
	}

        if(ioctl(fd, CMD1, 20) < 0) {
                printf("ioctl: %s\n", strerror(errno));
                return -1;
        }
	
	
        if(ioctl(fd, CMD2, 40) < 0) {
                printf("ioctl: %s\n", strerror(errno));
                return -1;
        }


	printf("ioctl success\n");
	close(fd);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>

//I2c Address
#define I2C_BUS_ADDR   (0x3)
#define I2C_SLAVE_ADDR (0x37)
#define PORTMAX        (24)
#define INDEXMAX       (8)

int main(int argc, char *argv[])
{
	char devname[64];
	int fd;
	int cmd;
	
	sprintf(devname,"/dev/i2c-%d",I2C_BUS_ADDR);
		
	if ((fd = open(devname, O_RDWR|O_SYNC)) < 0) {
		perror(devname);
		exit(1);
	}
	
	if (ioctl(fd, I2C_SLAVE, I2C_SLAVE_ADDR) < 0) {
		fprintf(stderr, "ioctl(I2C_SLAVE,%d): %s\n", I2C_SLAVE_ADDR, strerror(errno));
		exit(1);
	}
	
	int err;
	int val;
	int addr;
	int i;	
	addr=0;
	for(i=0;i<16;i++){
		val = i2c_read(fd,addr+i);
		//i2c_write(fd,addr+i,0xFF-i);
		printf("%02x ", val);
	}

	close(fd);

	return 0;
}

PWD   := $(shell pwd)
obj-m += i2cadap.o
obj-m += i2cdev.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

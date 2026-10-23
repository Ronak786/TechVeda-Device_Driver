obj-m += callback1.o

PWD   := $(shell pwd)
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD)
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

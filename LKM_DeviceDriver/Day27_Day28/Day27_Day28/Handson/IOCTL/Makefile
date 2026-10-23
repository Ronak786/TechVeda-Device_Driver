PWD   := $(shell pwd)
obj-m += ioctdrv1.o
obj-m += ioctdrv2.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) 
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm -rf ioctlapp1

app:
	gcc -g -o ioctlapp1 ioctlapp1.c

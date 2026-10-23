PWD   := $(shell pwd)
obj-m += platdtb1.o

all:
	make -C /home/test/qemu/linux-5.1.16/ M=${PWD} ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

clean:
	make -C /home/test/qemu/linux-5.1.16/ M=${PWD} ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- clean

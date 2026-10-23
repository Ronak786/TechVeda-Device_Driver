PWD   := $(shell pwd)
obj-m += gpio1.o
obj-m += gpio2.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

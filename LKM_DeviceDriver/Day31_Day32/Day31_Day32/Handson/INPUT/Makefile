PWD   := $(shell pwd)
obj-m += input1.o
obj-m += input2.o
obj-m += input3.o
obj-m += input4.o
obj-m += input5.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

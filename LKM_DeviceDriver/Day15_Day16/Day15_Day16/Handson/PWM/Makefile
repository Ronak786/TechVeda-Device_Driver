PWD   := $(shell pwd)
obj-m += fake-pwm.o
obj-m += pwm-ins.o 

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

PWD   := $(shell pwd)
obj-m += myspidev.o

all: dt
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD)

dt: myspidev.dts
	dtc -@ -I dts -O dtb -o myspidev.dtbo myspidev.dts

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm -rf myspidev.dtbo

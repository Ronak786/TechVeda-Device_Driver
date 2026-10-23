PWD   := $(shell pwd)
obj-m += char1.o
obj-m += char2.o
obj-m += char3.o
obj-m += char4.o


all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD)
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm -rf char2_test

app :
	gcc -g -o char2_test char2_test.c

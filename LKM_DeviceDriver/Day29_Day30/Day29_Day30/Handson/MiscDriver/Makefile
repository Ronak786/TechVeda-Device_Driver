PWD   := $(shell pwd)
obj-m += misc1.o
obj-m += misc2.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD)

app:
	gcc -g -o miscapp1 miscapp1.c

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm -rf miscapp1

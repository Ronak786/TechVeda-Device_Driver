PWD   := $(shell pwd)
obj-m += spissd1306.o
spissd1306-objs := spi_ssd1306_driver.o ssd1306.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

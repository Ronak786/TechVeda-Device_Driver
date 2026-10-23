PWD   := $(shell pwd)
obj-m += spissd1306.o
spissd1306-objs := spi_ssd1306_driver.o ssd1306.o

all: dt
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD)

dt: myssd1306.dts
	dtc -@ -I dts -O dtb -o myssd1306.dtbo myssd1306.dts


clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

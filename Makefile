KERN_DIR=/home/yang/linux-rpi-4.19.y
all:
	make -C $(KERN_DIR) M=`pwd` modules ARCH=arm CROSS_COMPILE=/home/yang/tools-master/arm-bcm2708/arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  modul* modules.order
obj-m += led_gpio.o

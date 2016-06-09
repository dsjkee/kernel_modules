obj-m:=my_usb.o
all:
		make -C /lib/modules/3.19.0-15-generic/build M=$(PWD) modules
#		sudo mknod -m0666 /dev/my_dev c 254 0
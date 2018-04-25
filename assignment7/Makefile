obj-m := usbkbd.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd)

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~

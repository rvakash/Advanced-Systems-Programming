obj-m := assignment6.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd)


deadlock1: 
	gcc deadlock1.c -o deadlock1 -pthread
	
deadlock2: 
	gcc deadlock2.c -o deadlock2 -pthread

deadlock3: 
	gcc deadlock3.c -o deadlock3 -pthread

deadlock4: 
	gcc deadlock4.c -o deadlock4 -pthread

deadlock5: 
	gcc deadlock5.c -o deadlock5 -pthread

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~

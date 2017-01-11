obj-m += hook.o

all:
	- sudo rmmod hook.ko
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod hook.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

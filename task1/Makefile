CONFIG_MODULE_SIG=n
obj-m += phonebook.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
instil:
	sudo insmod phonebook.ko
	dmesg | tail -1
remove:
	sudo rmmod phonebook.ko
	dmesg | tail -1

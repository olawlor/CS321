# This is the name of our module
MOD = hello

# This points to your kernel's makefile:
MAKE = make -C /lib/modules/$(shell uname -r)/build M=$(PWD) 

# This tells your kernel's makefiles how to build our stuff:
obj-m += $(MOD).o


all: run

$(MOD).ko: $(MOD).c
	$(MAKE) modules

run: $(MOD).ko
	sudo insmod $(MOD).ko
	sudo rmmod $(MOD).ko
	sudo dmesg | tail -10

clean:
	$(MAKE) clean


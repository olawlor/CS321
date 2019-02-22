# This is the name of our module
MOD = proc

# This points to your kernel's makefile:
MAKE = make -C /lib/modules/$(shell uname -r)/build M=$(PWD) 

# This tells your kernel's makefiles how to build our stuff:
obj-m += $(MOD).o


all: run

$(MOD).ko: $(MOD).c
	$(MAKE) modules

run: $(MOD).ko
	sudo dmesg -C   # clear old kernel messages
	sudo insmod $(MOD).ko
	cat /proc/foo
	sudo rmmod $(MOD)
	sudo dmesg 

clean:
	$(MAKE) clean


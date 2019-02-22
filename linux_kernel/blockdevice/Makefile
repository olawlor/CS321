# This is the name of our module
MOD = blocky

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
	echo "- - U *" | sudo sfdisk -X gpt /dev/blocky0
	sudo partprobe /dev/blocky0
	sudo mkfs.fat -F 32 /dev/blocky0p1
	mkdir -p tmp
	sudo mount /dev/blocky0p1 tmp
	sudo touch tmp/filezzzz
	sudo ls -al tmp/
	sudo umount tmp
	sudo rmmod $(MOD)
	sudo dmesg 

clean:
	$(MAKE) clean


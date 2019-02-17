# Makefile to build an EFI operating system drive image.

# This is a local copy of gnu-efi
GNU_EFI_CFLAGS=-Iinclude/efi -Iinclude/efi/x86_64
GNU_EFI=lib_efi

WARNINGS=-Wall 
OPTIMIZE=-O1

# Dark magic compile and link flags to build EFI binary with g++
CFLAGS=$(WARNINGS) $(OPTIMIZE)	\
  	-DEFI_FUNCTION_WRAPPER				\
    -I.  -Iinclude  -fno-stack-protector					 \
		-fPIC                      	\
		-fshort-wchar               \
		-mno-red-zone								\
		$(GNU_EFI_CFLAGS) 

LDFLAGS=	$(GNU_EFI)/crt0-efi-x86_64.o		 \
		 -nostdlib								\
		 -znocombreloc						\
		 -T $(GNU_EFI)/elf_x86_64_efi.lds \
		 -shared								\
		 -Bsymbolic							 \
		 -L $(GNU_EFI)					 \
		 -l:libgnuefi.a						 \
		 -l:libefi.a							

OFLAGS=-j .text						\
		-j .sdata					 \
		-j .data						\
		-j .dynamic				 \
		-j .dynsym					\
		-j .rel						 \
		-j .rela						\
		-j .reloc					 \
		--target=efi-app-x86_64 

KERNEL=kernel.efi
DRIVE=my_efi.hdd


all: run

# Needs qemu and "bios-256k.bin" from OVMF (EFI firmware)
run: $(DRIVE)
	qemu-system-x86_64 -L . -drive format=raw,file=$(DRIVE) -m 256

# This is the main compile line, to build the kernel:
$(KERNEL): main.cpp
	g++ $<	$(CFLAGS) -c -o main.o
	ld main.o	$(LDFLAGS) -o main.so
	objcopy $(OFLAGS)	main.so	$(KERNEL)


# This copies the kernel to a FAT16 filesystem on a superfloppy disk image
$(DRIVE):  $(KERNEL)
	dd if=/dev/zero of="$@" bs=1k count=2880
	mformat -i "$@" -f 2880 ::
	mmd -i "$@" ::/EFI
	mmd -i "$@" ::/EFI/BOOT
	mcopy -i "$@" $< ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i "$@" APPS  ::/


# This can make a FAT32 filesystem, 
# but seems to work the same as mformat.
$(DRIVE)_fat32: $(KERNEL)
	dd if=/dev/zero of=$@ bs=1024k count=40
	- sudo losetup /dev/loop9 $@
	- echo "- - U *" | sudo sfdisk -X gpt /dev/loop9
	sudo partprobe /dev/loop9
	sudo mkfs.fat -F 32 /dev/loop9p1
	sudo mkdir -p tmp
	sudo mount -o loop /dev/loop9p1 tmp
	sudo mkdir -p tmp/EFI/BOOT
	sudo cp $(KERNEL) tmp/EFI/BOOT/BOOTX64.EFI
	sudo cp -r APPS tmp/
	sudo umount tmp
	sudo losetup -d /dev/loop9

clean:
	- rm $(KERNEL) main.so main.o

apt-get:
	sudo apt-get install g++ build-essentials gnu-efi  mtools  qemu-system-x86 ovmf


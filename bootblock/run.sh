#!/bin/sh
# Compile and run boot block
src="$1"
nasm -f bin "$src" -o boot.hdd

od -A x -t x1 boot.hdd  > boot.hex    # hex dump, for good measure
ndisasm -b 16 boot.hdd  > boot.dis

qemu-system-i386  -drive format=raw,file=boot.hdd

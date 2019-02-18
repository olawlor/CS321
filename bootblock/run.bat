"C:\Program Files\nasm\nasm.exe" -f bin %1  -o boot.hdd
"C:\Program Files\qemu\qemu-system-i386.exe" boot.hdd

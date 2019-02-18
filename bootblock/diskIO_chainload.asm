;
;  Tiny boot block example
;  Dr. Orion Lawlor, lawlor@alaska.edu, 2019-02-01 (Public Domain)

BITS 16 ; awful segmented memory
ORG 0x07C00 ; bios loads this boot block to linear address 0x07C00 

start:
  cli ; disable interrupts until we're set up
	; Zero out all the segment registers
	xor ax, ax ; ax=0
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0xfff0 ; put stack pointer at end of our 64K area
	sti ; allow interrupts now that everything's set up

  mov al,'O'
  call printchar
  
  call read_disk
  
  call start_sector1
  
hang: jmp hang  ; wait here (forever)


read_disk:
  mov ah,0x42
  mov si, my_DAP
  ; mov dl, 0x80 ; C drive (in a boot sector, the BIOS loads dl with the boot drive)
  int 0x13 ; disk I/O
  ret

my_DAP:
  db 0x10 ; size
  db 0 ; padding
  dw 10 ; sectors to read
  dw 0x7e00 ; memory target offset
  dw 0 ; memory target segment
  dd 1, 0 ; read this sector (64-bit constant in two pieces)

;***** Utility: Text output functions **********
; Print one character, passed in al.
;   Trashes: bx
printchar:
	; See http://stanislavs.org/helppc/int_10-e.html
	mov bx, 0x0007  ; 'output character' selector
	mov ah, 0x0e; Output a character
	int 0x10  ; "output character" INT 10,E
	ret
	
	
; pad to make this boot sector exactly 512 bytes long total
	times 512-2-($-start) db 0x90  

; A boot sector's last two bytes must contain this magic flag.
	dw 0xAA55 ; Magic value needed by BIOS
; ------- end boot sector -----
; Sector 1: 0x7c00 + 512 (=0x7e00)
start_sector1:

  mov al,'K'
  call printchar
  
	mov si,print_str
  str_loop:
    mov al,BYTE [si]
    cmp al,0
    je hang
    call printchar
    add si,1
    jmp str_loop
    
  
print_str:
	db ' This is code from SECTOR 1, loaded and operational!',0


; VirtualBox wants a .hdd image to be padded to full 512 byte blocks
	times 512-(($-start_sector1)&0x1ff) db 0x90  



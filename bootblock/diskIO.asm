;
;  Tiny boot block disk linear addressing example
;  Dr. Orion Lawlor, lawlor@alaska.edu, 2019-01-31 (Public Domain)

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
  
  ; Do disk read using linear block addressing: BIOS INT 13,42: 
  ;    https://en.wikipedia.org/wiki/INT_13H  
  mov ah,0x42             ;function number for LBA disk read
  mov si,read_DAP         ;memory location of Disk Address Packet (DAP)
  
  ; The boot drive is actually loaded to register dl on startup
  ; mov dl,0x80             ;drive (0x80 is hard drive, 0 is A:, 1 is B:)
  int 0x13                ;read the disk
  
  ; If that read worked, we just read disk data to this memory
  mov al,[0x9000]
  call printchar
  
hang: jmp hang  ; wait here (forever)


; Disk Address Packet (DAP) identifies the data to read.
read_DAP:
  db 0x10 ; Size of this DAP in bytes
  db 0 ; unused
  dw 1 ; sector read count
  dw 0x9000 ; memory target: offset
  dw 0 ; memory target: segment
  dd 1, 0 ; 0-based disk sector number to read (64-bit constant)


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

; End of boot sector 0
; -----------------------------------------------------
; Start of sector 1 (immediately follows boot sector on disk)
sector1_start:
  db 'K'  

; Round up size to full disk sector (VirtualBox wants full 512-byte sectors for a .hdd file)
times (512 - ($-sector1_start))  db 0x90 



  

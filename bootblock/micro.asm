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

  mov al,'!'
  call printchar
  
hang: jmp hang  ; wait here (forever)


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


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

  ; Set up our own interrupt handler for INT 0, which is divide-by-zero
  mov WORD[0], my_divide0 ; offset to handler code
  mov WORD[2], 0 ; segment number of handler
  
  sti ; allow interrupts now that everything's set up
  
  ; Do a demo divide-by-zero
  mov cx,0
  idiv cx
  
  mov al,'!' ; if we get here, something's weird
  call printchar
  
hang: jmp hang  ; wait here (forever)


; The CPU jumps to this interrupt handler:
my_divide0:
  mov al,'D'
  call printchar
  jmp hang ; <- it's not easy to resume from a divide-by-zero
  
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


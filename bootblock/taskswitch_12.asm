;
;  Tiny boot block example: switch between two tasks, one time.
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
  
  mov WORD[0+4*0x8], switch ; offset to timer code
  mov WORD[2+4*0x8], 0 ; segment 0
  
  sti ; allow interrupts now that everything's set up

  mov al,'O'
  call printchar
  
process1: 
  mov al,'_'
  call printchar
  call slowdown
  jmp process1  ; keep running (forever)

process2: 
  mov al,'b'
  call printchar
 process2_loop:
  mov cx,0
  mov al,'B'
  call printchar
  call slowdown
  jmp process2_loop ; keep running (forever)


whichprogram:
  dw 0

switch:
  pusha 
  mov al,'!'
  call printchar
  
  ; Tell timer hardware to give us more interrupts
  mov al,0x20 ; End-of-interrupt
  out 0x20, al ; pic
  sti ; tell CPU to accepts more interrupts
  
  mov cx,WORD[whichprogram]
  add cx,1
  and cx,1 ; low bit of cx
  mov WORD[whichprogram],cx
  cmp cx,0
  
  je do1
    jmp process2
  do1:
    jmp process1

;***** Utility: Text output functions **********
; Print one character, passed in al.
;   Trashes: bx
printchar:
  ; See http://stanislavs.org/helppc/int_10-e.html
  mov bx, 0x0007  ; 'output character' selector
  mov ah, 0x0e; Output a character
  int 0x10  ; "output character" INT 10,E
  ret

; Burn some time (just to make the display visible)
slowdown:
  mov edx,10000000
  .slowdown_loop:
    dec edx
    cmp edx,0
    jne .slowdown_loop
  ret

; pad to make this boot sector exactly 512 bytes long total
  times 512-2-($-start) db 0x90  

; A boot sector's last two bytes must contain this magic flag.
  dw 0xAA55 ; Magic value needed by BIOS


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
  
  mov WORD[0+4*0x1C], my_int_handler ; handler code offset
  mov WORD[2+4*0x1C], 0 ; handler segment is zero
  
  sti ; allow interrupts now that everything's set up

  mov al,'O'
  call printchar
  
  ;mov cx,0
  ;idiv cx ; divide by zero here
  ; int 0x20
hang: 
  mov al,'_'
  call printchar
  call slowdown
  jmp hang  ; wait here (forever)

hang2: 
  call slowdown
  mov al,'B'
  call printchar
  call slowdown
  jmp hang2 ; wait here (forever)


slowdown:
  mov edx,10000000
  .slowdown_loop:
    dec edx
    cmp edx,0
    jne .slowdown_loop
  ret

; Interrupt Handler
; "Interrupt Service Routine"
my_int_handler:
  pusha ; ax - dx, bp, si, di (but NOT cs, ds, etc)
  push ds
  mov ax,0
  mov ds,ax
  
  mov cx,100
  repeat_ex:
    mov al,'!'
    call printchar
    sub cx,1
    jne repeat_ex
  
  pop ds
  popa
  
  ; Overwrite iret's return address to switch tasks:
  ;    Stack before: code offset, code segment, flags
  sub sp,4; <- worth 2 pops
  push 0 ; new code segment
  push hang2 ; new code offset
  
  iret ; pops where, code segment, flags


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


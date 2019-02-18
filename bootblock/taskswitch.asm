;
;  Tiny boot block example: switch between two tasks on timer interrupt.
;  Dr. Orion Lawlor, lawlor@alaska.edu, 2019-02-07 (Public Domain)

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
  
  ; Install a timer interrupt handler to switch processes
  mov WORD[0+4*0x8], timer_switch_processes ; handler code offset
  mov WORD[2+4*0x8], 0 ; handler segment is zero
  
  ; Create two processes, 1 and 2
  mov si,process1_task
  mov ax,process1
  call create_process
  
  mov si,process2_task
  mov ax,process2
  call create_process
  
  sti ; allow interrupts now that everything's set up
  
  jmp process1 ; start in process1

process1: 
  mov al,'_'
  call printchar
  call slowdown
  jmp process1  ; keep running (forever)

; process_struct data layout:
;    DWORD offset 0: stack pointer
;    DWORD offset 4: pointer to next task_struct

; This data structure is used by timer_switch_process
process1_task:
  dd 0x6ff0 ; stack pointer for process1
  dd process2_task ; link to next process

process2: 
  mov al,'b'
  call printchar
  call slowdown

  process2_loop:
    mov al,'B'
    call printchar
    call slowdown
    call slowdown
    jmp process2_loop ; keep running (forever)

; This data structure is used by timer_switch_process
process2_task:
  dd 0x5ff0 ; stack pointer for process2
  dd process1_task ; link to next process (circular list)

; This global variable points to the current process task struct:
current_process:
  dd process1_task

; Timer interrupt handler: switches processes
timer_switch_processes:
  pushad ; save old process's registers to old stack
  mov ax,0
  mov ds,ax ; zero out data segment
  
  mov al,'!'
  call printchar
  
  mov esi,DWORD[current_process] ; points to current task_struct
  
  mov DWORD[si],esp ; save old stack pointer
  mov esi,DWORD[si+4]; current = current->next
  mov esp,DWORD[si] ; bring up new stack pointer
  
  mov DWORD[current_process], esi ; save the new current process
  
  ; Send the 8259 PIC an End-of-Interrupt (timer interrupt hardware magic)
  mov al,0x20
  out 0x20, al
  
  popad ; restore new process's registers from new stack
  
  iret ; pops where, code segment, and flags for new process


; This initializes a process task struct (in si) 
;   to start running the process code passed in ax
create_process:
  mov bp,sp ; save the old stack
  
  mov esp,DWORD[si]; load up the new process's stack
  ; Set up this process's stack so the timer_switch pops will take us to ax
  push 0x0200|0x0002 ; flags register: turn on interrupts, reserved bit
  push 0 ; code segment
  push ax ; code offset
  pushad ; saved registers
  ; the pops happen during the first context switch
  mov DWORD[si],esp; save the new process's stack, now that it's set up
  
  mov sp,bp ; restore the old stack
  ret


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


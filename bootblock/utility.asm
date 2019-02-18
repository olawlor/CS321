;  Example x86 PC bootsector program 
;  Designed for the Netwide assembler, "nasm".
;  Compile into a boot block with "nasm -f bin boot.s -o boot.bin"
;  Dr. Orion Lawlor, lawlor@alaska.edu, 2019/01/22 (Public Domain)

BITS 16         ; Starts running in 16-bit segmented DOS mode
ORG 0x7c00    ; BIOS loads this boot block code to offset 0x7c00

_start: ; BIOS loads this code, and starts running here, at the beginning
	cli ; turn off interrupts until we get the machine running
	cld ; make string instructions run normal direction (+)

	; Zero out all the segment registers
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp,  0xfff0 ; stack pointer starts at end
  sti ; restore interrupts

	mov si,splashstring
	call printstrln

cmdloop:
	
	mov si,commandprompt
	call printstr
	
	call readchar
	call println

; Print hello string: h
	cmp al,'h'
	jne not_hellworld
    mov si,hellstr
    call printstrln
    jmp cmdloop
	not_hellworld:

; Simple read-hex example: x
	cmp al,'x'
	jne not_readhex
	  call readhex
	  call printhex32
	  call println
	  jmp cmdloop
	not_readhex:
	
; Dump all register values: d
	cmp al,'d'
	jne not_debugdump
	  call debugdump
	  jmp cmdloop
	not_debugdump:

; Print bytes of memory: m <hex segment> <hex offset>
  cmp al,'m'
  jne not_printmem
	  call readhex ; segment first
	  mov es,ax ; stash segment in es
	  call println
	  
	  call readhex ; then offset
	  mov si,ax ; offset is in si
	  call println
	  
	  ; Dump a row of bytes:
	  mov cx,0
	  start_print:
	    push cx
	    mov al, BYTE [es:si] ; load byte from memory
	    call printhex8
	    mov al, ' '
	    call printchar
	    pop cx
	    
	    inc si
	    inc cx
	    cmp cx,16
	    jl start_print
	  
	  call println
	  
	  jmp cmdloop
  not_printmem:

; Write to memory: w <hex segment> <hex offset> <hex byte value>
cmp al, 'w'
  jne not_writemem
	  call readhex ; segment first
	  mov es,ax ; stash segment in es
	  call println
	  
	  call readhex ; then offset
	  mov si,ax ; offset is in si
	  call println
	  
	  call readhex ; value
	  mov BYTE[es:si],al ; write value to segment:offset
    
    jmp cmdloop
  not_writemem:
	
; s: Trash the stack segment
	cmp al,'s'
	jne not_trashstack
	  call readhex ; read a segment
	  mov ss,ax ; that's the new stack segment
    jmp cmdloop
	not_trashstack:
	
	
; If we haven't figured it out yet by now, it's bad.
	mov si,cmd_error_str
	call printstrln
	
	jmp cmdloop


; Setup string:
splashstring:
  db "-Bare Metal-",0

; Command prompt:
commandprompt:
  db "C:\> ",0

; Generic error string
cmd_error_str:
  db "Command?",0

; Simple string demo
hellstr:
  db "Hell World!",0



; ------------ Simple utility functions (call/ret interface) -------------
; Print one character, passed in al.
;   Trashes: none
printchar:
  push ax
  push bx
	mov bx, 0x0007  ; 'output character' selector
	mov ah, 0x0e; Output a character
	int 0x10  ; "output character" interrupt
	pop bx
	pop ax
	ret

; Print a string, starting address in si
;   Trashes: si
printstr:
  lodsb ; = mov al, BYTE [ds:si]
  cmp al,0 
  je print_done
  call printchar
  jmp printstr
  print_done: ret

; Print a string (from the pointer si), then a newline
printstrln:
  call printstr
  ; no ret, so falls through!
println: ; print just the newline
  push ax
  mov al,13 ; CR
  call printchar
  mov al,10 ; LF
  call printchar
  pop ax
  ret

; Dump all registers for debugging
;  trashes: none
debugdump:
  push ax
  push bx
  
  mov bx,'ax' 
  call printreg
  
  pop ax ; grab pushed copy of bx
  push ax ; re-save the copy
  mov bx,'bx'
  call printreg
  
  mov ax,cx
  mov bx,'cx'
  call printreg
  
  mov ax,dx
  mov bx,'dx'
  call printreg

  call println
  
  mov ax,sp
  mov bx,'sp'
  call printreg
  
  mov ax,ss
  mov bx,'ss'
  call printreg
  
  mov ax,cs
  mov bx,'cs'
  call printreg
  
  mov ax,ds
  mov bx,'ds'
  call printreg
  
  call println
  
  pop bx
  pop ax
  ret

; Input: bl = char name of register.  ax = value of register
printreg:
  push ax
  push bx
  mov al,bl
  call printchar
  pop bx
  mov al,bh
  call printchar
  mov al,'='
  call printchar
  pop ax
  call printhex32
  mov al,' '
  call printchar
  ret

; Print eax as 8 hex digits onscreen
;  Trashes: none
printhex32:
  push cx
  mov cl,8*4 ; bit counter: print 8 hex digits, at 4 bits each
  jmp start
printhex8: ; print al as 2 hex digits onscreen
  push cx
  mov cl,2*4 ; 2 hex digits
start:
    sub cl,4 ; move down 1 hex digit = 4 bits
    push eax
    
    ; print hex digit from bit number cl
    shr eax,cl ; shift ax's high bits down first
    and al,0xF ; extract the low hex digit
    cmp al,9
    jbe print_number_only
      add al,'A'-10-'0' ; print as ASCII letter
    print_number_only:
    add al,'0' ; convert number to ascii
    call printchar
    
    pop eax
    cmp cl,0
    jne start ; repeat until cl == 0
  
  pop cx
  ret

; Read one char from the user, return ASCII in al / scancode in ah
;  Trashes: returns in ax
readchar:	
  xor ax, ax; set ax to zero (wait for keypress)
	int 0x16  ; Wait for a keypress
	call printchar ; Always echo user input
	ret

; Read hex digits from the user until hits spacebar, return value in eax
;  Trashes: ax, dx
readhex:
  xor edx, edx ; value we're accumulating
  
  read_hex_loop:
    call readchar
    cmp al,' ' ; we hit a space character
    je read_hex_done
    cmp al,13 ; we hit a (DOS) newline character
    je read_hex_done
    
    ; Else it's a real char: start by shifting old chars up
    shl edx,4 ; 4 bits per hex char
    
    cmp al,'9'
    jle read_hex_number
       ; else it's a letter
       cmp al,'F'
       jle read_hex_uppercase
        ; lowercase
        sub al,'a'-10
        jmp read_hex_add
       
       read_hex_uppercase:
        ; uppercase
        sub al,'A'-10
        jmp read_hex_add
      
      read_hex_number: ; it's a number
      sub al,'0'
    read_hex_add:
      movzx eax,al ; sign-extend al from 8 bits to 16 bits
      add edx,eax
      jmp read_hex_loop
  
read_hex_done:
  mov eax,edx ; return result in eax
  ret

; Restart the machine, by jumping to the BIOS's power-on code.
reboot:
	jmp  0xffff:0x0000 ; Jump to BIOS reset code

; A boot sector's last two bytes must contain this magic flag.
; This tells the BIOS it's OK to boot here; without this, 
;  you'll get the BIOS "Operating system not found" error.
	times 510-($-_start) db 0  ; pad to offset 510 
	dw 0xAA55 ; Magic value needed by BIOS


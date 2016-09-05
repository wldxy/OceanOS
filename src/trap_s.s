[GLOBAL idt_to_idtr]    ; Allows the C code to call idt_flush().
idt_to_idtr:
    mov eax, [esp+4]  ; Get the pointer to the IDT, passed as a parameter.
    lidt [eax]        ; Load the IDT pointer.
    ret
.end:

%macro ISR_NOERRCODE 1  ; define a macro, taking one parameter
[GLOBAL isr%1]        ; %1 accesses the first parameter.
isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
[GLOBAL isr%1]
isr%1:
    cli
    push byte %1
    jmp isr_common_stub
%endmacro

ISR_NOERRCODE  0        ; 0 - Division by zero exception
ISR_NOERRCODE  1        ; 1 - Debug exception
ISR_NOERRCODE  2        ; 2 - Non maskable interrupt
ISR_NOERRCODE  3        ; 3 - Breakpoint exception
ISR_NOERRCODE  4        ; 4 - Into detected overflow
ISR_NOERRCODE  5        ; 5 - Out of bounds exception
ISR_NOERRCODE  6        ; 6 - Invalid opcode exception
ISR_NOERRCODE  7        ; 7 - No coprocessor exception
ISR_NOERRCODE  8        ; 8 - Double fault (pushes an error code)
ISR_NOERRCODE  9        ; 9 - Coprocessor segment overrun
ISR_NOERRCODE  10       ; 10 - Bad TSS (pushes an error code)
ISR_NOERRCODE  11       ; 11 - Segment not present (pushes an error code)
ISR_NOERRCODE  12       ; 12 - Stack fault (pushes an error code)
ISR_NOERRCODE  13       ; 13 - General protection fault (pushes an error code)
ISR_NOERRCODE  14       ; 14 - Page fault (pushes an error code)
ISR_NOERRCODE  15       ; 15 - Unknown interrupt exception
ISR_NOERRCODE  16       ; 16 - Coprocessor fault
ISR_NOERRCODE  17       ; 17 - Alignment check exception
ISR_NOERRCODE  18       ; 18 - Machine check exception
ISR_NOERRCODE  19       ; 19 - XM SIMD float exception

ISR_NOERRCODE  20
ISR_NOERRCODE  21
ISR_NOERRCODE  22
ISR_NOERRCODE  23
ISR_NOERRCODE  24
ISR_NOERRCODE  25
ISR_NOERRCODE  26
ISR_NOERRCODE  27
ISR_NOERRCODE  28
ISR_NOERRCODE  29
ISR_NOERRCODE  30
ISR_NOERRCODE  31

ISR_NOERRCODE  255

[GLOBAL isr_common_stub]
[EXTERN isr_handler]
; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov ax, ds               ; Lower 16-bits of eax = ds.
    push eax                 ; save the data segment descriptor

    mov ax, 0x10  ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call isr_handler
    add esp, 4

    pop eax        ; reload the original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                     ; Pops edi,esi,ebp...
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
.end:

%macro IRQ 2
  global irq%1
  irq%1:
    cli
    push byte 0
    push byte %2
    jmp irq_common_stub
%endmacro

IRQ   0,   32
IRQ   1,   33
IRQ   2,   34
IRQ   3,   35
IRQ   4,   36
IRQ   5,   37
IRQ   6,   38
IRQ   7,   39
IRQ   8,   40
IRQ   9,   41
IRQ  10,   42
IRQ  11,   43
IRQ  12,   44
IRQ  13,   45
IRQ  14,   46
IRQ  15,   47

[GLOBAL irq_common_stub]
[EXTERN irq_handler]
; This is our common IRQ stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
irq_common_stub:
    pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov ax, ds               ; Lower 16-bits of eax = ds.
    push eax                 ; save the data segment descriptor

    mov ax, 0x10  ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_handler
    add esp, 4

    pop ebx        ; reload the original data segment descriptor
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa                     ; Pops edi,esi,ebp...
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
.end:


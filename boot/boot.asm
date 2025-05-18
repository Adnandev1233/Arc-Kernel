; Multiboot header constants
MBALIGN     equ 1 << 0            ; align loaded modules on page boundaries
MEMINFO     equ 1 << 1            ; provide memory map
VIDINFO     equ 1 << 2            ; provide video info
FLAGS       equ MBALIGN | MEMINFO | VIDINFO  ; this is the Multiboot 'flag' field
MAGIC       equ 0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)  ; checksum of above to prove we are multiboot

; Multiboot header - MUST be first!
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    dd 0, 0, 0, 0, 0             ; unused fields for header addr, load addr, etc
    dd 0                         ; graphics mode
    dd 800, 600, 32             ; width, height, depth

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:

section .text
global _start:function (_start.end - _start)
extern kernel_main

_start:
    ; Set up the stack
    mov esp, stack_top

    ; Clear interrupts
    cli

    ; Clear EFLAGS
    push 0
    popf

    ; Initialize segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Push multiboot info
    push ebx    ; Multiboot structure pointer
    push eax    ; Multiboot magic number

    ; Jump to kernel
    call kernel_main

    ; If kernel returns, halt the CPU
    cli
.hang:
    hlt
    jmp .hang
.end:

section .note.GNU-stack noalloc noexec nowrite progbits 
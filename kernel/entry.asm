[BITS 32]
[GLOBAL _start]
[EXTERN kernel_main]

; VGA text mode constants
VGA_WIDTH equ 80
VGA_HEIGHT equ 25
VGA_MEMORY equ 0xB8000
VGA_COLOR_LIGHT_GREY equ 0x07
VGA_COLOR_RED equ 0x04

; Multiboot header constants
MBOOT_PAGE_ALIGN    equ 1<<0
MBOOT_MEM_INFO      equ 1<<1
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

section .multiboot
align 4
    dd MBOOT_HEADER_MAGIC
    dd MBOOT_HEADER_FLAGS
    dd MBOOT_CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KB stack
stack_top:

section .text
_start:
    ; Set up stack
    mov esp, stack_top

    ; Push multiboot info pointer
    push ebx

    ; Call kernel main
    call kernel_main

    ; If kernel_main returns, halt
.hang:
    cli
    hlt
    jmp .hang

; Early debug output function
; Input: ESI = string pointer
early_print:
    push eax
    push ebx
    mov ebx, VGA_MEMORY
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, VGA_COLOR_LIGHT_GREY
    mov [ebx], ax
    add ebx, 2
    jmp .loop
.done:
    pop ebx
    pop eax
    ret

; Error handler
error:
    mov esi, error_msg
    call early_print
.hang:
    cli
    hlt
    jmp .hang

; Data
boot_msg db 'ArcOS Kernel Starting...', 13, 10, 0
init_msg db 'Initializing kernel...', 13, 10, 0
halt_msg db 'Kernel halted.', 13, 10, 0
error_msg db 'CRITICAL ERROR: System halted.', 13, 10, 0 
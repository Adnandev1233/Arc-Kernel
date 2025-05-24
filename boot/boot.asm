[BITS 32]

; Multiboot header constants
MULTIBOOT_PAGE_ALIGN    equ 1<<0
MULTIBOOT_MEM_INFO      equ 1<<1
MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEM_INFO
MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

; Kernel stack size
KERNEL_STACK_SIZE equ 16384

section .multiboot
align 4
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM

section .bss
align 16
stack_bottom:
    resb KERNEL_STACK_SIZE
stack_top:

section .text
global _start
extern kernel_main
extern kernel_init

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Save multiboot information
    push ebx    ; Multiboot info structure
    push eax    ; Multiboot magic number
    
    ; Initialize kernel
    call kernel_init
    
    ; Call kernel main
    call kernel_main
    
    ; If kernel_main returns, halt
.hang:
    cli
    hlt
    jmp .hang 
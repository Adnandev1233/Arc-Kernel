[BITS 16]                       ; Start in 16-bit real mode
[ORG 0x7C00]                    ; BIOS loads bootloader at 0x7C00

; Constants
KERNEL_OFFSET equ 0x1000        ; Where we'll load the kernel
KERNEL_SECTORS equ 32           ; How many sectors to read (16KB)

start:
    ; Set up segment registers
    xor ax, ax                  ; Zero AX
    mov ds, ax                  ; Data segment = 0
    mov es, ax                  ; Extra segment = 0
    mov ss, ax                  ; Stack segment = 0
    mov sp, 0x7C00             ; Stack grows down from bootloader

    ; Save boot drive number
    mov [boot_drive], dl

    ; Print welcome message
    mov si, msg_welcome
    call print_string

    ; Load kernel from disk
    call load_kernel

    ; Switch to protected mode
    call switch_to_pm

    jmp $                       ; Should never get here

; Function: print_string
; Input: SI points to null-terminated string
print_string:
    pusha
    mov ah, 0x0E               ; BIOS teletype output
.loop:
    lodsb                      ; Load byte from SI into AL
    test al, al                ; Check if character is null
    jz .done                   ; If null, we're done
    int 0x10                   ; Print character
    jmp .loop
.done:
    popa
    ret

; Function: load_kernel
load_kernel:
    pusha
    mov si, msg_load_kernel
    call print_string

    mov ah, 0x02              ; BIOS read sector function
    mov al, KERNEL_SECTORS    ; Number of sectors to read
    mov ch, 0                 ; Cylinder 0
    mov cl, 2                 ; Start from sector 2
    mov dh, 0                 ; Head 0
    mov dl, [boot_drive]      ; Drive number
    mov bx, KERNEL_OFFSET     ; Buffer to load kernel into
    int 0x13                  ; BIOS disk interrupt

    jc disk_error             ; If carry flag set, there was an error
    
    popa
    ret

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

; Function: switch_to_pm
switch_to_pm:
    cli                       ; Disable interrupts
    lgdt [gdt_descriptor]     ; Load GDT

    ; Enable protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Far jump to 32-bit code
    jmp CODE_SEG:init_pm

[BITS 32]
init_pm:
    ; Set up segment registers for protected mode
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Jump to kernel
    call KERNEL_OFFSET
    jmp $

; Global Descriptor Table
gdt_start:
    ; Null descriptor
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xFFFF                 ; Limit (bits 0-15)
    dw 0x0                    ; Base (bits 0-15)
    db 0x0                    ; Base (bits 16-23)
    db 10011010b             ; Flags
    db 11001111b             ; Flags + Limit (bits 16-19)
    db 0x0                    ; Base (bits 24-31)

gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1    ; GDT size
    dd gdt_start                   ; GDT address

; Constants for GDT segments
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Data
boot_drive: db 0
msg_welcome: db 'ArcOS Bootloader starting...', 13, 10, 0
msg_load_kernel: db 'Loading kernel...', 13, 10, 0
msg_disk_error: db 'Error loading kernel!', 13, 10, 0

; Boot sector padding
times 510-($-$$) db 0         ; Fill with zeros up to 510 bytes
dw 0xAA55                     ; Boot signature 
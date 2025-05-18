[BITS 16]
[ORG 0x10000]

; Constants
KERNEL_SEGMENT equ 0x2000
KERNEL_OFFSET  equ 0x0000
KERNEL_LOAD_ADDR equ 0x100000  ; Load kernel at 1MB
KERNEL_SECTORS equ 64
STACK_TOP equ 0x90000

; Multiboot constants
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_PAGE_ALIGN equ 1<<0
MULTIBOOT_MEMORY_INFO equ 1<<1
MULTIBOOT_FLAGS equ (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_FLAGS)

; GDT entries
align 8
gdt_start:
    ; Null descriptor
    dq 0

    ; Code segment descriptor
    dw 0xFFFF    ; Limit (bits 0-15)
    dw 0x0000    ; Base (bits 0-15)
    db 0x00      ; Base (bits 16-23)
    db 10011010b ; Access byte
    db 11001111b ; Flags + Limit (bits 16-19)
    db 0x00      ; Base (bits 24-31)

    ; Data segment descriptor
    dw 0xFFFF    ; Limit (bits 0-15)
    dw 0x0000    ; Base (bits 0-15)
    db 0x00      ; Base (bits 16-23)
    db 10010010b ; Access byte
    db 11001111b ; Flags + Limit (bits 16-19)
    db 0x00      ; Base (bits 24-31)
gdt_end:

; GDT descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size
    dd gdt_start                 ; GDT address

; Segment selectors
CODE_SEG equ 0x08
DATA_SEG equ 0x10

start:
    ; Set up segments
    cli
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0xFFF0
    sti

    ; Print welcome message
    mov si, msg_stage2
    call print_string

    ; Get memory map
    call detect_memory

    ; Load kernel from disk
    call load_kernel

    ; Enable A20 line
    call enable_a20

    ; Enter protected mode
    cli                     ; Disable interrupts
    lgdt [gdt_descriptor]   ; Load GDT
    mov eax, cr0
    or eax, 1              ; Set protected mode bit
    mov cr0, eax
    
    ; Jump to 32-bit code
    jmp CODE_SEG:protected_mode

; Function to detect memory using INT 15h, AX=E820h
detect_memory:
    mov si, msg_detect_mem
    call print_string

    mov di, memory_map     ; ES:DI points to buffer
    xor ebx, ebx          ; Clear EBX
    mov edx, 0x534D4150   ; 'SMAP'
    mov eax, 0xE820
    mov [es:di + 20], dword 1  ; Force a valid ACPI entry
    mov ecx, 24           ; 24 bytes for each entry

.next_entry:
    mov eax, 0xE820
    mov [es:di + 20], dword 1  ; Force a valid ACPI entry
    mov ecx, 24           ; 24 bytes for each entry
    int 0x15
    jc .done              ; Carry set means end of list

    cmp eax, 0x534D4150   ; EAX should contain 'SMAP'
    jne .error

    add di, 24            ; Point to next entry
    cmp ebx, 0            ; ebx = 0 means last entry
    jne .next_entry

    ; Success message
    mov si, msg_mem_ok
    call print_string
    ret

.error:
    mov si, msg_mem_fail
    call print_string
.done:
    ret

; Function to load kernel from disk
load_kernel:
    mov si, msg_load_kernel
    call print_string

    ; Reset disk system
    xor ax, ax
    int 0x13
    jc disk_error

    ; Load kernel
    mov ax, KERNEL_SEGMENT
    mov es, ax
    xor bx, bx          ; ES:BX = KERNEL_SEGMENT:0

    mov ah, 0x02        ; BIOS read sectors
    mov al, KERNEL_SECTORS
    mov ch, 0           ; Cylinder 0
    mov cl, 10          ; Start from sector 10
    mov dh, 0           ; Head 0
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    ; Verify sectors read
    cmp al, KERNEL_SECTORS
    jne disk_error

    mov si, msg_kernel_ok
    call print_string
    ret

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

; Function to enable A20 line
enable_a20:
    mov si, msg_a20
    call print_string

    ; Try BIOS method first
    mov ax, 0x2401
    int 0x15
    jnc .done

    ; Try keyboard controller method
    cli
    call .wait_input
    mov al, 0xAD
    out 0x64, al
    call .wait_input
    mov al, 0xD0
    out 0x64, al
    call .wait_output
    in al, 0x60
    push ax
    call .wait_input
    mov al, 0xD1
    out 0x64, al
    call .wait_input
    pop ax
    or al, 2
    out 0x60, al
    call .wait_input
    mov al, 0xAE
    out 0x64, al
    call .wait_input
    sti

.done:
    mov si, msg_a20_ok
    call print_string
    ret

.wait_input:
    in al, 0x64
    test al, 2
    jnz .wait_input
    ret

.wait_output:
    in al, 0x64
    test al, 1
    jz .wait_output
    ret

; Function to print string (SI = string pointer)
print_string:
    push ax
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    pop ax
    ret

[BITS 32]
protected_mode:
    ; Set up segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up stack
    mov esp, STACK_TOP

    ; Copy kernel to its final location
    mov esi, KERNEL_SEGMENT * 16  ; Source
    mov edi, KERNEL_LOAD_ADDR     ; Destination
    mov ecx, KERNEL_SECTORS * 512 ; Size in bytes
    rep movsb

    ; Set up multiboot info structure
    mov eax, MULTIBOOT_HEADER_MAGIC  ; Magic number in eax
    mov ebx, memory_map              ; Pointer to memory map in ebx

    ; Jump to kernel (note: removed pushes as we pass in eax/ebx per spec)
    jmp CODE_SEG:KERNEL_LOAD_ADDR

; Data section
align 4
msg_stage2 db 'Stage 2 bootloader started', 13, 10, 0
msg_detect_mem db 'Detecting memory...', 13, 10, 0
msg_mem_ok db 'Memory detection successful', 13, 10, 0
msg_mem_fail db 'Memory detection failed', 13, 10, 0
msg_load_kernel db 'Loading kernel...', 13, 10, 0
msg_kernel_ok db 'Kernel loaded successfully', 13, 10, 0
msg_disk_error db 'Disk error!', 13, 10, 0
msg_a20 db 'Enabling A20 line...', 13, 10, 0
msg_a20_ok db 'A20 line enabled', 13, 10, 0
boot_drive db 0

; Memory map buffer
align 4
memory_map:
    times 24 * 32 db 0  ; Space for 32 memory map entries 
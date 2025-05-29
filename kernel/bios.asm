[BITS 32]

; Call BIOS interrupt
global bios_call
bios_call:
    ; Save registers
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi

    ; Get parameters
    movzx eax, byte [ebp+8]    ; interrupt number
    movzx ebx, word [ebp+12]   ; ax
    movzx ecx, word [ebp+16]   ; bx
    movzx edx, word [ebp+20]   ; cx
    movzx esi, word [ebp+24]   ; dx
    mov edi, [ebp+28]          ; out_ax
    mov eax, [ebp+32]          ; out_bx
    mov ebx, [ebp+36]          ; out_cx
    mov ecx, [ebp+40]          ; out_dx

    ; Save segment registers
    push ds
    push es
    push fs
    push gs

    ; Set up segment registers for real mode
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call BIOS interrupt
    push esi             ; dx
    push ecx             ; cx
    push ebx             ; ax
    mov ax, 0x13         ; BIOS interrupt
    int 0x13
    pop ebx
    pop ecx
    pop esi

    ; Save return values
    mov [edi], ax
    mov [eax], bx
    mov [ebx], cx
    mov [ecx], dx

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore registers
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret

; Call BIOS interrupt with buffer
global bios_call_with_buffer
bios_call_with_buffer:
    ; Save registers
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi

    ; Get parameters
    movzx eax, byte [ebp+8]    ; interrupt number
    movzx ebx, word [ebp+12]   ; ax
    movzx ecx, word [ebp+16]   ; bx
    movzx edx, word [ebp+20]   ; cx
    movzx esi, word [ebp+24]   ; dx
    mov edi, [ebp+28]          ; buffer
    mov eax, [ebp+32]          ; out_ax

    ; Save segment registers
    push ds
    push es
    push fs
    push gs

    ; Set up segment registers for real mode
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call BIOS interrupt
    push esi             ; dx
    push ecx             ; cx
    push ebx             ; ax
    mov ax, 0x13         ; BIOS interrupt
    int 0x13
    pop ebx
    pop ecx
    pop esi

    ; Save return value
    mov [eax], ax

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore registers
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret

; GDT for 32-bit mode
gdt32:
    dq 0x0000000000000000  ; Null descriptor
    dq 0x00CF9A000000FFFF  ; Code descriptor
    dq 0x00CF92000000FFFF  ; Data descriptor
gdt32_ptr:
    dw $ - gdt32 - 1
    dd gdt32

; GDT for 64-bit mode
gdt64:
    dq 0x0000000000000000  ; Null descriptor
    dq 0x00209A0000000000  ; Code descriptor
    dq 0x0000920000000000  ; Data descriptor
gdt64_ptr:
    dw $ - gdt64 - 1
    dd gdt64 
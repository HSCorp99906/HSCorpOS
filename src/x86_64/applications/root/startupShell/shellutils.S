global shell_prompt
global curs_mov
global ds_kb_irq

shell_prompt:
    mov ecx, [esp + 4]
    mov ebx, [esp + 8]
    mov eax, 0x3
    int 0x79        ; Syscall.
    ret

curs_mov:
    mov ecx, [esp + 4]
    mov ebx, [esp + 8]
    mov eax, 0x4
    int 0x79        ; Syscall.
    ret

ds_kb_irq:
    mov eax, 0x2
    int 0x79        ; Syscall.
    ret

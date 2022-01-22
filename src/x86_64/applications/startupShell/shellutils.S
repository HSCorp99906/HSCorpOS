global shell_prompt

shell_prompt:
    mov ecx, [esp + 4]
    mov ebx, [esp + 8]
    mov eax, 3
    int 0x79        ; Syscall.
    ret

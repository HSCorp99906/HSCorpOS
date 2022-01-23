;; Copyright (C) 2022 Ian Marco Moffett.
;; Redistribution and use in source and binary forms, with or without
;; modification, are permitted provided that the following conditions
;; are met:
;; 1. Redistributions of source code must retain the above copyright
;; notice, this list of conditions and the following disclaimer.
;; 2. Redistributions in binary form must reproduce the above copyright
;; notice, this list of conditions and the following disclaimer in the
;; documentation and/or other materials provided with the distribution.

;; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;; ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
;; LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;; POSSIBILITY OF SUCH DAMAGE.



bits 16
org 0x7C00

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax

mov bp, 4096
mov sp, bp

_start:
    mov [DRIVE_NUM], dl

    mov si, start_msg
    call puts

    mov si, boot_choice
    call puts

    mov si, reboot_choice
    call puts

    call read_keys

    mov ah, 0x0
    mov al, 0x2
    int 0x10
 
    mov si, boot_msg
    call puts

    ; Delay.
    mov cx, 0xF
    mov dx, 0x4240
    mov ah, 0x86
    int 0x15

    ; Delay.
    mov cx, 0xF
    mov dx, 0x4240
    mov ah, 0x86
    int 0x15

    mov dl, [DRIVE_NUM]
    call disk_load

    cli
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:pm_entry

    cli
    hlt

read_keys:
    mov ah, 0x00
    int 0x16

    cmp al, 'r'
    je reboot
    
    cmp al, 'b'
    je rm_ret

    jmp read_keys

reboot: jmp 0xFFFF:0x0
DRIVE_NUM: db 0

puts:
    mov ah, 0x0E  ; Teletype.
    lodsb         ; Load byte from SI into AL and increment SI.
    or al, al     ; If AL is zero, zero flag will be set.
    jz rm_ret     ; Return if zero.
    int 0x10
    jmp puts

rm_ret: ret

disk_load:
    mov ah, 0x42
    mov si, dap
    int 0x13
    jc disk_error
    ret    

disk_error:
    mov si, disk_error_msg
    call puts
    cli
    hlt

start_msg: db "Welcome to HSCorpOS, select an option.", 0x0D, 0x0A, 0x0D, 0x0A, 0
boot_choice: db "[B]oot", 0x0D, 0x0A, 0
reboot_choice: db "[R]eboot", 0x0D, 0x0A, 0
boot_msg: db "Booting HSCorpOS..", 0x0D, 0x0A, 0
disk_error_msg: db "FATAL: Failed to read from disk.", 0x0D, 0x0A, 0

dap:
    db 0x10        ; Size of DAP structure.
    db 0x0         ; Unused.
    dw 0xC         ; Number of sectors to read.
    dw 0x1000      ; Dest offset.
    dw 0           ; Dest segment.
    dq 0x1         ; Start sector (LBA) (0 based).


; GDT: It defines what 
; memory can be executed (The Code Descriptor), 
; and what area contains data (Data Descriptor). 

; This is used because everything about memory
; has changed in protected mode.


gdt_start:
    gdt_null:
        dd 0x0
        dd 0x0
    gdt_code:
        ; Type flags:
        ; Present: 1 since we are using code.
        ; Privilege: 00 higest privilige.
        ; Descriptor type: 1 for code/data.
        ; Code: 1.
        ; Conforming: 0 so segments with a lower privilege may not call code in this segment.
        ; Readable: 1.
        ; Accessed: 0.

        ; Other flags:
        ; Granularity: 1 so we can reach father into memory.
        ; 32-bit default: 1 since our segment will have 32-bit code.
        ; 64-bit code segment: 0.
        ; AVL 0.
        ; Limit: 1111.

        dw 0xFFFF       ; Limit.
        dw 0x0          ; Base.
        db 0x0          ; Base.
        db 10011010b    ; 1st flags, type flags.
        db 11001111b    ; 2nd flags, type flags.
        db 0x0
    gdt_data:
        ; Type flags: 
        ; Code: 0.
        ; Expand down: 0.
        ; Writable: 0.
        ; Accessed: 0.

        dw 0xFFFF       ; Limit.
        dw 0x0          ; Base.
        db 0x0          ; Base.
        db 10010010b    ; 1st flags, type flags.
        db 11001111b    ; 2nd flags, type flags.
        db 0x0
gdt_end:


gdt_desc:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

bits 32
pm_entry:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x1000
    mov esp, ebp
    jmp _PM

_PM:
    call 0x1000
    hlt

times 510-($-$$) db 0
dw 0xAA55

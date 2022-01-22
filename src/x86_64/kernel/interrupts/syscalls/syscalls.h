/*
* Copyright (C) 2022 Ian Marco Moffett.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef SYSCALLS_H
#define SYSCALLS_H

#define MAX_SYSCALLS 4

#include "../../drivers/IO.h"
#include "../../drivers/VGA.h"
#include "../../drivers/keyboard.h"
#include "../IDT.h"
#include "../../util/types.h"

uint8_t shell_mode = 1;

// De-stubify keyboard IRQ's.
void syscall_ds_kb_irq() {
    set_idt_desc32(0x21, kb_isr, INT_GATE_FLAGS);  
}

// Stubify keyboard IRQ's.
void syscall_sb_kb_irq() {
    set_idt_desc32(0x21, kb_stub_isr, INT_GATE_FLAGS);
}

void syscall_write_str() {
    if (shell_mode) {
        register const char* const STR asm("ecx");
        register const uint8_t NEWLINE_BOOL asm("ebx");
        vga_puts(STR, &main_vga, NEWLINE_BOOL);
    }
}


void syscall_restart() {
    __asm__ __volatile__("int $0xC8");
}


void* syscalls[MAX_SYSCALLS] = {
    syscall_restart,
    syscall_sb_kb_irq,
    syscall_ds_kb_irq,
    syscall_write_str,
    
};

#endif

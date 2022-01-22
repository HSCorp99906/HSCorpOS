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

char* main_vga = (char*)0xB8000;
unsigned int cursor_x = 10;
unsigned int cursor_y = 2;
unsigned char command_mode = 0;

#include "drivers/VGA.h"
#include "interrupts/IDT.h"
#include "interrupts/interrupt_handlers.h"
#include "interrupts/syscalls/syscalls.h"
#include "util/startup.h"
#include "drivers/keyboard.h"

int main() {
    init_idt();

    // Setup exceptions:
    set_idt_desc32(0, div_by_0_handler, TRAP_GATE_FLAGS); 

    // Setup interrupts.

    unmask_kb_irq();
    set_idt_desc32(0xC8, _reboot, INT_GATE_FLAGS);
    set_idt_desc32(0x21, kb_stub_isr, INT_GATE_FLAGS);

    __asm__ __volatile__("sti");

    // Setup syscalls.
    extern void _syscall_dispatcher();
    set_idt_desc32(0x79, _syscall_dispatcher, INT_GATE_USER_FLAGS); 


    // Change background and foreground then greet. 
    vga_clear(&main_vga, 0x01, 0x06 + 8);
    const char* const GREET = "Kernel Loaded at 0x1000.";
    
    vga_puts(GREET, &main_vga, 1);
    vga_puts("\0", &main_vga, 1);

    _start_root_prog();

    return 0;
}

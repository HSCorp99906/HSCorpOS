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


#ifndef IDT_H
#define IDT_H

#define IDT_SIZE 256

#define TRAP_GATE_FLAGS 0x8F
#define INT_GATE_FLAGS 0x8E
#define INT_GATE_USER_FLAGS 0xEE

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT 0x21

#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT 0xA1


#include "../drivers/VGA.h"
#include "../drivers/IO.h"
#include "../util/types.h"

/* 
 * Each entry describes where ISR
 * is located in memory to service
 * the interrupt.
 */

typedef struct {
    uint16_t isr_addr_low;
    uint16_t kernel_cs;         // Code segment where the ISR is.
    uint8_t reserved;           // Reserved by intel for some reason.
    uint8_t attr;
    uint16_t isr_addr_high;
} __attribute__((packed)) idt_entry32_t;


/*
 *  IDTR describes where in memory
 *  the IDT is.
 */

typedef struct {
    uint16_t limit;         // Max address of IDT.
    uint32_t base;          // Lowest address of IDT.
} __attribute__((packed)) idtr32_t;

idt_entry32_t idt32[IDT_SIZE];     // Actual IDT.
idtr32_t idtr32;

typedef struct {
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t sp;
    uint32_t ss;
} __attribute__((packed)) int_frame32_t;


/*
 * Interrupt attribute will
 * return with IRET, not RET.
 */


__attribute__((interrupt)) void default_int_handler(int_frame32_t* frame) {
    __asm__ __volatile__("cli; hlt");
}


void set_idt_desc32(uint8_t entry, void* isr, uint8_t flags) {
    idt_entry32_t* descriptor = &idt32[entry];
    descriptor->isr_addr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08;
    descriptor->reserved = 0;
    descriptor->attr = flags;
    descriptor->isr_addr_high = ((uint32_t)isr >> 16) & 0xFFFF;
}


void init_idt() {
    idtr32.limit = (uint16_t)(sizeof(idt_entry32_t) * 256);
    idtr32.base = (uint32_t)&idt32; 

    unsigned char a1,a2;
    a1 = inportb(0x21);             // Get master mask.
    a2 = inportb(0xA1);             // Get slave mask.

    outportb(0x20,0x10 | 0x01);     // Tell the PIC we want to initialize it
    outportb(0xA0,0x10 | 0x01);
    outportb(0x21, 0x20);         // Master PIC offset.
    outportb(0xA1, 0x28);         // Slave PIC offset.

    outportb(0x21,0x04);            // Tell master PIC there is a slave PIC at IRQ2.
    outportb(0xA1,0x02);            // Tell slave PIC its cascade identity.

    outportb(0x21,0x01);            // Put the master PIC into 8086 mode.
    outportb(0xA1,0x01);            // Put the slave PIC into 8086 mode.

    outportb(0x21,a1);              // Restore the masks.
    outportb(0xA1,a2);

    __asm__ __volatile__("lidt %0" : : "memory"(idtr32)); 

}  
#endif

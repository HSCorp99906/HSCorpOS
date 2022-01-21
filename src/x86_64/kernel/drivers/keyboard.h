#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "IO.h"
#include "VGA.h"
#include "../interrupts/IDT.h"
#include "../util/types.h"

const uint8_t* const SC_ASCII = "\x00\x1B" "1234567890-=" "\x08"
"\x00" "qwertyuiop[]" "\x0D\x1D" "asdfghjkl;'`" "\x00" "\\"
"zxcvbnm,./" "\x00\x00\x00" " ";


void unmask_kb_irq() {
    outportb(PIC1_DATA_PORT, 0xFD);
}

uint8_t writeCh = 1;
char* kb_drvr_vga = (char*)0xB8000;

__attribute__((interrupt)) void kb_isr(int_frame32_t* frame) {  
    uint8_t scancode = inportb(0x60);
    char ch = SC_ASCII[scancode];

    if (ch >= 'a' && ch <= 'z') {
        char str[2] = "\0\0";
        ch -= 0x20;
        str[0] = ch;
        vga_puts(str, &main_vga);
        main_vga -= 2;
    }
 
    outportb(0x20, 0x20); 
}

#endif

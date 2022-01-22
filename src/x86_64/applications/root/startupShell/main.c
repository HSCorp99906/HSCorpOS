extern void shell_prompt(const char* str, unsigned char newline);
extern void curs_mov(int x, int y);
extern void ds_kb_irq();

int _su_shell_main() {
    // Prompt initalization.
    const char* const PROMPT = "KessOS~$ ";
    shell_prompt(PROMPT, 0);
    curs_mov(10, 2);
    ds_kb_irq();
    return 0;
}

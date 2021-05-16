#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdarg.h>
#include <stdint.h>

#define COM1 0x3f8

void debug_init(void);
void debug_putc(char);
void debug_puts(char *);
void debug_puti(uint64_t, int);
void debug_printf(char *, ...);

#endif

#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdarg.h>

#define COM1 0x3f8

void debug_putc(char);
void debug_puts(char *);
void dbeug_puti(int);
void debug_printf(char *, ...);

#endif

#include <debug.h>
#include <port.h>

const static char *DIGITS = "0123456789abcdef";

void debug_putc(char c) {
    while ((inb(COM1 + 5) & 0x20) == 0);
    outb(COM1, c);
}

void debug_puts(char* s) {
    for (; *s; s++) {
        debug_putc(*s);
    }
}

int pow(int n, int b) {
    int out = 1;
    for (; b; b--) {
        out *= n;
    }
    return out;
}

void debug_puti(int n, int base) {
    if (n < 0) {
        debug_putc('-');
        n = -n;
    }
    if (n == 0) {
        debug_putc('0');
        return;
    }

    int this, i = 0;
    while (pow(base, i) <= n) {
        i++;
    }
    i--;
    while (i >= 0) {
        this = n / pow(base, i);
        debug_putc(DIGITS[this]);
        n -= this * pow(base, i);
        i--;
    }
}

void debug_printf(char *msg, ...) {
    va_list args;
    va_start(args, msg);

    for(; *msg; msg++) {
        if (*msg == '%') {
            msg++;
            switch (*msg) {
                case '%':
                    {
                        debug_putc('%');
                        break;
                    }
               case 'd':
                    {
                        int n = va_arg(args, int);
                        debug_puti(n, 10);
                        break;
                    }
               case 'x':
                    {
                        int n = va_arg(args, int);
                        debug_puti(n, 16);
                        break;
                    }
               case 's':
                    {
                        char *s = va_arg(args, char *);
                        debug_puts(s);
                        break;
                    }
            }
        } else {
            debug_putc(*msg);
        }
    }

    va_end(args);
}

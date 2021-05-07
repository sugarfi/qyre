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

uint64_t pow(uint64_t b, int n) {
    uint64_t out = 1;
    for (; n; n--) {
        out *= b;
    }
    return out;
}

void debug_puti(uint64_t n, int base) {
    char buf[64];
    if (n == 0) {
        debug_putc('0');
        return;
    }

    int count = 0;
    uint64_t n2 = n;
    while (n2) {
        count++;
        n2 /= base;
    }
    buf[count] = 0;

    int i = count - 1;
    while (n) {
        buf[i--] = DIGITS[n % base];
        n /= base;
    }

    debug_puts(buf);
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
                        uint64_t n = va_arg(args, uint64_t);
                        debug_puti(n, 10);
                        break;
                    }
               case 'x':
                    {
                        uint64_t n = va_arg(args, uint64_t);
                        debug_puti(n, 16);
                        break;
                    }
               case 'b':
                    {
                        uint64_t n = va_arg(args, uint64_t);
                        debug_puti(n, 2);
                        break;
                    }
               case 's':
                    {
                        char *s = va_arg(args, char *);
                        debug_puts(s);
                        break;
                    }
               case 'c':
                    {
                        char c = (char) (va_arg(args, int) & 0xff);
                        debug_putc(c);
                        break;
                    }
            }
        } else {
            debug_putc(*msg);
        }
    }

    va_end(args);
}

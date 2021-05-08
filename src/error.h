#ifndef _ERROR_H
#define _ERROR_H

#include <debug.h>

#define error(...) debug_printf("error: " __VA_ARGS__);

typedef enum {
    OK,
    ERROR_NOT_FOUND,
    ERROR_NOT_IMPLEMENTED,
    ERROR_INVALID,
    ERROR_HARDWARE
} error_t;

#endif

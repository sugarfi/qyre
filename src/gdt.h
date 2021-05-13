#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
} gdt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t size;
    uint32_t offset;
} gdt_descriptor_t;

#endif

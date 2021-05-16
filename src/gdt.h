#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint16_t limit_lo;
    uint16_t base_lo;
    uint8_t base_mid;
    uint8_t access;
    uint8_t limit_hi:4;
    uint8_t flags:4;
    uint8_t base_hi;
} gdt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t size;
    uint64_t offset;
} gdt_descriptor_t;

void gdt_load(gdt_descriptor_t);
void gdt_set_gate(gdt_entry_t *, int, uint32_t, uint32_t, uint8_t, uint8_t);

#endif

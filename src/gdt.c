#include <gdt.h>

void gdt_load(gdt_descriptor_t desc) {
    __asm__ volatile ("lgdt (%0)" : : "r" (&desc));
}    

void gdt_set_gate(gdt_entry_t *gdt, int n, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt[n].base_lo = (base & 0xffff);
    gdt[n].base_mid = (base >> 16) & 0xff;
    gdt[n].base_hi = (base >> 24) & 0xff;

    gdt[n].limit_lo = (limit & 0xffff);
    gdt[n].limit_hi = (limit >> 16) & 0xf;

    gdt[n].flags = flags & 0xf;
    gdt[n].access = access;
}

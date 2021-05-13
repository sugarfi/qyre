#include <page.h>

void page_load(uint32_t *pd) {
    __asm__ volatile ("movq %0, %%cr3" : : "a" ((uint64_t) pd));
}

void page_flush_tlb(void) {
    __asm__ volatile ("movq %%cr3, %%rax; movq %%rax, %%cr3;" : :);
}

uint64_t *page_get_pml4(void) {
    uint64_t *pml4;
    __asm__ volatile ("mov %%cr3, %0" : "=a" (pml4) :);
    return pml4;
}

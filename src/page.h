#ifndef _PAGE_H
#define _PAGE_H

#include <stdint.h>

void page_load(uint32_t *);
void page_flush_tlb(void);
uint64_t *page_get_pml4(void);

#endif

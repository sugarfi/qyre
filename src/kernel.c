#include <string.h>
#include <node.h>
#include <debug.h>
#include <stddef.h>
#include <bootboot.h>
#include <graphics.h>
#include <alloc.h>
#include <page.h>

const int MAX_REGION = 32;
const int MAX_BACKUP = 4;
const int NODES_PER_REGION = 32;

const uintptr_t HEADER_ADDR = 0x1000000;
const uintptr_t REGION_LIST_ADDR = HEADER_ADDR + 512;
const uintptr_t BACKUP_LIST_ADDR = REGION_LIST_ADDR + (sizeof(node_region_entry_t) * MAX_REGION);
const uintptr_t NODE_LIST_ADDR = BACKUP_LIST_ADDR + (sizeof(node_backup_entry_t) * MAX_BACKUP);
const uintptr_t DATA_ADDR = NODE_LIST_ADDR + (sizeof(node_t) * NODES_PER_REGION * MAX_REGION);

extern BOOTBOOT bootboot;
extern unsigned char environment[4096];
extern uint8_t fb;

void kmain(void) {
    int i;

    node_database_header_t header = {
        .type = 1,
        .max_regions = MAX_REGION,
        .max_backups = MAX_BACKUP,
        .next_data_pos = 0
    };
    memcpy("BEGAYDOCRIME", header.signature, 12);
    memcpy("memory__000000000000000000000000", header.ref, 32);

    memset((char *) HEADER_ADDR, 0, 512);
    memcpy((char *) &header, (char *) HEADER_ADDR, sizeof(node_database_header_t));

    node_region_entry_t *region_set;
    for (region_set = (node_region_entry_t *) REGION_LIST_ADDR, i = 0; i < MAX_REGION; region_set++, i++) {
        region_set->start = i * 32;
        region_set->end = i * 32 + 32;

        int j;
        node_t *node_set;
        for (node_set = (node_t *) NODE_LIST_ADDR + i, j = 0; j < NODES_PER_REGION; node_set++, j++) {
            node_set->id = 0;
            node_set->region = 0;
            node_set->backup = 0;
            node_set->type = 0;
            node_set->size = 0;
            node_set->data_pos = 0;
        }
    };

    node_backup_entry_t *backup_set;
    for (backup_set = (node_backup_entry_t *) BACKUP_LIST_ADDR, i = 0; i < MAX_BACKUP; backup_set++, i++) {
        backup_set->type = 0;
        memset(backup_set->ref, 0, 32);
        memset((char *) backup_set->other, 0, 512);
    }

    node_database_t db = {
        .header = (node_database_header_t *) HEADER_ADDR,
        .regions = (node_region_entry_t *) REGION_LIST_ADDR,
        .backups = (node_backup_entry_t *) BACKUP_LIST_ADDR,
        .nodes = (node_t *) NODE_LIST_ADDR,
        .data = (uint8_t *) DATA_ADDR
    };

    alloc_db = &db;

    graphics_context_t ctx = {
        .width = bootboot.fb_width,
        .height = bootboot.fb_height,
        .fb = (uint32_t *) &fb,
        .color_type = bootboot.fb_type
    };

    node_backup_entry_t backup = {
        .type = 2
    };
    node_backup_disk_t backup_other = {
        .drive = 0, // this is ignored anyway,
        .sector = 32896
    };
    memcpy("disk____000000000000000000000000", backup.ref, 32);
    memset((char *) backup.other, 0, 512);
    memcpy((char *) &backup_other, (char *) backup.other, sizeof(node_backup_disk_t));
    memcpy((char *) &backup, (char *) db.backups, sizeof(node_backup_entry_t));

    uint64_t addr = 0x300000000;
    debug_printf("%x\r\n", addr);
    int pml4_i = (addr & 0b11000000000000000000000000000000) >> 30;
    int pdt_i =  (addr & 0b00111111111000000000000000000000) >> 21;
    int pd_i =   (addr & 0b00000000000111111111000000000000) >> 12;
    int pt_i =   (addr & 0b00000000000000000000111111111111) >> 0;
    debug_printf("%x %x %x %x\r\n", pml4_i, pdt_i, pd_i, pt_i);
    for(;;);

    uint64_t *pml4 = page_get_pml4();
    uint64_t *pdt = (uint64_t *) (pml4[pml4_i] & ~0xfff);
    uint64_t *pd = (uint64_t *) (pdt[pdt_i] & ~0xfff);
    uint64_t *pt = (uint64_t *) (pd[pd_i] & ~0xfff);
    
    pt[pt_i] = (0x200000000 << 12) | 0x03;

    __asm__ volatile ("mov %0, %%cr3" : : "r" ((uint64_t) pml4));

    uint32_t *x = (uint32_t *) 0x300000000;
    x[0] = 0xdeadbeef;

    node_ref_t ref = {
        .id = 1,
        .region = 0,
        .backup = 1
    };
    node_t node;
    node_lookup(ref, db, &node);

    for(;;);
}

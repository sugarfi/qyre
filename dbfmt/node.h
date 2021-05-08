#ifndef _NODE_H
#define _NODE_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    char signature[12]; // BEGAYDOCRIME
    char ref[32];
    uint64_t type;
    uint64_t max_regions;
    uint64_t max_backups;
    uint64_t next_data_pos;
} node_database_header_t;

typedef enum {
    NODE_TYPE_ALLOC,
    NODE_TYPE_TEXT,
    NODE_TYPE_BINARY
} node_type_t;

typedef struct __attribute__((packed)) {
    uint64_t id;
    uint64_t region;
    uint64_t backup;
    uint64_t type;
    uint64_t size;
    uint64_t data_pos;
} node_t;

typedef struct __attribute__((packed)) {
    uint64_t start;
    uint64_t end;
} node_region_entry_t;

typedef struct __attribute__((packed)) {
    uint64_t type;
    char ref[32];
    uint8_t other[512];
} node_backup_entry_t;

typedef struct __attribute__((packed)) {
    uint64_t addr;
} node_backup_memory_t;

typedef struct __attribute__((packed)) {
    uint64_t id;
    uint64_t region;
    uint64_t backup;
} node_backup_node_t;

typedef struct __attribute__((packed)) {
    uint8_t drive;
    uint64_t sector;
} node_backup_disk_t;

typedef struct __attribute__((packed)) {
    char addr[256];
    uint16_t port;
} node_backup_net_t;

typedef struct {
    uint64_t id;
    uint64_t region;
    uint64_t backup;
} node_ref_t;

typedef struct __attribute__((packed)) {
    node_database_header_t *header;
    node_region_entry_t *regions;
    node_backup_entry_t *backups;
    node_t *nodes;
    uint8_t *data;
} node_database_t;

#endif

#ifndef _ALLOC_H
#define _ALLOC_H

#include <node.h>
#include <string.h>
#include <debug.h>

typedef struct {
    void *ptr;
    int size;
    node_ref_t ref;
} alloc_t;

node_database_t *alloc_db;

error_t alloc(int, alloc_t *);
error_t free(alloc_t);

#endif

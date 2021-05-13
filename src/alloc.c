#include <alloc.h>
#include <debug.h>

error_t alloc(int size, alloc_t *out) {
    uint8_t buf[size];
    memset((char *) buf, 0, size);
    node_ref_t ref; 
    error_t res;
    if ((res = node_add(buf, size, NODE_TYPE_ALLOC, alloc_db, &ref)) != OK) {
        return res;
    };
    out->size = size;
    out->ref = ref;
    debug_printf("%d#%d@%d\r\n", ref.id, ref.region, ref.backup);
    //test(ref, *alloc_db, &out->ptr);
    node_get_data(ref, *alloc_db, &out->ptr);
    for(;;);
    return OK;
}

error_t free(alloc_t alloc) {
    error_t res;
    if ((res = node_delete(alloc.ref, alloc_db)) != OK) {
        return res;
    }
    return OK;
}

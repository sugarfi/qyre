#include <string.h>
#include <node.h>
#include <debug.h>
#include <rand.h>
#include <alloc.h>

error_t node_lookup_region(uint64_t region, node_database_t db, node_region_entry_t *out) {
    if (region > db.header->max_regions) {
        error("region index %d is too large", region);
        return ERROR_INVALID;
    }

    *out = *(db.regions + region);
    return OK;
}

error_t node_lookup_backup(uint64_t backup, node_database_t db, node_backup_entry_t *out) {
    if (backup > db.header->max_backups) {
        error("backup index %d is too large", backup);
        return ERROR_INVALID;
    }

    *out = *(db.backups + backup);
    return OK;
}

int node_get_max_node(node_region_entry_t *regions) {
    int end = 0;
    for (; regions->end != 0; ++regions) {
        end = regions->end;
    }
    return end;
}

uint64_t node_generate_id(void) {
    // TODO: better, more portable solution
    uint64_t out = 0;
    int i;
    for (i = 0; i < 64; i++) {
        out |= (rand64() & (2 << i));
    }
    return out;
}

error_t node_lookup_database_ref(node_backup_entry_t ref, node_database_t *out) {
    switch (ref.type) {
        case 1: // in memory database
            {
                node_backup_memory_t *other = (node_backup_memory_t *) ref.other;
                node_database_header_t *header = (node_database_header_t *) other->addr;
                if (!strncmp(header->signature, "BEGAYDOCRIME", 12)) {
                    error("invalid database signature");
                    return ERROR_INVALID;
                }
                if (!strncmp(ref.ref, header->ref, 32)) {
                    error("database refs don't match and no way to search for more");
                    return ERROR_NOT_FOUND;
                }

                node_region_entry_t *regions = (node_region_entry_t *) (other->addr + 512);
                node_backup_entry_t *backups = (node_backup_entry_t *) (regions + header->max_regions);
                node_t *nodes = (node_t *) (backups + header->max_backups);
                uint8_t *data = (uint8_t *) (nodes + node_get_max_node(regions));

                out->header = header;
                out->regions = regions;
                out->backups = backups;
                out->nodes = nodes;
                out->data = data;
                return OK;
            }
        case 2: // disk database
            {
                node_backup_disk_t *other = (node_backup_disk_t *) ref.other;
                alloc_t header_alloc;
                error_t res;
                if ((res = alloc(512, &header_alloc)) != OK) {
                    return res;
                }
                node_database_header_t *header = (node_database_header_t *) header_alloc.ptr;
                if (ata_read(other->sector, 1, (uint8_t *) header) != OK) {
                    return ERROR_HARDWARE;
                }
                if (!strncmp(header->signature, "BEGAYDOCRIME", 12)) {
                    error("invalid database signature");
                    return ERROR_INVALID;
                }
                if (!strncmp(ref.ref, header->ref, 32)) {
                    error("database refs don't match and no way to search for more");
                    return ERROR_NOT_FOUND;
                }

                node_region_entry_t *regions;
                int regions_size = sizeof(node_region_entry_t) * header->max_regions;
                while (regions_size++ % 512);
                ata_read(other->sector + 1, regions_size / 512, (uint8_t *) regions);
                node_backup_entry_t *backups;
                int backups_size = sizeof(node_backup_entry_t) * header->max_backups;
                while (backups_size++ % 512);
                ata_read(other->sector + 1 + regions_size / 512, backups_size / 512, (uint8_t *) backups);

                memcpy((char *) header, (char *) out->header, sizeof(node_database_header_t));
                debug_printf("%x %x %x\r\n", (uint64_t) header, (uint64_t) regions, (uint64_t) backups);
                out->regions = regions;
                out->backups = backups;
                return OK;
            }
        case 3:
        case 4:
            {
                error("unimplemented database type %d", ref.type);
                return ERROR_NOT_IMPLEMENTED;
            }
        default:
            {
                error("invalid database type %d", ref.type);
                return ERROR_INVALID;
            }
    }
}

error_t node_lookup_by_index(int index, node_database_t db, node_t *out) {
    switch (db.header->type) {
        case 1: // in memory database
            {
                *out = db.nodes[index];
                return OK;
            }
        case 2:
        case 3:
        case 4:
            {
                error("unimplemented database type %d", db.header->type);
                return ERROR_NOT_IMPLEMENTED;
            }
        default:
            {
                error("invalid database type %d", db.header->type);
                return ERROR_INVALID;
            }
    }
}

error_t node_set_by_index(int index, node_t node, node_database_t *db) {
    switch (db->header->type) {
        case 1: // in memory database
            {
                db->nodes[index] = node;
                return OK;
            }
        case 2:
        case 3:
        case 4:
            {
                error("unimplemented database type %d", db->header->type);
                return ERROR_NOT_IMPLEMENTED;
            }
        default:
            {
                error("invalid database type %d", db->header->type);
                return ERROR_INVALID;
            }
    }
}

error_t node_lookup(node_ref_t ref, node_database_t db, node_t *out) {
    if (ref.id == 0) {
        error("invalid node id");
        return ERROR_INVALID;
    }

    node_database_t real_db = db;
    if (ref.backup != 0) {
        node_backup_entry_t real_db_ref;
        if (node_lookup_backup(ref.backup - 1, db, &real_db_ref) != OK) {
            return ERROR_NOT_FOUND;
        }
        if (node_lookup_database_ref(real_db_ref, &real_db) != OK) {
            return ERROR_NOT_FOUND;
        }
    }

    node_region_entry_t region;
    if (node_lookup_region(ref.region, real_db, &region) != OK) {
        return ERROR_NOT_FOUND;
    }

    switch (real_db.header->type) {
        case 1: // in memory database
            {
                int i;
                for (i = region.start; i < region.end; i++) {
                    node_t node;
                    if (node_lookup_by_index(i, real_db, &node) != OK) {
                        return ERROR_NOT_FOUND;
                    }
                    if (node.id == ref.id) {
                        *out = node;
                        return OK;
                    }
                }

                if (ref.backup == 0) {
                    node_ref_t ref2 = ref;
                    node_backup_entry_t db2_ref;
                    node_database_t db2;

                    for (i = 0; i < real_db.header->max_backups; i++) {
                        if (db.backups[i].type != 0) {
                            if (node_lookup_backup(i, db, &db2_ref) != OK) {
                                return ERROR_NOT_FOUND;
                            }
                            if (node_lookup_database_ref(db2_ref, &db2) != OK) {
                                return ERROR_NOT_FOUND;
                            }

                            ref2.backup = i;
                            if (node_lookup(ref2, db2, out) == OK) {
                                return OK;
                            }
                        }
                    }
                }

                error("could not find node %d#%d@%d", ref.id, ref.region, ref.backup);
                return ERROR_NOT_FOUND;
            }
        case 2:
        case 3:
        case 4:
            {
                error("unimplemented database type %d", real_db.header->type);
                return ERROR_NOT_IMPLEMENTED;
            }
        default: 
            {
                error("invalid database type %d", real_db.header->type);
                return ERROR_INVALID;
            }
    }
}

error_t node_set(node_ref_t ref, node_t new, node_database_t *db) {
    if (ref.id == 0) {
        error("invalid node id");
        return ERROR_INVALID;
    }

    node_database_t *real_db = db;
    if (ref.backup != 0) {
        node_backup_entry_t real_db_ref;
        if (node_lookup_backup(ref.backup - 1, *db, &real_db_ref) != OK) {
            return ERROR_NOT_FOUND;
        }
        if (node_lookup_database_ref(real_db_ref, real_db) != OK) {
            return ERROR_NOT_FOUND;
        }
    }

    node_region_entry_t region;
    if (node_lookup_region(ref.region, *real_db, &region) != OK) {
        return ERROR_NOT_FOUND;
    }

    switch (real_db->header->type) {
        case 1: // in memory database
            {
                int i;
                for (i = region.start; i < region.end; i++) {
                    node_t node;
                    if (node_lookup_by_index(i, *real_db, &node) != OK) {
                        return ERROR_NOT_FOUND;
                    }
                    if (node.id == ref.id) {
                        node_set_by_index(i, new, real_db);
                        return OK;
                    }
                }
                error("could not find node %d#%d@%d", ref.id, ref.region, ref.backup);
                return ERROR_NOT_FOUND;
            }
        case 2:
        case 3:
        case 4:
            {
                error("unimplemented database type %d", real_db->header->type);
                return ERROR_NOT_IMPLEMENTED;
            }
        default: 
            {
                error("invalid database type %d", real_db->header->type);
                return ERROR_INVALID;
            }
    }
}

error_t node_add(uint8_t *data, uint64_t size, node_type_t type, node_database_t *db, node_ref_t *out) {
    node_region_entry_t *region;
    int i;
    for (region = db->regions, i = 0; region->end != 0; region++, i++) {
        int j;
        for (j = region->start; j < region->end; j++) {
            node_t node;
            if (node_lookup_by_index(j, *db, &node) != OK) {
                return ERROR_NOT_FOUND;
            }
            if (node.id == 0) {
                out->id = node.id = node_generate_id();
                out->region = i;
                out->backup = 0;

                if (node.size < size) {
                    node.data_pos = db->header->next_data_pos;
                    db->header->next_data_pos += size;
                }
                node.size = size;
                node.type = (uint64_t) type;
                memcpy((char *) data, (char *) (db->data + node.data_pos), size);

                error_t res;
                if ((res = node_set_by_index(j, node, db)) != OK) {
                    return res;
                };

                return OK;
            }
        }
    }

    error("could not find free node");
    return ERROR_NOT_FOUND;
}

error_t node_delete(node_ref_t ref, node_database_t *db) {
    node_database_t *real_db = db;
    if (ref.backup != 0) {
        node_backup_entry_t real_db_ref;
        if (node_lookup_backup(ref.backup - 1, *db, &real_db_ref) != OK) {
            return ERROR_NOT_FOUND;
        }
        if (node_lookup_database_ref(real_db_ref, real_db) != OK) {
            return ERROR_NOT_FOUND;
        }
    }

    node_t node;
    if (node_lookup(ref, *real_db, &node) != OK) {
        return ERROR_NOT_FOUND;
    }

    node.id = 0;
    memset((char *) real_db->data + node.data_pos, 0, node.size);

    error_t res;
    if ((res = node_set(ref, node, real_db)) != OK) {
        return res;
    }

    return OK;
}

error_t node_get_data(node_ref_t ref, node_database_t db, uint8_t **out) {
    node_database_t real_db = db;
    if (ref.backup != 0) {
        node_backup_entry_t real_db_ref;
        if (node_lookup_backup(ref.backup - 1, db, &real_db_ref) != OK) {
            return ERROR_NOT_FOUND;
        }
        if (node_lookup_database_ref(real_db_ref, &real_db) != OK) {
            return ERROR_NOT_FOUND;
        }
    }

    node_t node;
    error_t res;
    if ((res = node_lookup(ref, real_db, &node)) != OK) {
        return res;
    }

    *out = real_db.data + node.data_pos;
    return OK;
}


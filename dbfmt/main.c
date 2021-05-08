#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"

const int MAX_REGIONS = 32;
const int MAX_BACKUPS = 32;
const int NODES_PER_REGION = 32;
const int MAX_NODES = NODES_PER_REGION * MAX_REGIONS;

const int HEADER_POS = 0;
const int REGIONS_POS = 512;
const int BACKUPS_POS = REGIONS_POS + sizeof(node_region_entry_t) * MAX_REGIONS;
const int NODES_POS = BACKUPS_POS + sizeof(node_backup_entry_t) * MAX_BACKUPS;
const int DATA_POS = NODES_POS + sizeof(node_t) * MAX_NODES;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fputs("usage: dbfmt <database image> [files*]", stderr);
        return 1;
    }
    FILE *db = fopen(argv[1], "w");

    node_database_header_t header = {
        .type = 2,
        .max_regions = MAX_REGIONS,
        .max_backups = MAX_BACKUPS,
        .next_data_pos = 0
    };
    strncpy(header.signature, "BEGAYDOCRIME", 12);
    strncpy(header.ref, "disk____000000000000000000000000", 32);
    
    int i, id = 1;
    
    node_region_entry_t region;
    for (i = 0; i < MAX_REGIONS; i++) {
        region.start = i * 32;
        region.end = i * 32 + 32;
        fseek(db, REGIONS_POS + (i * sizeof(node_region_entry_t)), SEEK_SET);
        fwrite((char *) &region, sizeof(node_region_entry_t), 1, db);
    }

    node_backup_entry_t backup;
    for (i = 0; i < MAX_BACKUPS; i++) {
        backup.type = 0;
        memset(backup.ref, 0, 32);
        memset(backup.other, 0, 512);
        fseek(db, BACKUPS_POS + (i * sizeof(node_backup_entry_t)), SEEK_SET);
        fwrite((char *) &backup, sizeof(node_backup_entry_t), 1, db);
    }

    FILE* file;
    node_t node;
    for (i = 0; i < (argc - 2); i++) {
        fprintf(stderr, "write %s to database\n", argv[i + 2]);
        file = fopen(argv[i + 2], "r");
        
        fseek(file, 0, SEEK_END);
        int size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        char *data = malloc(size);
        fread(data, size, 1, file);

        node.id = id++;
        node.region = i / 32;
        node.backup = 0;
        node.type = 1;
        node.size = size;
        node.data_pos = header.next_data_pos;

        fseek(db, NODES_POS + (i * sizeof(node_t)), SEEK_SET);
        fwrite(db, sizeof(node_t), 1, db);

        fprintf(stderr, "%s is %ld#%ld@%ld\n", argv[i + 2], node.id, node.region, node.backup);

        fseek(db, DATA_POS + header.next_data_pos, SEEK_SET);
        fwrite(data, size, 1, db);

        header.next_data_pos += size;

        free(data);
        fclose(file);
    }

    for (; i < MAX_NODES; i++) {
        node.id = 0;
        node.region = i % 32;
        node.backup = 0;
        node.type = 0;
        node.size = 0;
        node.data_pos = 0;

        fseek(db, NODES_POS + (i * sizeof(node_t)), SEEK_SET);
        fwrite(db, sizeof(node_t), 1, db);
    }

    fseek(db, HEADER_POS, SEEK_SET);
    fwrite((char *) &header, sizeof(node_database_header_t), 1, db);

    fclose(db);
    return 0;
}

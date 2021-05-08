#ifndef _ATA_H
#define _ATA_H

#include <debug.h>
#include <error.h>
#include <port.h>

#define ATA_BASE 0x1f0
#define ATA_REG_DATA (ATA_BASE + 0)
#define ATA_REG_ERROR (ATA_BASE + 1)
#define ATA_REG_FEATURES (ATA_BASE + 1)
#define ATA_REG_COUNT (ATA_BASE + 2)
#define ATA_REG_LBALO (ATA_BASE + 3)
#define ATA_REG_LBAMID (ATA_BASE + 4)
#define ATA_REG_LBAHI (ATA_BASE + 5)
#define ATA_REG_DRIVE (ATA_BASE + 6)
#define ATA_REG_STATUS (ATA_BASE + 7)
#define ATA_REG_COMMAND (ATA_BASE + 7)

error_t ata_read(uint64_t, uint8_t, uint8_t *);
error_t ata_write(uint8_t *, uint64_t, uint8_t);

#endif

#include <ata.h>

error_t ata_poll(void) {
    uint8_t status;
    do {
        status = inb(ATA_REG_STATUS);
        if (!(status  & 0x80) && (status & 0x08)) break;
        if ((status & 0x01) || (status & 0x20)) {
            error("ata error flag was set");
            return ERROR_HARDWARE;
        }
    } while (1);

    return OK;
}

error_t ata_read(uint64_t sector, uint8_t count, uint8_t *out) {
    outb(ATA_REG_DRIVE, 0xe0 | (sector >> 24));
    outb(ATA_REG_COUNT, (uint8_t) count);
    outb(ATA_REG_LBALO, sector & 0xff);
    outb(ATA_REG_LBAMID, sector >> 8);
    outb(ATA_REG_LBAHI, sector >> 16);
    outb(ATA_REG_COMMAND, 0x20);  

    uint16_t *buf = (uint16_t *) out;

    int i;
    for (i = 0; i < count; i++) {
        if (ata_poll() != OK) {
            return ERROR_HARDWARE;
        }
        int n = 256;
        __asm__ volatile("rep insw" : "+D" (buf), "+c" (n), "=m" (*buf) : "d" (ATA_BASE) : "memory");
        buf += 256;
    }

    return OK;
}

error_t ata_write(uint8_t *data, uint64_t sector, uint8_t count) {
    outb(ATA_REG_DRIVE, 0xe0 | (sector >> 24));
    outb(ATA_REG_COUNT, (uint8_t) count);
    outb(ATA_REG_LBALO, sector & 0xff);
    outb(ATA_REG_LBAMID, sector >> 8);
    outb(ATA_REG_LBAHI, sector >> 16);
    outb(ATA_REG_COMMAND, 0x30);  

    uint16_t *buf = (uint16_t *) data;

    int i;
    for (i = 0; i < count; i++) {
        if (ata_poll() != OK) {
            return ERROR_HARDWARE;
        }
        int n = 256;
        __asm__ volatile("rep outsw" : "+S" (buf), "+c" (n), "=m" (*buf) : "d" (ATA_BASE) : "memory");
        buf += 256;
    }

    outb(ATA_REG_COMMAND, 0xe7);

    return OK;
}

#ifndef _PORT_H
#define _PORT_H

#include <stdint.h>

uint8_t inb(uint16_t);
void outb(uint16_t, uint8_t);
uint16_t inw(uint16_t);
void outw(uint16_t, uint16_t);

#endif

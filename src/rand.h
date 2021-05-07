#ifndef _RAND_H
#define _RAND_H

#include <stdint.h>

uint32_t rand(void);
uint64_t rand64(void);
void rand_seed(uint64_t);

#endif

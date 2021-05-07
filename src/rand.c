#include <rand.h>

static uint64_t next = 44;

uint32_t rand(void) {
    next = next * 1103515245 + 12345;
    return (next / 65536) % 32768;
}

uint64_t rand64(void) {
    // TODO: this is BAD!
    return (((uint64_t) rand()) << 32) | rand();
}

void rand_seed(uint64_t seed) {
    next = seed;
}

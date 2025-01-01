
#include <gmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
extern gmp_randstate_t state;

void randstate_init(uint64_t seed) {
    srandom(seed);  // Initialize the C library's random number generator
    gmp_randinit_mt(state);        // Initialize the GMP random state to use the
                                   // Mersenne Twister algorithm.
    gmp_randseed_ui(state, seed);  // Set the initial seed for the GMP random
                                   // state using an unsigned integer.
}
void randstate_clear(void) { gmp_randclear(state); }

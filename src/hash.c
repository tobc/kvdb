#include "hash.h"

#include <string.h>

// Largest prime number that fits into 32 bits.
// We could switch to using the Mersenne prime 2^31 - 1 and perform modular
// arithmetic using bit shifts to improve performance.
const uint64_t PRIME32 = (1ULL << 32) - 5;

// Polynomial hashing of strings of arbitrary length. This hash function relies
// on the fundamental theorem of algebra to guarantee a low probability of
// collision. Strings x, y of length at most k have a collision probability of
// at most k/PRIME32. It does not guarantee that the hash codes follow a
// high-quality pseudorandom distribution.
uint32_t hash_string(const char* str) {
    const uint64_t x = 1376620161;
    uint64_t res = 0;
    for (size_t i = 0; str[i] != '\0'; i++) {
        res = x * (res + (uint64_t)str[i]) % PRIME32;
    }
    return res;
}

// 16-independent hash function in GF(PRIME32).
uint32_t hash_uint32(uint32_t x) {
    const size_t DEGREE = 16;
    const uint64_t random[DEGREE] = {
        218470127,  1347007625, 1649783683, 2338716363, 2017170303, 2519877935,
        504091718,  402577613,  2566796408, 3836992736, 3963337496, 2321505666,
        2243901607, 1836924774, 238276928,  4108384667};

    uint64_t res = 0;
    for (size_t i = 0; i < DEGREE; i++) {
        res = (x * res + random[i]) % PRIME32;
    }
    return res;
}

// We first hash the string down to 32 bits, and then we apply a high-quality
// polynomial hash function to the fingerprint.
uint32_t hash(const char* str) {
    return hash_uint32(hash_string(str));
}
#ifndef KVDB_HASH_H
#define KVDB_HASH_H

#include <stdint.h>

// High-quality pseudorandom hashing of strings of arbitrary length.
// Hashes a string uniformly at random to an unsigned integer between 0 (inclusive) and 2^32 - 5 (exclusive).
uint32_t hash(const char* str);

#endif  // KVDB_HASH_H
#ifndef KVDB_CONFIG_H
#define KVDB_CONFIG_H

#include <stddef.h>
#include <stdint.h>

const uint32_t BUCKETS = 8; // Must be a power of two.
const uint32_t BUCKET_MASK = BUCKETS - 1;

const size_t MAX_KEY_LENGTH = 64;
const size_t MAX_VALUE_LENGTH = 64;

#endif  // KVDB_CONFIG_H
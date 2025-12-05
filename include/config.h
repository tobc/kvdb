#ifndef KVDB_CONFIG_H
#define KVDB_CONFIG_H

#include <stddef.h>
#include <stdint.h>

extern const uint32_t BUCKETS;
extern const uint32_t BUCKET_MASK;

#define MAX_KEY_LENGTH 63
#define MAX_VALUE_LENGTH 63
#define MAX_OP_LENGTH 3

#endif  // KVDB_CONFIG_H
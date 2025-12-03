#ifndef KVDB_CONFIG_H
#define KVDB_CONFIG_H

#include <stddef.h>
#include <stdint.h>

extern const uint32_t BUCKETS;
extern const uint32_t BUCKET_MASK;

extern const size_t MAX_KEY_LENGTH;
extern const size_t MAX_VALUE_LENGTH;

#endif  // KVDB_CONFIG_H
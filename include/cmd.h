#ifndef KVDB_CMD_H
#define KVDB_CMD_H

#include <stdint.h>
#include <stdbool.h>

#include "config.h"

// Epoch time in milliseconds: 1000*sec + ms.
struct Timestamp {
    uint64_t sec;
    uint64_t ms;
};

// A record recovered while scanning a file.
struct Record {
    int key_count; // Counts the number of times we have seen the key since the previous delete.
    struct Timestamp ts_first;
    struct Timestamp ts_last;
    char value[MAX_VALUE_LENGTH + 1];
};

// Exposed here for unit testing.
// Reads the log/bucket associated with key and writes the result to the record struct.
bool read_log(const char* key, struct Record* record); 

bool set(const char* key, const char* value);
bool get(const char* key);
bool del(const char* key);
bool ts(const char* key);

#endif  // KVDB_CMD_H
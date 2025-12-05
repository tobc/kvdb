#include "cmd.h"

#include <inttypes.h>  // PRIX32
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>    // strol
#include <string.h>    // strtok()
#include <sys/file.h>  // flock()
#include <time.h>      // timespec_get()
#include <unistd.h>    // sleep()

#include "config.h"
#include "hash.h"

bool write_log(const char* op, const char* key, const char* value) {
    // Open the file in data/{hash_code}.txt in append mode.
    // The file is created if it does not already exist.
    // Creating the file should be thread-safe.
    uint32_t h = hash(key);
    uint32_t bucket = h & BUCKET_MASK;
    char filename[32];
    sprintf(filename, "data/bucket_%u.csv", bucket);
    FILE* fp = fopen(filename, "a");
    if (fp == NULL) {
        perror("failed to open file");
        return true;
    }

    // Obtain file descriptor for calling flock().
    int fd = fileno(fp);
    if (fd == -1) {
        perror("failed to obtain file descriptor");
        fclose(fp);
        return true;
    }

    // Acquire exclusive lock on the file.
    if (flock(fd, LOCK_EX) == -1) {
        perror("failed to acquire exclusive lock");
        fclose(fp);
        return true;
    }

    // Write to file.
    if (strcmp(op, "DEL") == 0) {
        fprintf(fp, "%08" PRIX32 ",%s,%s,\n", h, key, op);
    }
    if (strcmp(op, "SET") == 0) {
        struct timespec ts;
        timespec_get(&ts, TIME_UTC);
        const uint64_t sec = (uint64_t)ts.tv_sec;
        const uint64_t ms = (uint64_t)ts.tv_nsec / 1000000;
        fprintf(fp, "%08" PRIX32 ",%s,%s,%s,%llu,%llu,\n", h, key, op, value, sec,
                ms);
    }

    // Release lock.
    if (flock(fd, LOCK_UN) == -1) {
        perror("failed to release lock");
        close(fd);
        return true;
    }
    fclose(fp);
    return false;
}

void timestamp_to_utc(struct Timestamp ts, char* buf) {
    time_t epoch = (time_t)ts.sec;
    struct tm* utc_time = gmtime(&epoch);
    char str[32];
    strftime(str, 32, "%Y-%m-%d %H:%M:%S", utc_time);
    sprintf(buf, "%s.%llu", str, ts.ms);
}

bool empty_token(const char* token, const char* line, bool* error) {
    if(token == NULL) {
        printf("missing value in line %s", line);
        *error = true;
        return true;
    }
    return false;
}

bool read_log(const char* key, struct Record* record) {

    // Open the file in data/{hash_code}.txt in read mode.
    uint32_t key_hash = hash(key);
    uint32_t bucket = key_hash & BUCKET_MASK;
    char filename[32];
    sprintf(filename, "data/bucket_%u.csv", bucket);
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        record->key_count = 0;
        return false;
    }

    // Obtain file descriptor for calling flock().
    int fd = fileno(fp);
    if (fd == -1) {
        perror("failed to obtain file descriptor");
        fclose(fp);
        return true;
    }

    // Acquire shared lock on the file.
    if (flock(fd, LOCK_SH) == -1) {
        perror("failed to acquire shared lock");
        fclose(fp);
        return true;
    }

    // Read the file line by line.
    const int MAX_LINE_LENGTH = MAX_KEY_LENGTH + MAX_KEY_LENGTH + 128;
    char line[MAX_LINE_LENGTH];
    bool error = false;
    record->key_count = 0;
    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        // printf("%s", line);
        // Hash.
        char* token = strtok(line, ",");
        if(empty_token(token, line, &error)) break;
        uint32_t record_hash = strtoul(token, NULL, 16);
        if (record_hash != key_hash) {
            // Skip the record if the hash values do not match.
            continue;
        }

        // Key.
        token = strtok(NULL, ",");
        if(empty_token(token, line, &error)) break;
        if (strcmp(key, token) != 0) {
            // Skip the record if the keys differ.
            continue;
        }

        // Operation.
        token = strtok(NULL, ",");
        if(empty_token(token, line, &error)) break;
        if (strcmp(token, "DEL") == 0) {
            record->key_count = 0;
            continue;
        }
        record->key_count++;
        
        // Value.
        token = strtok(NULL, ",");
        if(empty_token(token, line, &error)) break;
        strncpy(record->value, token, MAX_VALUE_LENGTH);
        
        // Timestamp.
        struct Timestamp ts;
        token = strtok(NULL, ",");
        if(empty_token(token, line, &error)) break;
        ts.sec = strtoull(token, NULL, 10);
        token = strtok(NULL, ",");
        if(empty_token(token, line, &error)) break;
        ts.ms = strtoull(token, NULL, 10);

        // Assume for simplicity that timestamps follow insertion order. This
        // should be true as we hold an exclusive lock when writing.
        if (record->key_count == 1) {
            record->ts_first = ts;
        }
        record->ts_last = ts;
    }

    // Release lock.
    if (flock(fd, LOCK_UN) == -1) {
        perror("failed to release lock");
        close(fd);
        return true;
    }
    fclose(fp);
    return error;
}

bool set(const char* key, const char* value) {
    return write_log("SET", key, value);
}

bool del(const char* key) { 
    return write_log("DEL", key, NULL); 
}

bool get(const char* key) { 
    struct Record record;
    if (read_log(key, &record)) return true; // errors reported.
    if(record.key_count == 0) {
        printf("record for key %s not found\n", key);
        return false;
    }
    printf("the key \'%s\' is associated with the value \'%s\'\n", key, record.value);
    return false;
}

bool ts(const char* key) { 
    struct Record record;
    if (read_log(key, &record)) return true; // errors reported.
    if(record.key_count == 0) {
        printf("record for key \'%s\' not found\n", key);
        return false;
    }
    char first_ts_str[64];
    char last_ts_str[64];
    timestamp_to_utc(record.ts_first, first_ts_str);
    timestamp_to_utc(record.ts_last, last_ts_str);
    printf("the key \'%s\' was first set %s UTC and last set %s UTC\n", key, first_ts_str, last_ts_str);
    return false;
}
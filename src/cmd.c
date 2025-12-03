#include <stdio.h>
#include <stdbool.h>
#include <sys/file.h> // flock()
#include <unistd.h> // sleep()
#include <time.h> // timespec_get()

#include "hash.h"
#include "config.h"

bool set(const char* key, const char* value) {
    // Open the file in data/{hash_code}.txt in append mode.
    // The file is created if it does not already exist.
    // Creating the file should be thread-safe.
    uint32_t bucket = hash(key) & BUCKET_MASK;
    char filename[32];
    sprintf(filename, "data/%x.csv", bucket);
    FILE* f = fopen(filename, "a");
    if(f == NULL) {
        perror("failed to open file");
        return true;
    }

    // Obtain file descriptor for calling flock().
    int fd = fileno(f);
    if(fd == -1) {
        perror("failed to obtain file descriptor");
        fclose(f);
        return true;
    }

    // Acquire exclusive lock on the file.
    if(flock(fd, LOCK_EX) == -1) {
        perror("failed to acquire exclusive lock");
        fclose(f);
        return true;
    }

    // Write to the file.
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    fprintf(f, "%s,%s,%llu,%llu\n", key, value, (uint64_t)ts.tv_sec, (uint64_t)ts.tv_nsec);

    // Release lock.
    if (flock(fd, LOCK_UN) == -1) {
        perror("failed to release lock");
        close(fd);
        return true;
    }
    fclose(f); 
    return false;
}

void get(const char* key) { printf("getting key: %s\n", key); }

void del(const char* key) { printf("deleting key: %s\n", key); }

void ts(const char* key) { printf("getting timestamps for key: %s\n", key); }
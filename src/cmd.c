#include <stdio.h>
#include <stdbool.h>

#include "hash.h"

bool set(const char* key, const char* value) {

    FILE* f = fopen("test.txt", "a");
    if(f == NULL) {
        puts("failed to open file");
        return true;
    }
    fprintf(f, "%s\t%s\n", key, value);
    fclose(f); 
    return false;
}

void get(const char* key) { printf("getting key: %s\n", key); }

void del(const char* key) { printf("deleting key: %s\n", key); }

void ts(const char* key) { printf("getting timestamps for key: %s\n", key); }
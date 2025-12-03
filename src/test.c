#include "test.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"

void test_hash();

void test(const char* key) { 
    if(strcmp(key, "hash") == 0) {
        test_hash();
    }
}

void test_hash() {
    puts("running hash() test");
    const char* strings[] = {"this",    "is",   "a",         "collection", "of",
                           "strings", "of",   "different", "lengths",    "and",
                           "content", "that", "we",        "use",        "to",
                           "test", "that",   "our",  "hash",      "function", "output", "looks", "random"};
    size_t length = sizeof(strings)/sizeof(char*);
    for(size_t i = 0; i < length; i++) {
        printf("%12s: %4x\n", strings[i], hash(strings[i]));
    }

    assert(hash("abc") == hash("abc") && "identical strings hash to the same value");
    assert(hash("abc") != hash("def") && "different strings hash to different values (most of the time)");
}

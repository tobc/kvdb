#include "test.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"
#include "cmd.h"

void test_hash();
void test_db();

void test(const char* key) { 
    if(strcmp(key, "hash") == 0) {
        test_hash();
    }

    if(strcmp(key, "db") == 0) {
        test_db();
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

void test_db() {
    // Insert keys and verify that the values we get back are accurate.
    char key[MAX_KEY_LENGTH + 1];
    char value[MAX_VALUE_LENGTH + 1];
    int num_keys = 1000;
    printf("inserting %d keys\n", num_keys);
    for(int i = 0; i < num_keys; i++) {
        sprintf(key, "k%d", i);
        sprintf(value, "v%d", num_keys - i);
        bool err = set(key, value);
        assert(!err && "error during insetion");
    }

    printf("getting %d keys\n", num_keys);
    for(int i = 0; i < num_keys; i++) {
        sprintf(key, "k%d", i);
        sprintf(value, "v%d", num_keys - i);
        struct Record record;
        bool err = read_log(key, &record);
        assert(!err && "error during read");
        assert(record.key_count == 1 && "key appeared more than once");
        assert(strcmp(record.value, value) == 0 && "retrieved value not equal to set value");
    }
}
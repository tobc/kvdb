#include "test.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cmd.h"
#include "hash.h"

void test_hash();
void test_setget();
void test_ts();
void test_del();

void test(const char* key) {
    if (strcmp(key, "hash") == 0) {
        test_hash();
    }

    if (strcmp(key, "setget") == 0) {
        test_setget();
    }

    if (strcmp(key, "del") == 0) {
        test_del();
    }

    if (strcmp(key, "ts") == 0) {
        test_ts();
    }
}

void test_hash() {
    puts("running hash() test");
    const char* strings[] = {
        "this", "is",        "a",       "collection", "of",      "strings",
        "of",   "different", "lengths", "and",        "content", "that",
        "we",   "use",       "to",      "test",       "that",    "our",
        "hash", "function",  "output",  "looks",      "random"};
    size_t length = sizeof(strings) / sizeof(char*);
    for (size_t i = 0; i < length; i++) {
        printf("%12s: %4x\n", strings[i], hash(strings[i]));
    }

    assert(hash("abc") == hash("abc") &&
           "identical strings hash to the same value");
    assert(hash("abc") != hash("def") &&
           "different strings hash to different values (most of the time)");
}

void test_setget() {
    // Insert keys and verify that the values we get back are accurate.
    char key[MAX_KEY_LENGTH + 1];
    char value[MAX_VALUE_LENGTH + 1];
    int num_keys = 1000;
    printf("inserting %d keys\n", num_keys);
    for (int i = 0; i < num_keys; i++) {
        sprintf(key, "k%d", i);
        sprintf(value, "v%d", num_keys - i);
        bool err = set(key, value);
        assert(!err && "error during insertion");
    }

    printf("getting %d keys\n", num_keys);
    for (int i = 0; i < num_keys; i++) {
        sprintf(key, "k%d", i);
        sprintf(value, "v%d", num_keys - i);
        struct Record record;
        bool err = read_log(key, &record);
        assert(!err && "error during read");
        assert(record.key_count == 1 && "key appeared more than once");
        assert(strcmp(record.value, value) == 0 &&
               "retrieved value not equal to set value");
    }

    // Set half of the keys to new values and ensure that we get the right
    // values back.
    printf("re-inserting %d keys with new values\n", num_keys / 2);
    for (int i = 0; i < num_keys / 2; i++) {
        sprintf(key, "k%d", i);
        sprintf(value, "v%d", 2 * num_keys - i);
        bool err = set(key, value);
        assert(!err && "error during insetion");
    }

    for (int i = 0; i < num_keys; i++) {
        sprintf(key, "k%d", i);
        int count = 1;
        if (i < num_keys / 2) {
            count = 2;
            sprintf(value, "v%d", 2 * num_keys - i);
        } else {
            sprintf(value, "v%d", num_keys - i);
        }
        struct Record record;
        bool err = read_log(key, &record);
        assert(!err && "error during read");
        assert(record.key_count == count &&
               "key appeared the wrong number of times");
        assert(strcmp(record.value, value) == 0 &&
               "retrieved value not equal to set value");
    }
}

void test_del() {
    char key[] = "abc";
    char value[] = "value";
    struct Record record;

    // Insert key and verify that we can retrieve it.
    set(key, value);
    read_log(key, &record);
    assert(record.key_count == 1 && "key should appear once");

    // Delete and verify that we do not retrieve it. 
    bool err = del(key);
    assert(!err && "error during deletion");
    read_log(key, &record);
    assert(record.key_count == 0 && "key count should be zero after deletion");

    // Re-insert and re-delete.
    set(key, value);
    read_log(key, &record);
    assert(record.key_count == 1 && "key should appear once");

    err = del(key);
    assert(!err && "error during deletion");
    read_log(key, &record);
    assert(record.key_count == 0 && "key count should be zero after deletion");
}

void test_ts() {
    char key[] = "k1";
    set(key, "v1");
    struct Record record;
    read_log(key, &record);
    assert(record.ts_first.sec == record.ts_last.sec &&
           record.ts_first.ms == record.ts_last.ms &&
           "first and last timestamps should be identical");

    sleep(2);  // Make sure the second insert happens at least a second after
               // the first.
    set(key, "v2");
    read_log(key, &record);
    assert(record.ts_first.sec < record.ts_last.sec &&
           "the timestamp from the second insert should come after the first");
}
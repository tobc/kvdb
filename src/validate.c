#include "validate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "config.h" // MAX_KEY_LENGTH, MAX_VALUE_LENGTH

void print_help() {
    const char* help =
        "kvdb usage:\n"
        "kvdb set {key} {value} : Associates `key` with `value`.\n"
        "kvdb get {key}         : Fetches the value associated with `key`.\n"
        "kvdb del {key}         : Removes `key` from the database.\n"
        "kvdb ts {key}          : Returns the timestamp when `key` was first "
        "set "
        "and when it was last set.";
    puts(help);
}

bool validate_input(int argc, char* argv[]) {
    // Enough input arguments.
    if (argc < 3 || (strcmp(argv[1], "set") == 0 && argc < 4)) {
        puts("too few arguments");
        return true;
    }

    // Valid command.
    const char* cmd = argv[1];
    if (strcmp(cmd, "set") != 0 && strcmp(cmd, "get") != 0 &&
        strcmp(cmd, "del") != 0 && strcmp(cmd, "ts") != 0 &&
        strcmp(cmd, "test") != 0) {
        puts("invalid command");
        return true;
    }

    // Key length.
    const char* key = argv[2];
    const size_t key_length = strlen(key);
    if (key_length > MAX_KEY_LENGTH) {
        printf("key length of %zu exceeds the max key length of %d\n",
               key_length, MAX_KEY_LENGTH);
        return true;
    }

    // Value length.
    if (strcmp(argv[1], "set") == 0) {
        const char* value = argv[3];
        const size_t value_length = strlen(value);
        if (value_length > MAX_VALUE_LENGTH) {
            printf("value length of %zu exceeds the max value length of %d\n",
                   value_length, MAX_VALUE_LENGTH);
            return true;
        }
    }

    // Alphanumeric input.
    // This depends on the current C locale so it is probably not very robust.
    for(int i = 1; i < argc; i++) {
        const char* s = argv[i];
        for(int j = 0; s[j] != '\0'; j++) {
            if(isalnum(s[j]) == 0) {
                printf("only alphanumeric characters supported: %s contained non-alphanumeric character %c\n", s, s[j]);
                return true;
            }
        }
    }
    return false;
}
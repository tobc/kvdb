#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "validate.h"
#include "cmd.h"

int main(int argc, char* argv[]) {
    if (validate_input(argc, argv)) {
        print_help();
        return EXIT_FAILURE;
    }

    const char* cmd = argv[1];
    const char* key = argv[2];

    if (strcmp(cmd, "set") == 0) {
        const char* value = argv[3];
        if(set(key, value)) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    if (strcmp(cmd, "get") == 0) {
        get(key);
        return EXIT_SUCCESS;
    }

    if (strcmp(cmd, "del") == 0) {
        del(key);
        return EXIT_SUCCESS;
    }

    if (strcmp(cmd, "ts") == 0) {
        ts(key);
        return EXIT_SUCCESS;
    }
    
    if (strcmp(cmd, "test") == 0) {
        test(key);
        return EXIT_SUCCESS;
    }

    // Any errors should have been caught by validate_input().
    puts("unexpected failure");
    print_help();
    return EXIT_FAILURE;
}





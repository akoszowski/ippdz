//
// Created by antoniusz on 14.05.2020.
//
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "gamma_parser.h"


int main() {
    char *buffer;
    size_t buf_size = 32;

    buffer = malloc(sizeof(char) * buf_size);
    if (buffer == NULL) {
        exit(1);
    }

    while (getline(&buffer, &buf_size, stdin) != -1) {
        parse_input(buffer);
    }

    // musimy jeszcze zwolnić grę !!!
    delete_game();
    free(buffer);

    return 0;
}
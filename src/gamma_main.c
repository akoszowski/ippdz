/**@file
 * Implementation  of gamma game main function.
 *
 * @author Antoni Koszowski <a.koszowski@students.mimuw.edu.pl>
 * @copyright
 * @date 17.05.2020
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "gamma_parser.h"

/**
 * Buffer storing parsed input.
 */
static char *buffer;

/**
 * Function deallocating buffer.
 */
static void free_buffer() {
    free(buffer);
}


/**@brief Main function of gamma game.
 * Main function of gamma game, realising simulation.
 * @return Value @p 0 if game was run successfully.
 */
int main() {
    size_t buf_size = 32;

    atexit(free_buffer);

    buffer = malloc(sizeof(char) * buf_size);
    if (buffer == NULL) {
        exit(1);
    }

    while (getline(&buffer, &buf_size, stdin) != -1) {
        parse_input(buffer);
    }

    delete_game();

    return 0;
}
/**@file
 * Implementation of gamma game input parser.
 *
 * @author Antoni Koszowski <a.koszowski@students.mimuw.edu.pl>
 * @copyright
 * @date 17.05.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gamma_parser.h"
#include "gamma_interactive.h"
#include "gamma.h"

/**
 * Maximal number of considered parameters.
 */
#define MAX_PARAMS 4

/**
 * Pointer to structure storing gamma game.
 */
static gamma_t *gamma_game= NULL;

/**
 * Number of currently parsed input line.
 */
static uint64_t cur_line = 0;

/**
 * Array storing whitespaces ASCII representation.
 */
static const char *delim = " \t\v\r\f\n";
/**
 * Array storing meaningful command signs.
 */
static const char *proper_signs = "IBmgbfqp";

/**@brief Gives status of game.
 * Gives information whether game is active or not.
 * @return Value @p true if game is active, @p false otherwise.
 */
static bool is_active() {
    if (gamma_game != NULL) {
        return true;
    } else {
        return false;
    }
}

/**@brief Checks for comment line.
 * Gives information whether parsed @p input_line is a comment.
 * @param input_line - checked input line.
 * @return Value @p true if input line is a comment, @p false otherwise.
 */
static bool comment_line(char *input_line) {
    if (input_line[0] == '#') {
        return true;
    }

    return false;
}

/**@brief Checks for empty line.
 * Gives information whether @p input_line is empty.
 * @param input_line - checked input line.
 * @return Value @p if input line is empty, @p false otherwise.
 */
static bool empty_line(char *input_line) {
    return input_line[0] == '\n';
}

/**@brief Checks for '\n' ending.
 * Gives information whether @p input_line has '\n' ending.
 * @param input_line  - checked input line.
 * @return Value @p true if input line has '\n' ending, @p false otherwise.
 */
static bool endl_ending(char *input_line) {
    size_t i = strlen(input_line) - 1;

    return input_line[i] == '\n';
}

/**@brief Returns first sign of input line.
 * Returns first sign of @p input_line,
 * and gives information about its propriety.
 * @param input_line - input line which first sign is returned.
 * @return First sign of input line - if it is proper, 0 otherwise.
 */
static char get_first_sign(char *input_line) {
    size_t  i;
    bool is_proper = false;

    for (i = 0; i < strlen(proper_signs) && !is_proper; ++i) {
        if (input_line[0] == proper_signs[i]) {
            is_proper = true;
        }
    }

    if (!is_proper || strlen(input_line) == 1) {
        return 0;
    }

    for (i = 0; i < strlen(delim); ++i) {
        if (input_line[1] == delim[i]) {
            return input_line[0];
        }
    }

    return 0;
}

/**@brief Checks whether given signs are printable.
 * Checks whether @p token contains printable sign (ASCII 33 - 255).
 * @param token - array of signs.
 * @return Value @p true if there is some sign out of range,
 *         @p false otherwise.
 */
static bool out_of_range(char *token) {
    size_t i;

    for (i = 0; i < strlen(token); ++i) {
        if ((unsigned char)token[i] < 32) {
            return true;
        }
    }

    return false;
}

/**@brief Converts correct char parameters into unsigned integer.
 * If given @p token contains transformable parameter
 * it is converted into unsigned integer pointed by @p param.
 * otherwise gives information that it is illegal.
 * @param[in] token - array containing char parameter,
 * @param[in] param - pointer where value of converted chars is stored.
 * @return Value @p true if char parameter was converted properly,
 *         @p false otherwise.
 */
static bool got_param(char *token, uint32_t *param) {
    if (out_of_range(token) || token[0] == '-') {
        return false;
    }

    char *ending;
    uint64_t cur_num = strtoul(token, &ending, 10);

    if (strlen(ending) == 0 && cur_num <= UINT32_MAX) {
        *param = (uint32_t)cur_num;
        return true;
    }

    return false;
}

/**@brief Checks if first parameter is equal zero.
 * Gives information whether first parameter from @p params is equal zero.
 * @param params - array containing parameters/
 * @return  Value @p true if first parameter is zero, @p false otherwise.
 */
static bool param_zero(uint32_t params[MAX_PARAMS]) {
    int i;

    for (i = 0; i < MAX_PARAMS; ++i) {
        if (params[i] == 0) {
            return true;
        }
    }

    return false;
}

/**@brief Activates choosen mode.
 * If given command is proper, basing on @p first_sign, @p params,
 * @p p_number and such action is possible appropriate mode is activated.
 * @param first_sign - first sign parameter,
 * @param params     - next parameters,
 * @param p_number   - number of parameters.
 */
static void choose_mode(char first_sign, uint32_t params[], uint32_t p_number) {
    if (p_number != 4) {
        fprintf(stderr, "ERROR %lu\n", cur_line);
        return;
    }
    else if (param_zero(params)) {
        fprintf(stderr, "ERROR %lu\n", cur_line);
        return;
    }

    if (first_sign == 'I') {
        gamma_game = gamma_new(params[0], params[1], params[2], params[3]);
        if (gamma_game == NULL) {
            fprintf(stderr, "ERROR %lu\n", cur_line);
        }
        else {
            launch_interactive(gamma_game, params[0], params[1], params[2]);
        }
    }
    else if (first_sign == 'B') {
        gamma_game = gamma_new(params[0], params[1], params[2], params[3]);
        if (gamma_game == NULL) {
            fprintf(stderr, "ERROR %lu\n", cur_line);
        }
        else {
            printf("OK %lu\n", cur_line);
        }
    }
}

/**@brief Chooses command option.
 * Checks whether given command is appropriate, basing on information from
 * @p first_sign, @p params, @p p_number, if so it is executed.
 * @param first_sign - first sign parameter,
 * @param params     - next parameters,
 * @param p_number   - number of parameters.
 */
static void choose_option(char first_sign, uint32_t params[], uint32_t p_number) {
    if (!is_active()) {
        choose_mode(first_sign, params, p_number);
    }
    else {
        if (first_sign == 'm' && p_number == 3) {
            printf("%d\n", gamma_move(gamma_game, params[0], params[1], params[2]));
        }
        else if (first_sign == 'g' && p_number == 3) {
            printf("%d\n", gamma_golden_move(gamma_game, params[0], params[1], params[2]));
        }
        else if (first_sign == 'b' && p_number == 1) {
            printf("%lu\n", gamma_busy_fields(gamma_game, params[0]));
        }
        else if (first_sign == 'f' && p_number == 1) {
            printf("%lu\n", gamma_free_fields(gamma_game, params[0]));
        }
        else  if (first_sign == 'q' && p_number == 1) {
            printf("%d\n", gamma_golden_possible(gamma_game, params[0]));
        }
        else if (first_sign == 'p' && p_number == 0) {
            char *board = gamma_board(gamma_game);
            if (board == NULL) {
                exit(1);
            }
            else {
                printf("%s", board);
                free(board);
            }
        }
        else {
            fprintf(stderr, "ERROR %lu\n", cur_line);
        }
    }
}

void parse_input(char *input_line) {
    uint32_t p_number = 0, params[MAX_PARAMS];
    char first_sign, *token;

    cur_line++;

    // Checking preconditions.
    if (comment_line(input_line) || empty_line(input_line)) {
        return;
    }

    first_sign = get_first_sign(input_line);

    if (!endl_ending(input_line)) {
        fprintf(stderr, "ERROR %lu\n", cur_line);
        return;
    }
    else if (first_sign == 0) {
        fprintf(stderr, "ERROR %lu\n", cur_line);
        return;
    }

    token = strtok(input_line, delim);

    // Checking parameters.
    while ((token = strtok(0, delim))) {
        if (p_number == 4) {
            fprintf(stderr, "ERROR %lu\n", cur_line);
            return;
        }

        if (!(got_param(token, &params[p_number++]))) {
            fprintf(stderr, "ERROR %lu\n", cur_line);
            return;
        }
    }

    choose_option(first_sign, params, p_number);
}

void delete_game() {
    gamma_delete(gamma_game);
}

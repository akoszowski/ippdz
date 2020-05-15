//
// Created by antoniusz on 14.05.2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gamma_parser.h"
#include "gamma.h"

#define MAX_PARAMS 4

static gamma_t *gamma_game= NULL;

static int count_lines = 0;

static const char *delim = " \t\v\r\f\n";
static const char *proper_signs = "IBmgbfqp";

static bool is_active() {
    if (gamma_game != NULL) {
        return true;
    } else {
        return false;
    }
}

static bool comment_line(char *input_line) {
    if (input_line[0] == '#') {
        return true;
    }

    return false;
}

static bool empty_line(char *input_line) {
    return input_line[0] == '\n';
}

static bool endl_ending(char *input_line) {
    size_t i = strlen(input_line) - 1;

    return input_line[i] == '\n';
}

// pobieramy pierwszy znak + sprawdzamy czy się zgadza
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

static bool out_of_range(char *token) {
    size_t i;

    for (i = 0; i < strlen(token); ++i) {
        if ((unsigned char)token[i] < 32) {
            return true;
        }
    }

    return false;
}

// parsujemy parametry - muszą to być liczby dodatnie !!!
// może to być zero,
// trzeba jeszcze uważać na duuże liczby !!!
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

static bool param_zero(uint32_t params[MAX_PARAMS]) {
    int i;

    for (i = 0; i < MAX_PARAMS; ++i) {
        if (params[i] == 0) {
            return true;
        }
    }

    return false;
}

static void launch_mode(char first_sign, uint32_t params[], uint32_t p_number) {
    if (p_number != 4) {
        fprintf(stderr, "ERROR %d\n", count_lines);
        return;
    }
    else if (param_zero(params)) {
        fprintf(stderr, "ERROR %d\n", count_lines);
        return;
    }

    if (first_sign == 'I') {
        printf("Przechodzimy w tryb interaktywny\n");
    }
    else if (first_sign == 'B') {
        gamma_game = gamma_new(params[0], params[1], params[2], params[3]);
        if (gamma_game == NULL) {
            fprintf(stderr, "ERROR %d\n", count_lines);
        }
        else {
            printf("OK %d\n", count_lines);
        }
    }
}

static void choose_option(char first_sign, uint32_t params[], uint32_t p_number) {
    if (!is_active()) {
        launch_mode(first_sign, params, p_number);
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
                printf("plansza się znulowałą !!!\n");
            }
            else {
                printf("%s", board);
                free(board);
            }
        }
        else {
            fprintf(stderr, "ERROR %d\n", count_lines);
        }
    }
}

// wyabstrahować jeszcze potem wypisywanie komunikatów
void parse_input(char *input_line) {
    uint32_t p_number = 0, params[MAX_PARAMS];
    char first_sign, *token;

    count_lines++;

    // sprawdzamy warunki wstępne
    if (comment_line(input_line) || empty_line(input_line)) {
        return;
    }

    first_sign = get_first_sign(input_line);
    // no po prostu nie kończy się endlem !!!
    if (!endl_ending(input_line)) {
        fprintf(stderr, "ERROR %d\n", count_lines);
        return;
    }
    else if (first_sign == 0) {
        fprintf(stderr, "ERROR %d\n", count_lines);
        return;
    }

    token = strtok(input_line, delim);

    // pierwszy znak jest ok, teraz sprawdzamy poprawność parametrów

    while ((token = strtok(0, delim))) {
        if (p_number == 4) {
            fprintf(stderr, "ERROR %d\n", count_lines);
            return;
        }

        // ale to może być gracz zero!!!
        if (!(got_param(token, &params[p_number++]))) {
            fprintf(stderr, "ERROR %d\n", count_lines);
            return;
        }
    }

    choose_option(first_sign, params, p_number);
}

void delete_game() {
    gamma_delete(gamma_game);
}

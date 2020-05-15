/**@file
 * Implementation of structure of single field
 * and other utilities for managing gamma game board.
 *
 * @author Antoni Koszowski <a.koszowski@students.mimuw.edu.pl>
 * @copyright
 * @date 17.04.2020
 */

#include <stdlib.h>
#include "board_utilities.h"

/**
 * Number of directions.
 */
#define DIR 4

/**
 * Array representing x-coordinate vectors.
 */
static const int dirx[DIR] = {-1, 0, 0, 1};
/**
 * Array representing y-coordinate vectors.
 */
static const int diry[DIR] = {0, 1, -1, 0};

field_t **alloc_board(uint32_t width, uint32_t height) {
    uint32_t i, j;

    field_t **b = malloc(sizeof(struct field *) * height);
    if (b == NULL) {
        return NULL;
    }

    for (i = 0; i < height; ++i) {
        b[i] = malloc(sizeof(struct field) * width);
        if (b[i] == NULL) {
            if (i == 0) {
                free(b);

                return NULL;
            }

            for (--i; i > 0; --i) {
                free(b[i]);
            }
            free(b[0]);
            free(b);

            return NULL;
        }

        for (j = 0; j < width; ++j) {
            b[i][j].owner_id = 0;
            b[i][j].rank = 1;
            b[i][j].rep = NULL;
            b[i][j].visited = false;
        }
    }

    return b;
}

void delete_board(field_t **b, uint32_t height) {
    if (b == NULL) {
        return;
    }

    uint32_t i = height - 1;

    while (i > 0) {
        free(b[i]);
        i--;
    }
    free(b[0]);
    free(b);
}

bool params_ok(uint32_t width, uint32_t height, uint32_t x, uint32_t y) {
    if (x >= width || y >= height) {
        return false;
    } else {
        return true;
    }
}

uint32_t field_owner(field_t *f) {
    return f->owner_id;
}

bool adjacent_field(field_t **b, uint32_t player_id,
                    uint32_t width, uint32_t height, uint32_t x, uint32_t y) {
    uint32_t i, cordx, cordy;

    for (i = 0; i < DIR; ++i) {
        cordx = x + dirx[i];
        cordy = y + diry[i];

        if (params_ok(width, height, cordx, cordy)) {
            if (field_owner(&b[cordy][cordx]) == player_id) {
                return true;
            }
        }
    }

    return false;
}

void set_up_field(field_t *f, uint32_t player_id) {
    f->owner_id = player_id;
    f->rep = f;
    f->rank = 0;
}

field_t *find_rep(field_t *f) {
    if (f->rep == NULL || f->rep == f) {
        return f;
    } else {
        return f->rep = find_rep(f->rep);
    }
}

uint32_t union_adj(field_t **b, uint32_t player_id,
                   uint32_t width, uint32_t height, uint32_t x, uint32_t y) {
    uint32_t i, cordx, cordy, counter = 0;
    field_t *field_rep = find_rep(&b[y][x]), *cur_rep;

    for (i = 0; i < DIR; ++i) {
        cordx = x + dirx[i];
        cordy = y + diry[i];

        if (params_ok(width, height, cordx, cordy)) {
            if (field_owner(&b[cordy][cordx]) == player_id) {
                cur_rep = find_rep(&b[cordy][cordx]);

                if (field_rep != cur_rep) {
                    if (field_rep->rank > cur_rep->rank) {
                        cur_rep->rep = field_rep;
                    } else if (field_rep->rank < cur_rep->rank) {
                        field_rep->rep = cur_rep;
                        field_rep = field_rep->rep;
                    } else {
                        cur_rep->rep = field_rep;
                        field_rep->rank++;
                    }

                    counter++;
                }
            }
        }
    }

    return counter;
}

/**@brief Sets all fields attribute visited to true.
 * Sets all fields on board pointed by @p b with width @p width
 * and height @p height to true.
 * @param[in] b           – pointer to the board,
 * @param[in] width       – width of the board,
 * @param[in] height      – height of the board.
 */
static void set_up_visited(field_t **b, uint32_t width, uint32_t height) {
    uint32_t i, j;

    for (i = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            if (params_ok(width, height, j, i)) {
                b[i][j].visited = false;
            }
        }
    }
}

/**@brief Searches and updates all fields in the current area.
 * Searches and updates all fields belonging to the player @p player_id
 * on board pointed by @p b with width @p width and height @p height
 * in the current area represented by field @p cur_rep.
 * Making next search from current field (@p x, @p y).
 * @param[in] b           – pointer to the board,
 * @param[in] cur_rep     – pointer to field representing current area,
 * @param[in] player_id   – player owning current area,
 * @param[in] width       – width of the board,
 * @param[in] height      – height of the board,
 * @param[in] x           – number of column of the current field,
 * @param[in] y           – number of row od the current field.
 */
static void dfs(field_t **b, field_t *cur_rep, uint32_t player_id,
                uint32_t width, uint32_t height, uint32_t x, uint32_t y) {
    uint32_t i, cordx, cordy;

    b[y][x].rep = cur_rep;
    b[y][x].visited = true;

    for (i = 0; i < DIR; ++i) {
        cordx = x + dirx[i];
        cordy = y + diry[i];

        if (params_ok(width, height, cordx, cordy)) {
            if (field_owner(&b[cordy][cordx]) == player_id) {
                if (!b[cordy][cordx].visited) {
                    dfs(b, cur_rep, player_id, width, height, cordx, cordy);
                }
            }
        }
    }
}

uint32_t divide_adj(field_t **b, uint32_t player_id,
                    uint32_t width, uint32_t height, uint32_t x, uint32_t y) {
    uint32_t i, cordx, cordy, counter = 0;
    field_t *cur_rep;

    set_up_visited(b, width, height);

    b[y][x].visited = true;

    for (i = 0; i < DIR; ++i) {
        cordx = x + dirx[i];
        cordy = y + diry[i];

        if (params_ok(width, height, cordx, cordy)) {
            if (field_owner(&b[cordy][cordx]) == player_id) {
                if (!b[cordy][cordx].visited) {
                    cur_rep = &b[cordy][cordx];

                    dfs(b, cur_rep, player_id, width, height, cordx, cordy);
                    counter++;
                }
            }
        }
    }

    return counter;
}

uint64_t count_free_fields(field_t **b, uint32_t player_id,
                           uint32_t width, uint32_t height) {
    uint32_t i, j;
    uint64_t counter = 0;

    for (i = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            if (adjacent_field(b, player_id, width, height, j, i)) {
                if (field_owner(&b[i][j]) == 0) {
                    counter++;
                }
            }
        }
    }

    return counter;
}

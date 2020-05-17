/**@file
 * Implementation of class storing state of gamma game.
 *
 * @author Antoni Koszowski <a.koszowski@students.mimuw.edu.pl>
 * @copyright
 * @date 17.04.2020
 */

#include <stdlib.h>
#include "player.h"
#include "board_utilities.h"
#include "gamma.h"

/**
 * Structure storing state of the gamma game.
 */
struct gamma {
    uint32_t width;                ///< width of the board
    uint32_t height;               ///< height of the board
    uint32_t players_num;          ///< number of players in game
    uint32_t max_areas;            ///< maximal number of areas one can occupy
    uint64_t globally_free_fields; ///< number of free fields on board
    field_t **board;               ///< pointer to structure representing board
    player_t **players;            ///< pointer to array storing state of players
};

/**@brief Checks preconditions.
 * Checks whether parameters @p g and @p player_id are both correct.
 * @param[in] g           – pointer to the structure storing game,
 * @param player_id       – player identifier.
 * @return Value @p true if both parameters are correct; @p false otherwise.
 */
static bool preconditions(gamma_t *g, uint32_t player_id) {
    if (g == NULL) {
        return false;
    } else if (player_id == 0 || player_id > g->players_num) {
        return false;
    } else {
        return true;
    }
}

/**@brief Updates state of player.
 * Updates state of player represented by structure pointed by @p p
 * whose identifier is @p player_id, inter alia, his status in game as well as
 * number of free fields he can capture on board
 * in game represented by the pointer @p g.
 * @param[in] g           – pointer to the current game,
 * @param[in] p           – pointer to the structure representing player,
 * @param[in] player_id   – number identifying player.
 */
static void update_player_state(gamma_t *g, player_t *p, uint32_t player_id) {
    uint64_t counted = 0;

    if (p->busy_areas == g->max_areas) {
        counted = count_free_fields(g->board, player_id, g->width, g->height);
        p->free_fields = counted;

        if (counted == 0 && p->golden_used) {
            p->in_game = false;
        } else if (!p->in_game) {
            p->in_game = true;
        }
    } else if (p->busy_areas < g->max_areas) {
        p->free_fields = g->globally_free_fields;
    }
}

gamma_t *gamma_new(uint32_t width, uint32_t height,
                   uint32_t players_num, uint32_t max_areas) {
    if (width == 0 || height == 0 || players_num == 0 || max_areas == 0) {
        return NULL;
    }

    gamma_t *g = malloc(sizeof(struct gamma));
    if (g == NULL) {
        return NULL;
    }

    g->width = width;
    g->height = height;
    g->players_num = players_num;
    g->max_areas = max_areas;
    g->globally_free_fields = width * height;

    g->board = alloc_board(width, height);
    if (g->board == NULL) {
        free(g);

        return NULL;
    }

    g->players = alloc_players(players_num);
    if (g->players == NULL) {
        delete_board(g->board, height);
        free(g);

        return NULL;
    }

    return g;
}

void gamma_delete(gamma_t *g) {
    if (g == NULL) {
        return;
    }

    delete_board(g->board, g->height);
    delete_players(g->players, g->players_num);
    free(g);
}

bool gamma_move(gamma_t *g, uint32_t player_id, uint32_t x, uint32_t y) {
    if (!preconditions(g, player_id)) {
        return false;
    } else if (!params_ok(g->width, g->height, x, y)) {
        return false;
    }

    uint32_t joined_areas = 0;
    player_t *cur_player = g->players[player_id];

    update_player_state(g, cur_player, player_id);

    if (!cur_player->in_game) {
        return false;
    } else if (field_owner(&g->board[y][x]) != 0) {
        return false;
    }

    // In case of new are is created,
    // checking whether max_areas limit is exceeded.
    if (!adjacent_field(g->board, player_id, g->width, g->height, x, y)) {
        if (cur_player->busy_areas + 1 > g->max_areas) {
            return false;
        } else {
            set_up_field(&g->board[y][x], player_id);

            cur_player->busy_areas++;
        }
    } else {
        set_up_field(&g->board[y][x], player_id);

        joined_areas = union_adj(g->board, player_id, g->width, g->height, x,
                                 y);
        cur_player->busy_areas -= joined_areas - 1;
    }

    cur_player->busy_fields++;
    g->globally_free_fields--;

    return true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player_id, uint32_t x, uint32_t y) {
    if (!preconditions(g, player_id)) {
        return false;
    } else if (!params_ok(g->width, g->height, x, y)) {
        return false;
    }

    uint32_t prev_owner_id = field_owner(&g->board[y][x]), areas_num = 0;
    player_t *prev_owner = g->players[prev_owner_id];
    player_t *cur_player = g->players[player_id];

    if (!gamma_golden_possible(g, player_id)) {
        return false;
    } else if (prev_owner_id == 0 || prev_owner_id == player_id) {
        return false;
    }

    // Splitting prev_owner areas, determining number of newly emerged areas.
    areas_num = prev_owner->busy_areas - 1;
    areas_num += divide_adj(g->board, prev_owner_id, g->width, g->height, x, y);

    if (areas_num > g->max_areas) {
        union_adj(g->board, prev_owner_id, g->width, g->height, x, y);

        return false;
    }

    g->board[y][x].owner_id = 0;

    // Checking whether cur_player can execute a move.
    if (!gamma_move(g, player_id, x, y)) {
        union_adj(g->board, prev_owner_id, g->width, g->height, x, y);
        g->board[y][x].owner_id = prev_owner_id;

        return false;
    }

    cur_player->golden_used = true;
    prev_owner->busy_areas = areas_num;
    prev_owner->busy_fields--;
    g->globally_free_fields++;

    return true;
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player_id) {
    if (!preconditions(g, player_id)) {
        return 0;
    }

    player_t *cur_player = g->players[player_id];

    return cur_player->busy_fields;
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player_id) {
    if (!preconditions(g, player_id)) {
        return 0;
    }

    player_t *cur_player = g->players[player_id];

    update_player_state(g, cur_player, player_id);

    return cur_player->free_fields;
}

bool gamma_golden_possible(gamma_t *g, uint32_t player_id) {
    if (!preconditions(g, player_id)) {
        return false;
    }

    player_t *cur_player = g->players[player_id];

    update_player_state(g, cur_player, player_id);

    if (cur_player->golden_used || !cur_player->in_game) {
        return false;
    } else if (cur_player->busy_fields + g->globally_free_fields ==
               g->width * g->height) {
        return false;
    }

    return true;
}

/**
 * Gives gamma game board cell width
 * @param players_num   - number of players.
 * @return Value @p counter - board cell width.
 */
static uint32_t get_cell_width(uint32_t players_num) {
    int counter = 0;

    while (players_num > 0) {
        counter++;
        players_num /= 10;
    }

    return counter;
}

char *gamma_board(gamma_t *g) {
    if (g == NULL) {
        return NULL;
    }

    bool dots = false;
    bool spaces = false;
    uint32_t cell_width = get_cell_width(g->players_num);
    uint64_t row_width = g->width * cell_width + 1;
    uint64_t cells = row_width * g->height;
    uint64_t i = 0, j = 0, k = 0, x = 0, y = 0, owner_id = 0;

    char *b = malloc(sizeof(char) * (cells + 1));
    if (b == NULL) {
        return NULL;
    }

    while (i < cells) {
        y = i / row_width;
        k = row_width * (g->height - y - 1) + x * cell_width;

        if (i % row_width == row_width - 1) {
            b[k] = '\n';
            x = 0;
            i++;
        } else {
            dots = false;
            spaces = false;
            owner_id = field_owner(&g->board[y][x]);

            if (owner_id == 0) {
                dots = true;
            }

            for (j = cell_width; j > 0; --j) {
                if (dots) {
                    b[k + j - 1] = '.';
                    dots = false;
                    spaces = true;
                } else if (spaces) {
                    b[k + j - 1] = ' ';
                } else {
                    b[k + j - 1] = '0' + owner_id % 10;
                    owner_id /= 10;

                    if (owner_id == 0) {
                        spaces = true;
                    }
                }
            }

            x++;
            i += cell_width;
        }
    }

    b[cells] = '\0';

    return b;
}

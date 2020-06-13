/**@file
 * Implementation of structure storing the state of single player.
 *
 * @author Antoni Koszowski <a.koszowski@students.mimuw.edu.pl>
 * @copyright
 * @date 17.04.2020
 */

#include <stdlib.h>
#include "player.h"

player_t **alloc_players(uint32_t players_num) {
    uint32_t i;

    //Since we use zero to mark fields that are not yet occupied.
    if (players_num == UINT32_MAX) {
        return NULL;
    }

    player_t **p = malloc(sizeof(struct player *) * (players_num + 1));
    if (p == NULL) {
        return NULL;
    }

    for (i = 0; i <= players_num; ++i) {
        p[i] = malloc(sizeof(struct player));
        if (p[i] == NULL) {
            for (--i; i > 0; --i) {
                free(p[i]);
            }
            free(p[0]);
            free(p);

            return NULL;
        }

        p[i]->in_game = true;
        p[i]->golden_used = false;
        p[i]->busy_areas = 0;
        p[i]->free_fields = 0;
        p[i]->busy_fields = 0;
    }

    return p;
}

void delete_players(player_t **p, uint32_t players_num) {
    uint32_t i = players_num;

    while (i > 0) {
        free(p[i]);
        i--;
    }
    free(p[0]);
    free(p);
}
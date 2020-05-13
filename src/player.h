/**@file
 * Interface of structure storing the state of single player.
 *
 * @author Antoni Koszowski <a.koszowski@students.mimuw.edu.pl>
 * @copyright
 * @date 17.04.2020
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Structure representing single player in gamma game.
 */
typedef struct player {
   bool in_game;         ///< depending on that if player can execute some move
   bool golden_used;     ///< depending on that if player used his golden move
   uint32_t busy_areas;  ///< number of occupied areas
   uint64_t busy_fields; ///< number of occupied fields
   uint64_t free_fields; ///< number of fields which player can still capture
 } player_t;

/** @brief Creates an array storing state of players.
 * Allocates memory for a new array storing pointers to state of players.
 * Initializes the structure, so that it stores initial state of players.
 * @param[in] players_num –  number of players, whom array is representing.
 * @return  Pointer to the newly created array or NULL in case of
 * memory was not allocated.
 */
player_t** alloc_players(uint32_t players_num);

/**@brief Deletes an array storing state of players.
 * Deletes from memory the array pointed by @p p
 * containing state of @p players_num players.
 * Nothing happens if the pointer's value is NULL.
 * @param[in] p           – pointer to array that will be removed,
 * @param[in] players_num – number of players, whom array is representing.
 */
void delete_players(player_t **p, uint32_t players_num);

#endif /* PLAYER_H */

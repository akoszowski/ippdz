/** @file
 * Interface of class storing state of gamma game.
 *
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 18.03.2020
 */

#ifndef GAMMA_H
#define GAMMA_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Structure storing state of the gamma game.
 */
typedef struct gamma gamma_t;

/** @brief Creates a structure storing the state of game.
 * Allocates memory for a new structure storing the state of game.
 * Initializes the structure so that it represents the initial state of game.
 * @param[in] width       – width of the board, positive number,
 * @param[in] height      – height of the board, positive number,
 * @param[in] players_num – number of players, that is positive,
 * @param[in] max_areas   – maximal, positive number of areas,
 *                          that one player can occupy.
 * @return Pointer to the newly created structure or NULL in case of memory
 * was not allocated or one of the parameters is incorrect.
 */
gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players_num, uint32_t max_areas);

/** @brief Deletes a structure storing the state of game.
 * Deletes from memory the structure pointed by @p g.
 * Nothing happens if the pointer's value is NULL.
 * @param[in] g          – pointer to structure that will be removed.
 */
void gamma_delete(gamma_t *g);

/** @brief Executes a move.
 * Places the piece of player @p player_id on field (@p x, @p y).
 * @param[in,out] g      – pointer to structure storing the state of game,
 * @param[in] player_id  – number of player, that is positive not bigger than
 *                         value @p players_num from function @ref gamma_new,
 * @param[in] x          – number of column, that is non-negative smaller than
 *                         value @p width from function @ref gamma_new,
 * @param[in] y          – number of row, that is non-negative smaller than
 *                         @p height from function @ref gamma_new.

 * @return Value @p true if move was executed;
 * @p false if move is illegal or one of the parameters is incorrect.
 */
bool gamma_move(gamma_t *g, uint32_t player_id, uint32_t x, uint32_t y);

/** @brief Executes golden move.
 * Places the piece of player @p player_id on field (@p x, @p y), occupied by
 * the another player, removing his piece.
 * @param[in,out] g      – pointer to structure storing the state of game,
 * @param[in] player_id  – number of player, that is positive not bigger than
 *                         value @p players_num from function @ref gamma_new,
 * @param[in] x          – number of column, that is non-negative smaller than
 *                         value @p width from function @ref gamma_new,
 * @param[in] y          – number of row, that is non-negative smaller than
 *                         @p height from function @ref gamma_new.
 * @return Value @p true if move was executed;
 * @p false if player already used his golden move, move is illegal,
 * or one of the parameters is incorrect.
 */
bool gamma_golden_move(gamma_t *g, uint32_t player_id, uint32_t x, uint32_t y);

/** @brief Gives number of fields occupied by a player.
 * Gives number of fields occupied by the player @p player_id.
 * @param[in] g          – pointer to structure storing the state of game,
 * @param[in] player_id  – number of player, that is positive not bigger than
 *                         value @p players_num from function @ref gamma_new.
 * @return Number of fields occupied by the player or zero
 * if one of the parameters is incorrect.
 */
uint64_t gamma_busy_fields(gamma_t *g, uint32_t player_id);

/** @brief Gives number of fields, which player can still capture.
 * Gives number of free fields on which, in the current state of game,
 * player @p player_id can place his piece in the next move.
 * @param[in] g          – pointer to structure storing the state of game,
 * @param[in] player_id  – number of player, that is positive not bigger than
 *                         value @p players_num from function @ref gamma_new.
 * @return Number of fields which player can still capture or zero
 * if one of the parameters is incorrect.
 */
uint64_t gamma_free_fields(gamma_t *g, uint32_t player_id);

/** @brief Checks whether player can execute a golden move.
 * Checks whether player @p player_id hasn't yet used a golden move
 * in this game and there is at least one field occupied by the another player.
 * @param[in] g          – pointer to structure storing the state of game,
 * @param[in] player_id  – number of player, that is positive not bigger than
 *                         value @p players_num from function @ref gamma_new.
 * @return Value @p true if player hasn't yet used golden move in this game
 * and there is at least one field occupied by the another player;
 * @p false otherwise.
 */
bool gamma_golden_possible(gamma_t *g, uint32_t player_id);

/** @brief Gives string depicting the state of board.
 * Allocates a buffer in memory that stores string containing text description
 * of the current state of board. Example can be found in file gamma_test.c.
 * Function calling must free this buffer.
 * @param[in] g          – pointer to structure storing the state of game,
 * @return Pointer to the allocated buffer containing string depicting
 * state of board or NULL if memory allocation failed.
 */
char* gamma_board(gamma_t *g);

#endif /* GAMMA_H */

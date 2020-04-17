/**@file
 * Interface of structure of single field
 * and other utilities for managing gamma game board.
 *
 * @author Antoni Koszowski <a.koszowski@students.mimuw.edu.pl>
 * @copyright
 * @date 17.04.2020
 */

#ifndef BOARD_UTILITIES_H
#define BOARD_UTILITIES_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Structure representing a field on gamma game board.
 */
typedef struct field field_t;

struct field {
    uint32_t owner_id; //< field owner identifier
    uint32_t rank;     //< rank of field necessary for find&union operations
    field_t *rep;      //< representative of field in terms of find&union sets
    bool visited;      //< attribute used by dfs algorithm
};

/** @brief Creates a structure storing gamma game board.
 * Allocates memory for a new two-dimensional array consisting of fields
 * storing the state of gamma game board.
 * Initializes the structure so that is represents the initial state of board.
 * @param[in] width       – width of the board,
 * @param[in] height      – height of the board.
 * @return  Pointer to the newly created structure or NULL in case of
 * memory was not allocated.
 */
field_t **alloc_board(uint32_t width, uint32_t height);

/**@brief Deletes a structure storing gamma game board.
 * Deletes from memory structure pointed by @p b.
 * @param[in] b           – pointer to structure that will be removed,
 * @param[in] height      – height of the board to be removed.
 */
void delete_board(field_t **b, uint32_t height);

/**@brief Checks whether given field is on board.
 * Checks whether field (@p x, @p y) is on board which width is @p width
 * and height @height.
 * @param[in] width        – width of the board,
 * @param[in] height       – height of the board,
 * @param[in] x            – number of column of checked field,
 * @param[in] y            – number of row of checked field.
 * @return Value @p true if field (@p x, @p y) is on board; @p false otherwise.
 */
bool params_ok(uint32_t width, uint32_t height, uint32_t x, uint32_t y);

/**@brief Gives identifier of field's owner.
 * Gives identifier of field's @p f owner.
 * @param[in] f           – pointer to the field.
 * @return Number which is field's owner identifier.
 */
uint32_t field_owner(field_t *f);

/** @brief Checks if some of the adjacent fields has the same owner.
 * Checks whether some of the adjacent fields to (@p x, @p y) on board
 * pointed by @p b with width @p width and height @p height
 * has the same owner @p player_id.
 * @param[in] b           – pointer to the board,
 * @param[in] player_id   – player owning current field,
 * @param[in] width       – width of the board,
 * @param[in] height      – height of the board,
 * @param[in] x           – number of column,
 * @param[in] y           – number of row.
 * @return Value @p true if some of the adjacent fields has the same owner;
 * @p false otherwise.
 */
bool adjacent_field(field_t **b, uint32_t player_id,
                    uint32_t width, uint32_t height, uint32_t x, uint32_t y);

/**@brief Sets up new owner of the field.
 * Sets up new owner @p player_id of the field pointed by @p f.
 * @param[in] f           – pointer to the field to be updated,
 * @param[in] player_id   – new owner.
 */
void set_up_field(field_t *f, uint32_t player_id);

/**@brief Finds representative field.
 * Finds representative field of the area which field pointed by @p f
 * is a part of.
 * @param[in] f           – pointer to the field.
 * @return Pointer to the representative field.
 */
field_t *find_rep(field_t *f);

/**@brief Joins adjacent fields. Gives number of areas that were joined.
 * Joins adjacent fields on board pointed by @p b with width @p width
 * and height @p height to field (@p x, @p y) owned by player @p player_id
 * into the same area.
 * Gives number of separate areas the were joined.
 * @param[in] b           – pointer to the board,
 * @param[in] player_id   – owner of the joined area,
 * @param[in] width       – width of the board,
 * @param[in] height      – height of the board,
 * @param[in] x           – number of column of the linking field,
 * @param[in] y           – number of row of the linking field.
 * @return Number of the separate areas that were joined.
 */
uint32_t union_adj(field_t **b, uint32_t player_id,
                   uint32_t width, uint32_t height, uint32_t x, uint32_t y);

/**@brief Splits adjacent fields. Gives a number of newly emerged areas.
 * Splits adjacent fields on board pointed by @p b with width @p width
 * and height @p height to field (@p x, @p y) owned by player @p player_id.
 * Gives a number of newly emerged areas after exclusion of field (@p x, @p y).
 * @param[in] b           – pointer to the board
 * @param[in] player_id   – owner of the splited area,
 * @param[in] width       – width of the board,
 * @param[in] height      – height of the board,
 * @param[in] x           – number of column of the excluded field,
 * @param[in] y           – number of row of the excluded field.
 * @return Number of newly emerged areas.
 */
uint32_t divide_adj(field_t **b, uint32_t player_id,
                    uint32_t width, uint32_t height, uint32_t x, uint32_t y);

/**@brief Gives number of free fields.
 * Gives number of free fields which player @p player_id can still capture
 * on board pointed by @p b with width @p width and height @p height.
 * @param b               – pointer to the board,
 * @param player_id       – player whose free fields will be counted,
 * @param width           – width of the board,
 * @param height          – height of the board.
 * @return Number of free fields which player can still capture.
 */
uint64_t count_free_fields(field_t **b, uint32_t player_id,
                           uint32_t width, uint32_t height);


#endif /* BOARD_UTILITIES_H */

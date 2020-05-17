/**@file
 * Interface of gamma game interactive mode.
 *
 * @author Antoni Koszowski <a.koszowski@students.mimuw.edu.pl>
 * @copyright
 * @date 17.05.2020
 */
#include <stdint.h>
#include "gamma.h"


#ifndef GAMMA_GAMMA_INTERACTIVE_H
#define GAMMA_GAMMA_INTERACTIVE_H

/**@brief Launches gamma game interactive mode.
 * Launches gamma game interactive mode, where game structure is pointed by
 * @p game_ptr, with given game parameters - @p widht, @p height,
 * @p players_num.
 * @param game_ptr      - pointer to game structure,
 * @param width         - game board width,
 * @param height        - game board height,
 * @param players_num   - number of players in game.
 */
void launch_interactive(gamma_t *game_ptr,  uint32_t width, uint32_t height,
                        uint32_t players_num);

#endif //GAMMA_GAMMA_INTERACTIVE_H

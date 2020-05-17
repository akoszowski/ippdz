/**@file
 * Interface of gamma game input parser.
 *
 * @author Antoni Koszowski <a.koszowski@students.mimuw.edu.pl>
 * @copyright
 * @date 17.05.2020
*/

#ifndef GAMMA_GAMMA_PARSER_H
#define GAMMA_GAMMA_PARSER_H

/**@brief Parses and interprets input.
 * Parses input lines and then interprets it.
 * In case that command was matched it is realized.
 * @param input_line - input line to be parsed.
 */
void parse_input(char *input_line);

/**@brief Deletes gamma game.
 * Deletes from memory structure storing gamma game.
 */
void delete_game();

#endif //GAMMA_GAMMA_PARSER_H

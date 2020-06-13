/**@file
 * Implementation of gamma game interactive mode.
 *
 * @author Antoni Koszowski <a.koszowski@students.mimuw.edu.pl>
 * @copyright
 * @date 17.05.2020
 */

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include "gamma_interactive.h"

/**
 * Macro checking for keyboard combination with CTRL and given key.
 */
#define CTRL_KEY(k) ((k) & 0x1f)
/**
 * Onscreen information height.
 */
#define INFO_HEIGHT 12
/**
 * Onscreen information width.
 */
#define INFO_WIDTH 24

/**
 * Structure representing current configuration of terminal.
 */
struct terminal {
    bool tips;                   ///< controlling whether tips are on/off
    uint64_t cursor_x;           ///< cursor x-coordinate position
    uint64_t cursor_y;           ///< cursor y-coordinate position
    uint32_t screen_rows;        ///< number of screen rows
    uint32_t screen_cols;        ///< number of screen columns
    struct termios orig_config;  ///< original terminal configuration
};

/**
 * Prompt when tips are off.
 */
const char *tips_off = "\nPress t to see game tips";

/**
 * Prompt when tips are off.
 */
const char *tips_on = "\nPress t to hide game tips"
                      "\n\x1b[44m \x1b[0m: cursor position"
                      "\nPress space to make a standard move"
                      "\nPress g or G to make a golden move"
                      "\nPress c or C to skip your move"
                      "\nPress CTRL-D to end up the game";

/**
 * Structure representing current parameters of gamma game.
 */
struct game_params {
    uint32_t width;         ///< board width
    uint32_t height;        ///< board height
    uint32_t players_num;   ///< number of players
    uint32_t cur_player;    ///< number of current player
    uint32_t cell_width;    ///< board cell width
};

/**
 * Structure representing screen buffer.
 */
struct screen_buffer {
    char *buf;          ///< content of buffer
    uint64_t len;       ///< length of buffer
};

/**
 * Structure representing recognizable keyboard commands.
 */
enum key {
    ARROW_LEFT = 1000,  ///< left arrow
    ARROW_RIGHT,        ///< right arrow
    ARROW_UP,           ///< arrow up
    ARROW_DOWN,         ///< arrow down
    N_MOVE,             ///< normal move - pressing space
    G_MOVE,             ///< golden move - pressing G or g
    RESIGN,             ///< player resigning from move - pressing C or c
    TIPS                ///< player folding/unfolding game tips - pressing t
};

/**
 * Pointer to structure storing gamma game state.
 */
gamma_t *gamma_game = NULL;

/**
 * Structure storing current terminal configuration.
 */
struct terminal config;

/**
 * Structure storing gamma game parameters.
 */
struct game_params g_ps;

/**
 * Disables terminal raw mode.
 */
static void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.orig_config) == -1) {
        exit(1);
    }
}

/**
 * Enables terminal raw mode.
 */
static void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &config.orig_config) == -1) {
        exit(1);
    }
    atexit(disable_raw_mode);

    struct termios raw = config.orig_config;

    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        exit(1);
    }
}

/**
 * Acquires terminal window size.
 */
static void get_window_size() {
    struct winsize winsize;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize) == -1) {
        exit(1);
    }

    config.screen_rows = winsize.ws_row;
    config.screen_cols = winsize.ws_col;
}

/**
 * Checks whether in terminal with given window size
 * gamma game board can be screened.
 */
static void check_window() {
    if (config.screen_rows - INFO_HEIGHT < g_ps.height ||
        config.screen_cols < INFO_WIDTH ||
        config.screen_cols < g_ps.width * g_ps.cell_width) {
        printf("Unfortunately, size of your window is not big enough for given parameters.\n"
               "Try to enlarge the window or change game parameters !!!\n");
        exit(1);
    }
}

/**
 * Reads and interprets keyboard presses/ escape code sequences.
 * @return Character @p c converted if recognizable into enum.
 */
static int read_key() {
    char c;

    if (read(STDIN_FILENO, &c, 1) == -1) {
        exit(1);
    }

    if (c == '\x1b') {
        char next[3];

        if (read(STDIN_FILENO, &next[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &next[1], 1) != 1) return '\x1b';

        if (next[0] == '[') {
            switch (next[1]) {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
            }
        }

        return '\x1b';
    } else if (c == ' ') {
        return N_MOVE;
    } else if (c == 'G' || c == 'g') {
        return G_MOVE;
    } else if (c == 'C' || c == 'c') {
        return RESIGN;
    } else if (c == 't') {
        return TIPS;
    }

    return c;
}

/**
 * Appends new string @p s, with length @p len
 * to the screen buffer pointed by @p content.
 * @param content  - pointer to the buffer,
 * @param s        - appended string,
 * @param len      - length of appended string.
 */
static void append(struct screen_buffer *content, const char *s, int len) {
    char *new = realloc(content->buf, content->len + len);
    if (new == NULL) {
        exit(1);
    }

    memcpy(&new[content->len], s, len);
    content->buf = new;
    content->len += len;
}

/**
 * Prints gamma game board into the terminal window.
 */
static void print_board() {
    char *board = gamma_board(gamma_game);
    if (board == NULL) {
        exit(1);
    }

    // Clearing the screen.
    if (write(STDOUT_FILENO, "\x1b[2J", 4) == -1) {
    }
    // Cursor to first row and column.
    if (write(STDOUT_FILENO, "\x1b[H", 3) == -1) {
        exit(1);
    }

    printf("%s", board);

    free(board);
}

/**
 * Prints to the terminal results of current game that already has ended.
 */
static void print_results() {
    uint32_t i;
    uint64_t b_f;
    char buf[128];
    struct screen_buffer results = {NULL, 0};

    snprintf(buf, sizeof(buf), "\n\x1b[4mGAMMA GAME SUMMARY:\x1b[0m\n"
                               "\nPLAYER ID | BUSY_FIELDS\n");
    append(&results, buf, strlen(buf));

    for (i = 1; i <= g_ps.players_num; ++i) {
        b_f = gamma_busy_fields(gamma_game, i);
        snprintf(buf, sizeof(buf), "PLAYER %d  | %ld\n", i, b_f);

        append(&results, buf, strlen(buf));
    }

    if (write(STDOUT_FILENO, results.buf, results.len) == -1) {
        exit(1);
    }

    free(results.buf);
}

/**
 * Moves cursor in respect to interpreted @p key.
 * @param key   - interpeted keybord press.
 */
static void move_cursor(int key) {
    switch (key) {
        case ARROW_LEFT:
            if (config.cursor_x != g_ps.cell_width) {
                config.cursor_x -= g_ps.cell_width;
            }
            break;
        case ARROW_RIGHT:
            if (config.cursor_x != g_ps.width * g_ps.cell_width) {
                config.cursor_x += g_ps.cell_width;
            }
            break;
        case ARROW_UP:
            if (config.cursor_y != 1) {
                config.cursor_y--;
            }
            break;
        case ARROW_DOWN:
            if (config.cursor_y != g_ps.height) {
                config.cursor_y++;
            }
            break;
    }
}

/**
 * Updates gamma game parameters.
 */
static void update_params() {
    int counter = g_ps.players_num;
    uint32_t next_p = (g_ps.cur_player % g_ps.players_num) + 1;
    bool f_f = (gamma_free_fields(gamma_game, next_p) != 0);
    bool g_p = gamma_golden_possible(gamma_game, next_p);

    while (!f_f && !g_p && counter > 0) {
        next_p = (next_p % g_ps.players_num) + 1;
        f_f = gamma_free_fields(gamma_game, next_p);
        g_p = gamma_golden_possible(gamma_game, next_p);
        counter--;
    }

    if (counter == 0) {
        if (write(STDOUT_FILENO, "\x1b[2J", 4) == -1) {
            exit(1);
        }
        if (write(STDOUT_FILENO, "\x1b[H", 3) == -1) {
            exit(1);
        }
        if (write(STDOUT_FILENO, "\x1b[?25h", 6) == -1) {
            exit(1);
        }

        print_board();
        print_results();

        gamma_delete(gamma_game);

        exit(0);
    } else {
        g_ps.cur_player = next_p;
    }
}

/**
 * Prints incentive and basic information pointed by @p content
 * about current player.
 * @param content       - printed information.
 */
static void get_info(struct screen_buffer *content) {
    char b[512];
    int c_p = g_ps.cur_player;
    int b_f = gamma_busy_fields(gamma_game, c_p);
    int f_f = gamma_free_fields(gamma_game, c_p);

    snprintf(b, sizeof(b), "Current player:  %d\n"
                           "Busy fields:     %d\n"
                           "Free fields:     %d\n", c_p, b_f, f_f);

    append(content, b, strlen(b));

    if (gamma_golden_possible(gamma_game, c_p)) {
        snprintf(b, sizeof(b), "Golden move possible\n");
    } else {
        snprintf(b, sizeof(b), "Golden move not possible\n");
    }
    append(content, b, strlen(b));

    if (!config.tips) {
        append(content, tips_off, strlen(tips_off));
    } else {
        append(content, tips_on, strlen(tips_on));
    }
}

/**
 * Sets cursor in terminal window on given position given by appending
 * escape code in to screen buffer pointed by @p content.
 * @param content   - ponter to screen buffer.
 */
static void set_cursor(struct screen_buffer *content) {
    uint32_t x = config.cursor_x / g_ps.cell_width - 1;
    uint32_t y = g_ps.height - config.cursor_y;

    char b[128];
    char *temp = get_cell_content(gamma_game, x, y);
    snprintf(b, sizeof(b), "\x1b[%lu;%luH\x1b[44m%s\x1b[0m", config.cursor_y,
             config.cursor_x - g_ps.cell_width + 1, temp);

    append(content, b, strlen(b));

    free(temp);
}

/**
 * Refreshes, actualizes content of the terminal window.
 */
static void refresh_screen() {
    print_board();

    struct screen_buffer info = {NULL, 0};

    get_info(&info);

    set_cursor(&info);

    if (write(STDOUT_FILENO, info.buf, info.len) == -1) {
        exit(1);
    }
    free(info.buf);
}

/**
 * Processes, interprets consecutive keypresses.
 */
static void process_keypress() {
    int c = read_key();
    uint32_t x = config.cursor_x / g_ps.cell_width - 1;
    uint32_t y = g_ps.height - config.cursor_y;

    switch (c) {
        case CTRL_KEY('d'):
            if (write(STDOUT_FILENO, "\x1b[2J", 4) == -1) {
                exit(1);
            }
            if (write(STDOUT_FILENO, "\x1b[H", 3) == -1) {
                exit(1);
            }
            if (write(STDOUT_FILENO, "\x1b[?25h", 6) == -1) {
                exit(1);
            }

            print_board();
            print_results();

            gamma_delete(gamma_game);

            exit(0);
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            move_cursor(c);
            break;
        case N_MOVE:
            if (gamma_move(gamma_game, g_ps.cur_player, x, y)) {
                update_params();
            }
            break;
        case G_MOVE:
            if (gamma_golden_move(gamma_game, g_ps.cur_player, x, y)) {
                update_params();
            }
            break;
        case RESIGN:
            update_params();
            break;
        case TIPS:
            config.tips = !config.tips;
            break;
    }
}

/**
 * Initializes basic game paramters, given by @p width, @p height,
 * @p players_num.
 * @param width         - game board width,
 * @param height        - game board height,
 * @param players_num   - number of players in game.
 */
void init(uint32_t width, uint32_t height, uint32_t players_num) {
    g_ps.width = width;
    g_ps.height = height;
    g_ps.players_num = players_num;
    g_ps.cur_player = 1;
    g_ps.cell_width = get_cell_width(players_num);

    config.cursor_x = g_ps.cell_width;
    config.cursor_y = height;
    config.tips = true;

    get_window_size();

    check_window();
}

void launch_interactive(gamma_t *game_ptr, uint32_t width, uint32_t height,
                        uint32_t players_num) {
    gamma_game = game_ptr;
    if (gamma_game == NULL) {
        exit(1);
    }

    enable_raw_mode();

    init(width, height, players_num);

    if (write(STDOUT_FILENO, "\x1b[?25l", 6) == -1) {
        exit(1);
    }

    while (1) {
        refresh_screen();
        process_keypress();
    }
}

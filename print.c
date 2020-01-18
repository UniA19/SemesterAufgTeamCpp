#include "print.h"
#include "row_formats.inc"

/* The gap between left and right fields */
#define print_gap() (printf("        "))
#define printh_stat_line(print_utf, max_ship_length) \
{\
        int length;\
        print_utf ? printf("───────┼") : printf("-------+");\
        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) print_utf ? printf("───%s", (length == max_ship_length) ? "┤" : "┼") : printf("---+");\
}


/* prints horizontal line, deviding the rows (--+---+---+...) */
void print_hline(int nx, int print_utf)
{
        if (print_utf) {
                if (print_utf == 1) {
                        int coli;
                        printf("──┼");
                        for (coli = 0; coli < nx; ++coli) {
                                if (coli == nx - 1) {
                                        printf("───┤");
                                } else {
                                        printf("───┼");
                                }
                        }
                } else {
                        int coli;
                        printf("──┴");
                        for (coli = 0; coli < nx; ++coli) {
                                if (coli == nx - 1) {
                                        printf("───┘");
                                } else {
                                        printf("───┴");
                                }
                        }
                }
        } else {
                int coli;
                printf("--+");
                for (coli = 0; coli < nx; ++coli) {
                        printf("---+");
                }
        }
}

/* prints top row (  | A | B | C | ... ) */
void print_top_row(int nx, int print_utf)
{
        if (print_utf) {
                int coli;
                printf("  │");
                for (coli = 0; coli < nx; ++coli) {
                        printf("%c%c │", (coli < 26 ? ' ' : (coli / 26 - 1) + 'A'), ((coli % 26) + 'A'));
                }
        } else {
                int coli;
                printf("  |");
                /* format as "AA" etc if there are more than 26 cols */
                for (coli = 0; coli < nx; ++coli) {
                        printf("%c%c |", (coli < 26 ? ' ' : (coli / 26 - 1) + 'A'), ((coli % 26) + 'A'));
                }
        }
}

/* prints row including the numbering at the beginning of the line ( 1|~~~|XXX|~~~|...) */
void print_row(shipstate_t row[], int nx, int row_num, int is_left_field, int print_color, int print_utf)
{
        int coli;

        printf("%2i%s", row_num, (print_utf ? "│" : "|"));

        for (coli = 0; coli < nx; ++coli) {
                shipstate_t state = row[coli];
#ifndef SHOW_BOTS_SHIP
                if (is_left_field) {
                        if (is_unhit(state)) {
                                state = NONE; /* keep hidden */
                        }
                        if (is_hit(state)) {
                                state = is_invert(state) ? HIT_INVERT : HIT;
                        }
                }
#endif
                if (print_color) {
                        if (print_utf) {
                                printf(row_formats_color_utf[state]);
                        } else {
                                printf(row_formats_color[state]);
                        }
                } else if (print_utf) {
                        printf(row_formats_utf[state]);
                } else {
                        printf(row_formats[state]);
                }

                row[coli] = remove_invert(row[coli]);
        }
}

/* prints both entire battleship-fields */
void print_field(play_fields_t *fld)
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_left = fld->data_left;
        shipstate_t **data_right = fld->data_right;
        const int print_vertical = fld->print_vertical;
        const int print_color = fld->print_color;
        const int print_utf = fld->print_utf;

        const char bot_title[] = "BOT's ships";
        const char player_title[] = "PLAYER's ships";
        const int nCharsX = 4 * nx + 3;

        if (print_vertical) {
                int players_field;
                for (players_field = 0; players_field <= 1; ++players_field) {
                        int row;
                        /* title: e.g.:     BOT's ships     */
                        if (!players_field) {
                                if (print_color) printf("\033[31;1m");
                                printf("\n%*s%*s\n", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                        } else {
                                if (print_color) printf("\033[32;1m");
                                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                        }
                        if (print_color) printf("\033[0m");
                        /* top row:   | A | B |...*/
                        print_top_row(nx, print_utf);
                        printf("\n");
                        for (row = 0; row < ny; ++row) {
                                /* horizontal line: --+---+---+...*/
                                print_hline(nx, print_utf);
                                printf("\n");
                                /* other rows:   <row>|~~~|XXX|...*/
                                if (!players_field) {
                                        print_row(data_left[row], nx, row + 1, 1, print_color, print_utf);
                                } else {
                                        print_row(data_right[row], nx, row + 1, 0, print_color, print_utf);
                                }
                                printf("\n");
                        }
                        /* horizontal line: --+---+---+...*/
                        print_hline(nx, 2 * print_utf);
                        printf("\n\n");
                }
        } else {
                int row;
                /* title: e.g.:     BOT's ships     */
                if (print_color) printf("\033[31;1m");
                printf("\n%*s%*s", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                print_gap();
                if (print_color) printf("\033[32;1m");
                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                if (print_color) printf("\033[0m");

                /* top row:   | A | B |...          | A | B |... */
                print_top_row(nx, print_utf);
                print_gap();
                print_top_row(nx, print_utf);
                printf("\n");
                for (row = 0; row < ny; ++row) {
                        /* horizontal line: --+---+---+...        --+---+---+... */
                        print_hline(nx, print_utf);
                        print_gap();
                        print_hline(nx, print_utf);
                        printf("\n");
                        /* other rows:   <row>|~~~|XXX|...          |   |XXX|... */
                        print_row(data_left[row], nx, row + 1, 1, print_color, print_utf);
                        print_gap();
                        print_row(data_right[row], nx, row + 1, 0, print_color, print_utf);
                        printf("\n");
                }
                /* horizontal line: --+---+---+...        --+---+---+... */
                print_hline(nx, 2 * print_utf);
                print_gap();
                print_hline(nx, 2 * print_utf);
                printf("\n\n");
        }
}

/* Counts how many ships ships of which length have how many hits */
void countShipHits(shipstate_t **field, int nx, int ny, int ship_count[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1]) {
        int i;
        int y;
        for (i = 0; i <= MAX_SHIP_LENGTH; ++i) {
                int j;
                for (j = 0; j <= MAX_SHIP_LENGTH; ++j) {
                        ship_count[i][j] = 0;
                }
        }
        for (y = 0; y < ny; ++y) {
                int x;
                for (x = 0; x < nx; ++x) {
                        if (is_left(field[y][x])) {
                                /* count length and number of horizontal, hit ship-boxes and increment the counter in ship_count[<length>][<number of hits>] */
                                int length = 0, n_hits = 0;
                                do  {
                                        if (has_been_shot(field[y][x + length])) {
                                                ++n_hits;
                                        }
                                        ++length;
                                } while (!is_right(field[y][x + length - 1]));
                                ++ship_count[length][n_hits];
                        } else if (is_top(field[y][x])) {
                                /* count length and number of vertical, hit ship-boxes and increment the counter in ship_count[<length>][<number of hits>] */
                                int length = 0, n_hits = 0;
                                do {
                                        if (has_been_shot(field[y + length][x])) {
                                                ++n_hits;
                                        }
                                        ++length;
                                } while (!is_bottom(field[y + length - 1][x]));
                                ++ship_count[length][n_hits];
                        } else if (is_single(field[y][x])) {
                                /* increment the number of unhit and hit single ships accordingly in ship_count[<length>][<number of hits>] */
                                has_been_shot(field[x][y]) ? ++ship_count[1][1] : ++ship_count[1][0];
                        }
                }
        }
}

/* prints the statistics at the end of the game */
void print_stats(play_fields_t *fld, int n_ships_total[])
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        const int max_ship_length = fld->max_ship_length;
        shipstate_t **data_left = fld->data_left;
        shipstate_t **data_right = fld->data_right;
        const int print_vertical = fld->print_vertical;
        const int print_color = fld->print_color;
        const int print_utf = fld->print_utf;

        const char bot_title[] = "BOT's ships";
        const char player_title[] = "PLAYER's ships";
        const int nCharsX = 4 * max_ship_length + 8;
        char vert_bar[sizeof("│")];
        int length, n_hits;

        strcpy(vert_bar, print_utf ? "│" : "|");


        if (print_vertical) {
                /* ship_count[<length>][<n_hits>] corresponds to the number of ships of length <length> that have been hit <n_hits> times */
                int ship_count[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1];
                int players_field;
                for (players_field = 0; players_field <= 1; ++players_field) {
                        if (!players_field) {
                                countShipHits(data_left, nx, ny, ship_count);
                                if (print_color) printf("\033[31;1m");
                                printf("\n%*s%*s\n", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                                if (print_color) printf("\033[0m");
                        } else {
                                countShipHits(data_right, nx, ny, ship_count);
                                if (print_color) printf("\033[32;1m");
                                printf("\n%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                                if (print_color) printf("\033[0m");
                        }

                        printf("%slength:%s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", length, vert_bar);

                        printf("\n");
                        printh_stat_line(print_utf, max_ship_length);
                        printf("\n");
                        for (n_hits = 0; n_hits <= max_ship_length; ++n_hits) {
                                printf("%s%2i hit%c%s%s", print_color ? "\033[1m" : "", n_hits, (n_hits == 1) ? ' ' : 's', print_color ? "\033[0m" : "", vert_bar);
                                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                                        if (n_hits <= length) print_color ? printf("\033[%sm%3i\033[0m%s", (n_hits == length ? "35" : "31"), ship_count[length][n_hits], vert_bar) : printf("%3i%s", ship_count[length][n_hits], vert_bar);
                                        else printf("   %s", vert_bar);
                                }
                                printf("\n");
                        }
                        printh_stat_line(print_utf, max_ship_length);
                        printf("\n%s  sum: %s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", n_ships_total[length], vert_bar);
                        printf("\n");
                }
        } else {
                /* ship_count[<length>][<n_hits>] corresponds to the number of ships of length <length> that have been hit <n_hits> times */
                int ship_count_left[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1];
                int ship_count_right[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1];
                countShipHits(data_left, nx, ny, ship_count_left);
                countShipHits(data_right, nx, ny, ship_count_right);

                /* title: e.g.:     BOT's ships     */
                if (print_color) printf("\033[31;1m");
                printf("\n%*s%*s", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                print_gap();
                if (print_color) printf("\033[32;1m");
                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                if (print_color) printf("\033[0m");

                printf("%slength:%s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", length, vert_bar);
                print_gap();
                printf("%slength:%s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", length, vert_bar);

                printf("\n");
                printh_stat_line(print_utf, max_ship_length);
                print_gap();
                printh_stat_line(print_utf, max_ship_length);
                printf("\n");

                for (n_hits = 0; n_hits <= max_ship_length; ++n_hits) {
                        printf("%s%2i hit%c%s%s", print_color ? "\033[1m" : "", n_hits, (n_hits == 1) ? ' ' : 's', print_color ? "\033[0m" : "", vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                                if (n_hits <= length) print_color ? printf("\033[%sm%3i\033[0m%s", (n_hits == length ? "35" : "31"), ship_count_left[length][n_hits], vert_bar) : printf("%3i%s", ship_count_left[length][n_hits], vert_bar);
                                else printf("   %s", vert_bar);
                        }
                        print_gap();
                        printf("%s%2i hit%c%s%s", print_color ? "\033[1m" : "", n_hits, (n_hits == 1) ? ' ' : 's', print_color ? "\033[0m" : "", vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                                if (n_hits <= length) print_color ? printf("\033[%sm%3i\033[0m%s", (n_hits == length ? "35" : "32"), ship_count_right[length][n_hits], vert_bar) : printf("%3i%s", ship_count_right[length][n_hits], vert_bar);
                                else printf("   %s", vert_bar);
                        }
                        printf("\n");
                }
                printh_stat_line(print_utf, max_ship_length);
                print_gap();
                printh_stat_line(print_utf, max_ship_length);

                printf("\n%s  sum: %s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) print_color ? printf("\033[31m%3i\033[0m%s", n_ships_total[length], vert_bar) : printf("%3i%s", n_ships_total[length], vert_bar);
                print_gap();
                printf("%s  sum: %s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) print_color ? printf("\033[32m%3i\033[0m%s", n_ships_total[length], vert_bar) : printf("%3i%s", n_ships_total[length], vert_bar);
                printf("\n\n");
        }
}

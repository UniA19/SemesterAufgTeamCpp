#include "print.h"
#include "row_formats.inc"

/* The gap between left and right fields, when not printing vertically */
#define print_gap() (printf("        "))

/* prints horizontal line, deviding the rows (--+---+---+...) */
void print_hline(int nx, int print_utf)
{
        int coli;
        /* special handling for closing line */
        if (print_utf == 2) {
                /* beginnning of row */
                printf("──┴");
                /* rest of row */
                for (coli = 0; coli < nx - 1; ++coli) {
                        printf("───┴");
                }
                /* special case for end of the row */
                printf("───┘");
        /* regular line */
        } else if (print_utf) {
                /* beginnning of row */
                printf("──┼");
                /* rest of row */
                for (coli = 0; coli < nx - 1; ++coli) {
                        printf("───┼");
                }
                /* special case for end of the row */
                printf("───┤");
        /* ASCII line */
        } else {
                /* beginnning of row */
                printf("--+");
                /* rest of row */
                for (coli = 0; coli < nx; ++coli) {
                        printf("---+");
                }
        }
}

/* prints top row (  | A | B | C | ... ) */
void print_top_row(int nx, int print_utf)
{
        int coli;
        /* Vertical bar either in UTF-8 box-drawing or as ASCII */
        char vert_bar[sizeof("│")];
        strcpy(vert_bar, print_utf ? "│" : "|");

        /* Print space at the beginnning of each row ('  |') */
        printf("  %s", vert_bar);
        /* Print letters signaling the column number (' A |')*/
        for (coli = 0; coli < nx; ++coli) {
                printf("%c%c %s", (coli < 26 ? ' ' : (coli / 26 - 1) + 'A'), ((coli % 26) + 'A'), vert_bar);
        }
}

/* prints row including the numbering at the beginning of the line ( 1|~~~|XXX|~~~|...) */
void print_row(int nx, shipstate_t row[], int row_num, int is_left_field, int print_color, int print_utf)
{
        int coli;

        /* Print numbering at the beginnning of each row */
        printf("%2i%s", row_num, (print_utf ? "│" : "|"));

        /* Print column for column */
        for (coli = 0; coli < nx; ++coli) {
                /* copy of the stae of the current field */
                shipstate_t state = row[coli];
                /* hide unhit and hide direction of hit ships on the left field,
                        so the Player doesn't know where Bot's ships are */
                if (is_left_field) {
                        if (is_unhit(state)) {
                                state = NONE;
                        }
                        if (is_hit(state)) {
                                state = is_invert(state) ? HIT_INVERT : HIT;
                        }
                }
                /* Different possible printing modes */
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

                /* remove inversion after having printed it */
                row[coli] = remove_invert(row[coli]);
        }
}

/* prints both entire battleship-fields */
void print_field(play_fields_t *fld)
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **field_left = fld->field_left;
        shipstate_t **field_right = fld->field_right;
        const int print_vertical = fld->print_vertical;
        const int print_color = fld->print_color;
        const int print_utf = fld->print_utf;

        const char bot_title[] = "BOT's ships";
        const char player_title[] = "PLAYER's ships";
        /* Total character width of the field */
        const int nCharsX = 4 * nx + 3;

        /* when printing the fields above eachother */
        if (print_vertical) {
                int players_field;
                for (players_field = 0; players_field <= 1; ++players_field) {
                        int row;
                        /* title: e.g.: "     BOT's ships     " */
                        if (!players_field) {
                                print_bold_red(print_color);
                                /* print "BOT's ships" centered */
                                printf("\n%*s%*s\n", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                        } else {
                                print_bold_green(print_color);
                                /* print "PLAYER's ships" centered */
                                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                        }
                        print_nocolor(print_color);

                        /* top row:   | A | B |...*/
                        print_top_row(nx, print_utf);
                        putchar('\n');
                        for (row = 0; row < ny; ++row) {
                                /* horizontal line: --+---+---+...*/
                                print_hline(nx, print_utf);
                                putchar('\n');
                                /* other rows:   <row>|~~~|XXX|...*/
                                if (!players_field) {
                                        /* Bot's rows */
                                        print_row(nx, field_left[row], row + 1, 1, print_color, print_utf);
                                } else {
                                        /* Player's rows */
                                        print_row(nx, field_right[row], row + 1, 0, print_color, print_utf);
                                }
                                putchar('\n');
                        }
                        /* horizontal line: --+---+---+...*/
                        print_hline(nx, 2 * print_utf);
                        printf("\n\n");
                }
        /* when printing the fields beside eachother */
        } else {
                int row;
                /* title:     BOT's ships               PLAYER's ships */
                print_bold_red(print_color);
                /* print "BOT's ships" centered */
                printf("\n%*s%*s", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                print_gap();
                print_bold_green(print_color);
                /* print "PLAYER's ships" centered */
                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                print_nocolor(print_color);

                /* top row:   | A | B |...          | A | B |... */
                print_top_row(nx, print_utf);
                print_gap();
                print_top_row(nx, print_utf);
                putchar('\n');
                for (row = 0; row < ny; ++row) {
                        /* horizontal line: --+---+---+...        --+---+---+... */
                        print_hline(nx, print_utf);
                        print_gap();
                        print_hline(nx, print_utf);
                        putchar('\n');
                        /* other rows:   <row>|~~~|XXX|...          |   |XXX|... */
                        print_row(nx, field_left[row], row + 1, 1, print_color, print_utf);
                        print_gap();
                        print_row(nx, field_right[row], row + 1, 0, print_color, print_utf);
                        putchar('\n');
                }
                /* horizontal line: --+---+---+...        --+---+---+... */
                print_hline(nx, 2 * print_utf);
                print_gap();
                print_hline(nx, 2 * print_utf);
                printf("\n\n");
        }
}

/* prints horizontal line, deviding the rows in the statistics at the bottom (------+---+---+...) */
void print_stat_hline(int max_ship_length, int print_utf)
{
        int length;
        /* Print line at the beginnning of each row */
        print_utf ? printf("───────┼") : printf("-------+");
        /* Print the line rest of the line */
        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                if (print_utf) {
                        printf("───%s", (length == max_ship_length) ? "┤" : "┼");
                } else {
                        printf("---+");
                }
        }
}

/* Counts how many ships ships of which length have how many hits and saves the values in ship_count */
void countShipHits(const int nx, const int ny, shipstate_t **field, int ship_count[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1]) {
        int i, y;
        /* Make sure the ship_count-array is zero initialized */
        for (i = 0; i <= MAX_SHIP_LENGTH; ++i) {
                int j;
                for (j = 0; j <= MAX_SHIP_LENGTH; ++j) {
                        ship_count[i][j] = 0;
                }
        }
        for (y = 0; y < ny; ++y) {
                int x;
                for (x = 0; x < nx; ++x) {
                        /* Check if the current field is at the top or left end of a ship that way every ship is checked only once */
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
                        /* special case: ship with the length 1 */
                        } else if (is_single(field[y][x])) {
                                /* increment the number of unhit and hit single ships in ship_count[<length>][<number of hits>]
                                        according to whether the single ship is hit */
                                has_been_shot(field[y][x]) ? ++ship_count[1][1] : ++ship_count[1][0];
                        }
                }
        }
}

/* prints the statistics at the end of the game */
void print_stats(play_fields_t *fld, const int n_ships_total[])
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        const int max_ship_length = fld->max_ship_length;
        shipstate_t **field_left = fld->field_left;
        shipstate_t **field_right = fld->field_right;
        const int print_vertical = fld->print_vertical;
        const int print_color = fld->print_color;
        const int print_utf = fld->print_utf;

        const char bot_title[] = "BOT's ships";
        const char player_title[] = "PLAYER's ships";
        /* Total character width of the field of stats */
        const int nCharsX = 4 * max_ship_length + 8;
        int length, n_hits;

        /* Vertical bar either in UTF-8 box-drawing or as ASCII */
        char vert_bar[sizeof("│")];
        strcpy(vert_bar, print_utf ? "│" : "|");

        /* when printing the stats fields above eachother */
        if (print_vertical) {
                /* ship_count[<length>][<n_hits>] corresponds to the number of ships of length <length> that have been hit <n_hits> times */
                int ship_count[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1];
                int players_field;
                for (players_field = 0; players_field <= 1; ++players_field) {
                        if (!players_field) {
                                /* count get array ship_count filled with the stats */
                                countShipHits(nx, ny, field_left, ship_count);
                                print_bold_red(print_color);
                                /* print "BOT's ships" centered */
                                printf("\n%*s%*s\n", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                        } else {
                                /* count get array ship_count filled with the stats */
                                countShipHits(nx, ny, field_right, ship_count);
                                print_bold_green(print_color);
                                /* print "PLAYER's ships" centered */
                                printf("\n%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                        }
                        print_nocolor(print_color);

                        /* top row:   length:|  1|  2|... */
                        print_bold(print_color);
                        printf("length:");
                        print_nocolor(print_color);
                        printf(vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", length, vert_bar);
                        putchar('\n');

                        /* horizontal line: --+---+---+...*/
                        print_stat_hline(max_ship_length, print_utf);
                        putchar('\n');

                        for (n_hits = 0; n_hits <= max_ship_length; ++n_hits) {
                                /* rows: 0 hits|  1|  1|  0... */
                                print_bold(print_color);
                                printf("%2i hit%c",  n_hits, (n_hits == 1) ? ' ' : 's');
                                print_nocolor(print_color);
                                printf(vert_bar);
                                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                                        if (n_hits <= length) {
                                                if (n_hits == length) {
                                                        print_magenta(print_color);
                                                } else if (players_field) {
                                                        print_green(print_color);
                                                } else {
                                                        print_red(print_color);
                                                }
                                                printf("%3i", ship_count[length][n_hits]);
                                                print_nocolor(print_color);
                                                printf(vert_bar);
                                        } else {
                                                printf("   %s", vert_bar);
                                        }
                                        putchar('\n');
                                }
                        }
                        /* horizontal line: --+---+---+...*/
                        print_stat_hline(max_ship_length, print_utf);
                        putchar('\n');

                        /* End line:  sum: |  0|  4|... */
                        print_bold(print_color);
                        printf("  sum: ");
                        print_nocolor(print_color);
                        printf(vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                                if (players_field) {
                                        print_bold_green(print_color);
                                } else {
                                        print_bold_red(print_color);
                                }
                                printf("%3i", n_ships_total[length]);
                                print_nocolor(print_color);
                                printf(vert_bar);
                        }
                        putchar('\n');
                }
        /* when printing the fields beside eachother */
        } else {
                /* ship_count[<length>][<n_hits>] corresponds to the number of ships of length <length> that have been hit <n_hits> times */
                int ship_count_left[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1];
                int ship_count_right[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1];
                /* count get array ship_count filled with the stats */
                countShipHits(nx, ny, field_left, ship_count_left);
                countShipHits(nx, ny, field_right, ship_count_right);

                /* title: e.g.:     BOT's ships     */
                print_bold_red(print_color);
                /* print "BOT's ships" centered */
                printf("\n%*s%*s", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                print_gap();
                print_bold_green(print_color);
                /* print "PLAYER's ships" centered */
                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                print_nocolor(print_color);

                /* top row:   length:|  1|  2|...    length:|  1|  2|... */
                print_bold(print_color);
                printf("length:");
                print_nocolor(print_color);
                printf(vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", length, vert_bar);
                print_gap();
                print_bold(print_color);
                printf("length:");
                print_nocolor(print_color);
                printf(vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", length, vert_bar);
                putchar('\n');

                /* horizontal line: --+---+---+...        --+---+---+... */
                print_stat_hline(max_ship_length, print_utf);
                print_gap();
                print_stat_hline(max_ship_length, print_utf);
                putchar('\n');

                for (n_hits = 0; n_hits <= max_ship_length; ++n_hits) {
                        /* rows: 0 hits|  1|  1|  0...     0 hits|  1|  1|  0... */
                        print_bold(print_color);
                        printf("%2i hit%c",  n_hits, (n_hits == 1) ? ' ' : 's');
                        print_nocolor(print_color);
                        printf(vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                                if (n_hits <= length) {
                                        if (n_hits == length) {
                                                print_magenta(print_color);
                                        } else {
                                                print_red(print_color);
                                        }
                                        printf("%3i", ship_count_left[length][n_hits]);
                                        print_nocolor(print_color);
                                        printf(vert_bar);
                                } else {
                                        printf("   %s", vert_bar);
                                }
                        }
                        print_gap();
                        print_bold(print_color);
                        printf("%2i hit%c",  n_hits, (n_hits == 1) ? ' ' : 's');
                        print_nocolor(print_color);
                        printf(vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                                if (n_hits <= length) {
                                        if (n_hits == length) {
                                                print_magenta(print_color);
                                        } else {
                                                print_green(print_color);
                                        }
                                        printf("%3i", ship_count_right[length][n_hits]);
                                        print_nocolor(print_color);
                                        printf(vert_bar);
                                } else {
                                        printf("   %s", vert_bar);
                                }
                        }
                        putchar('\n');
                }
                /* horizontal line: --+---+---+...        --+---+---+... */
                print_stat_hline(max_ship_length, print_utf);
                print_gap();
                print_stat_hline(max_ship_length, print_utf);
                putchar('\n');

                /* End line:  sum: |  0|  4|...      sum: |  0|  4|... */
                print_bold(print_color);
                printf("  sum: ");
                print_nocolor(print_color);
                printf(vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                        print_bold_red(print_color);
                        printf("%3i", n_ships_total[length]);
                        print_nocolor(print_color);
                        printf(vert_bar);
                }
                print_gap();
                print_bold(print_color);
                printf("  sum: ");
                print_nocolor(print_color);
                printf(vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                        print_bold_green(print_color);
                        printf("%3i", n_ships_total[length]);
                        print_nocolor(print_color);
                        printf(vert_bar);
                }
                printf("\n\n");
        }
}

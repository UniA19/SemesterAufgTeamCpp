/* ---------------------------------*- C -*--------------------------------- *\
 *
\* ------------------------------------------------------------------------- */

#include "global.h"
#include "print.h"
#include "scan.h"
#include "bot.h"

/* allocates one 2D-array for the battleship field */
static shipstate_t ** alloc_field(int nx, int ny)
{
        /* Room for all rows (Array of pointers) */
        shipstate_t **array2d = (shipstate_t**) calloc(ny, sizeof(shipstate_t*));

        /* All data as a single slab */
        shipstate_t *slabs = (shipstate_t*) calloc(nx * ny, sizeof(shipstate_t));

        int i;
        for (i = 0; i < ny; ++i) {
                array2d[i] = slabs;
                slabs += nx;
        }
        return array2d;
}

/* frees the memory used in both the data_left and data_right 2D-array */
static void free_field(play_fields_t *fld)
{
        if (fld->data_left != NULL)  free(fld->data_left[0]);
        if (fld->data_right != NULL)  free(fld->data_right[0]);
        free(fld->data_left);
        free(fld->data_right);
}


int main(int argc, char *argv[])
{
        /* invert… descibes the coordinates, that should be inverted (last targeted) on the bot's field and on the player's field, when using -c (color-mode) */
        int i, auto_choose = FALSE, difficulty = 50;
        status_t status;
        int n_ships_total[MAX_SHIP_LENGTH + 1] = NUM_SHIPS_INIT;
        /* Default with 10 x 10, default NUM_SHIPS_INIT, NULL pointers instead of arrays, which are initialized in alloc_field() */
        play_fields_t field = {10, 10, NUM_SHIPS_INIT, NUM_SHIPS_INIT, MAX_SHIP_LENGTH_INIT, NULL, NULL, FALSE, FALSE, FALSE};
        memcpy(field.n_ships_remaining_left, n_ships_total, (MAX_SHIP_LENGTH + 1) * sizeof(int));
        memcpy(field.n_ships_remaining_right, n_ships_total, (MAX_SHIP_LENGTH + 1) * sizeof(int));

        srand(time(NULL));

        for (i = 1; i < argc; ++i) {
                if ('-' == argv[i][0] && '-' != argv[i][1] ) {
                        const char *opt = &argv[i][1];
                        if (strcmp(opt, "a") == 0) {
                                auto_choose = TRUE;
                        } else if (strcmp(opt, "c") == 0) {
                                field.print_color = TRUE;
                        } else if (strcmp(opt, "u") == 0) {
                                field.print_utf = TRUE;
                        } else if (strncmp(opt, "d=", 2) == 0) {
                                char *endp;
                                long ival;
                                ival = strtol(opt + 2, &endp, 10);
                                if (endp == opt + 2 || *endp || ival < 0 || 100 < ival) {
                                        printf("Invalid input: number in argument: \"-%s\"!\n", opt);
                                        return INPUT_ERROR;
                                }
                                difficulty = (int)ival;
                        } else if (strncmp(opt, "s=", 2) == 0) {
                                char *endp;
                                int shipLen;
                                long ival;
                                const char *p = (opt + 2);

                                /* Reset all ship lengths to 0 */
                                for (shipLen = 0; shipLen <= MAX_SHIP_LENGTH; ++shipLen) {
                                        n_ships_total[shipLen] = field.n_ships_remaining_right[shipLen] = field.n_ships_remaining_left[shipLen] = 0;
                                }

                                /* Ignore optional opening delimiters */
                                if (*p == '{' || *p == '(' || *p == '[') ++p;
                                for (shipLen = MIN_SHIP_LENGTH; *p != '\0' && shipLen <= MAX_SHIP_LENGTH; ++shipLen) {
                                        ival = strtol(p, &endp, 10);
                                        if (endp == p || ival < 0) {
                                                printf("Invalid input: number in argument: \"-%s\"!\n", opt);
                                                return INPUT_ERROR;
                                        }
                                        n_ships_total[shipLen] = field.n_ships_remaining_right[shipLen] = field.n_ships_remaining_left[shipLen] = (int)ival;
                                        p = endp;

                                        if (*p == '}' || *p == ')' || *p == ']') {
                                                ++p;
                                                break;
                                        }
                                        while (isspace(*p) || *p == ',' || *p == ';') ++p;
                                }
                                if (*p != '\0') {
                                        printf("Invalid input format in argument: \"-%s\"!\n", opt);
                                        printf("Perhaps you put in more than %i ship lengths.\n", MAX_SHIP_LENGTH);
                                        return INPUT_ERROR;
                                }
                                field.max_ship_length = shipLen;
                        }  else if (strncmp(opt, "s[", 2) == 0) {
                                char *p;
                                char *endp;
                                long shipLen;
                                long nShips;
                                shipLen = strtol(opt + 2, &p, 10);
                                if (p == opt + 2 || *(p++) != ']' || *(p++) != '=' || shipLen < MIN_SHIP_LENGTH || shipLen > MAX_SHIP_LENGTH) {
                                        printf("Invalid input: first number in argument: \"%s\"!\n", argv[i]);
                                        return INPUT_ERROR;
                                }

                                nShips = strtol(p, &endp, 10);
                                if (endp == p || *endp || nShips < 0) {
                                        printf("Invalid input: second number in argument: \"%s\"!\n", argv[i]);
                                        return INPUT_ERROR;
                                }
                                n_ships_total[shipLen] = field.n_ships_remaining_right[shipLen] = field.n_ships_remaining_left[shipLen] = (int)nShips;

                                if (shipLen > field.max_ship_length) {
                                        field.max_ship_length = shipLen;
                                }
                        } else if (strncmp(opt, "n=", 2) == 0) {
                                char *endp;
                                long ival;
                                ival = strtol(opt + 2, &endp, 10);
                                if (endp == opt + 2 || *endp || ival > min(MAX_WIDTH, MAX_HEIGHT)) {
                                        printf("Invalid input: number in argument: \"%s\"!\n", argv[i]);
                                        return INPUT_ERROR;
                                }
                                field.nx = (int)ival;
                                field.ny = (int)ival;
                        } else if (strncmp(opt, "x=", 2) == 0) {
                                char *endp;
                                long ival;
                                ival = strtol(opt + 2, &endp, 10);
                                if (endp == opt + 2 || *endp || ival > MAX_WIDTH) {
                                        printf("Invalid input: number in argument: \"%s\"!\n", argv[i]);
                                        return INPUT_ERROR;
                                }
                                field.nx = (int)ival;
                        } else if (strncmp(opt, "y=", 2) == 0) {
                                char *endp;
                                long ival;
                                ival = strtol(opt + 2, &endp, 10);
                                if (endp == opt + 2 || *endp || ival > MAX_HEIGHT) {
                                        printf("Invalid input: number in argument: \"%s\"!\n", argv[i]);
                                        return INPUT_ERROR;
                                }
                                field.ny = (int)ival;
                        } else if (strcmp(opt, "v") == 0) {
                                field.print_vertical = TRUE;
                        } else if (opt[0] == 'h') {
                                printf("\nUsage: %s [OPTION]...\n", argv[0]);
                                printf("\nOptions:\n");
                                printf(" -a     player's ships are automatically chosen\n");
                                printf(" -c     print fields using ansi colors (note not every console supports this) \n");
                                printf(" -u     prints fields using UTF-8 characters (note not every console supports this)\n");
                                printf(" -d=<n> sets the difficulty of the bot with <n> being chance of the bot hitting in percent\n");
                                printf(" -s=<array> sets the number of ships <n> of the length corresponding to the position of <n> in <array>\n");
                                printf("    <array> = [ \"{\" | \"[\" | \"(\" ] , [ <n> , { \",\" ,  <n> } ] , [ \"}\" | \"]\" | \")\" ] ;\n");
                                printf(" -s[<length>]=<n> sets the number of ships of the length <length> to <n>\n");
                                printf(" -n=<n> sets the battle-field width and height to <n>\n");
                                printf(" -x=<n> sets the battle-field width to <n>\n");
                                printf(" -y=<n> sets the battle-field height to <n>\n");
                                printf(" -v     prints the fields above eachother\n");
                                printf("(-h)    shows this help\n\n");
                                printf("General Help on using the program:\n");
                                printf("    use '/' to exit the program at any time\n\n");
                                return SUCCESS;
                        } else {
                                printf("Unknown argument: \"%s\"!\n", argv[i]);
                                return INPUT_ERROR;
                        }
                }
        }
        if (field.nx < field.max_ship_length && field.ny < field.max_ship_length) {
                if (field.print_color) printf("\033[31;1m");
                printf("Input Error: your fields (%i x %i) are smaller than the maximum ship length (%i).\n", field.nx, field.ny, field.max_ship_length);
                if (field.print_color) printf("\033[0m");
                return INPUT_ERROR;
        }

        field.data_left = alloc_field(field.nx, field.ny);
        field.data_right = alloc_field(field.nx, field.ny);

        if (field.data_left == NULL || field.data_left == NULL) {
                printf("Error recieved NULL-pointer from allocation - %s line %i\n", __FILE__, __LINE__);
                free_field(&field);
                return ALLOC_ERROR;
        }

        /* bot's automatic ship choice */
        if (auto_choose_ships(field.nx, field.ny, field.data_left, field.n_ships_remaining_left, field.max_ship_length) == INPUT_ERROR) {
                free_field(&field);
                return INPUT_ERROR;
        }
        /* manual or automatic ship choice for the player */
        if (auto_choose) {
                if (auto_choose_ships(field.nx, field.ny, field.data_right, field.n_ships_remaining_right, field.max_ship_length) == INPUT_ERROR) {
                        free_field(&field);
                        return INPUT_ERROR;
                }
        } else {
                status = choose_ships(&field);
                if (status == EXIT) {
                        free_field(&field);
                        return SUCCESS;
                }
                if (status < 0) {
                        printf("ERROR: Should not occur - %s line %i\n", __FILE__, __LINE__);
                        free_field(&field);
                        return status;
                }
        }

        while (1) {
                do {
                        print_field(&field);
                        status = player_shoot(&field);
                        if (status == EXIT) {
                                break;
                        }
                        if (status < 0) {
                                printf("ERROR: Should not occur - %s line %i\n", __FILE__, __LINE__);
                                free_field(&field);
                                return status;
                        }
                        if (has_somemone_won(field.n_ships_remaining_left, field.max_ship_length)) {
                                print_field(&field);
                                if (field.print_color) printf("\033[32;1;5m");
                                if (field.print_utf) {
                                        printf("╭───────────────────╮\n");
                                        printf("│ PLAYER has won!!! │\n");
                                        printf("╰───────────────────╯\n");

                                } else {
                                        printf("/-------------------\\\n");
                                        printf("| PLAYER has won!!! |\n");
                                        printf("\\-------------------/\n");
                                }
                                if (field.print_color) printf("\033[0m");
                                break;
                        }
                } while (status == SUCCESS_HIT);
                if (has_somemone_won(field.n_ships_remaining_left, field.max_ship_length) || status == EXIT) break;

                bot_shoot(&field, difficulty);
                if (has_somemone_won(field.n_ships_remaining_right, field.max_ship_length)) {
                        print_field(&field);
                        if (field.print_color) printf("\033[31;1;5m");
                        if (field.print_utf) {
                                printf("╭────────────────╮\n");
                                printf("│ BOT has won!!! │\n");
                                printf("╰────────────────╯\n");
                        } else if (field.print_color) {
                                printf("/----------------\\\n");
                                printf("| BOT has won!!! |\n");
                                printf("\\----------------/\n");
                        }
                        if (field.print_color) printf("\033[0m");
                        break;
                }
                if (has_somemone_won(field.n_ships_remaining_right, field.max_ship_length)) break;
        }

        print_stats(&field, n_ships_total);

        free_field(&field);
        return SUCCESS;
}

/* ************************************************************************* */

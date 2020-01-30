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
                /* set pointers to the correct parts of the slab */
                array2d[i] = slabs;
                slabs += nx;
        }
        return array2d;
}

/* frees the memory used in both the field_left and field_right 2D-array */
static void free_field(play_fields_t *fld)
{
        if (fld->field_left != NULL)  free(fld->field_left[0]);
        if (fld->field_right != NULL)  free(fld->field_right[0]);
        free(fld->field_left);
        free(fld->field_right);
}


int main(int argc, char *argv[])
{
        /* default settings for various variables */
        int i, auto_choose = FALSE, difficulty = 50;
        status_t status;
        int n_ships_total[MAX_SHIP_LENGTH + 1] = NUM_SHIPS_INIT;
        /* Default field with 10 x 10, default NUM_SHIPS_INIT, NULL pointers instead of arrays, which are initialized in alloc_field() */
        play_fields_t field = {10, 10, NUM_SHIPS_INIT, NUM_SHIPS_INIT, MAX_SHIP_LENGTH_INIT, NULL, NULL, FALSE, FALSE, FALSE};

        srand(time(NULL));

        /* check the arguments */
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
                                printf(" -a     Player's ships are automatically chosen\n");
                                printf(" -c     print fields using ansi colors (note not every console supports this)\n");
                                printf(" -u     prints fields using UTF-8 characters (note not every console supports this)\n");
                                printf(" -d=<n> sets the difficulty of the Bot with <n> being chance of the Bot hitting in percent\n");
                                printf(" -s=<array> sets the number of ships of the length corresponding to the position of <n> in <array> to <n>\n");
                                printf("    <array> = [ \"{\" | \"[\" | \"(\" ] , [ <n> , { \",\" ,  <n> } ] , [ \"}\" | \"]\" | \")\" ] ;\n");
                                printf("    e.g. '-s={3,2,1}' means there are 3 ships of length 1, 2 ships of length 2 and 1 ship of length 3 and no other ships\n");
                                printf("    NOTE: when typing brackets as arguments you may have to put quotes around it to avoid it being interpreted, by Shell\n");
                                printf(" -s[<length>]=<n> sets the number of ships of the length <length> to <n>\n");
                                printf("    e.g. '-s[3]={2}' means there are 2 ships of length 3 and the number of ships of the other lengths stays the same\n");
                                printf("    NOTE: when typing brackets as arguments you may have to put quotes around it to avoid it being interpreted, by Shell\n");
                                printf(" -n=<n> sets the battle-field width and height to <n>\n");
                                printf(" -x=<n> sets the battle-field width to <n>\n");
                                printf(" -y=<n> sets the battle-field height to <n>\n");
                                printf(" -v     prints the fields above eachother\n");
                                printf("(-h)    shows this help\n\n");
                                printf("General Help on using the program:\n");
                                printf("    use '/' to exit the program at any time\n");
                                printf("    use '-h' to get help on how to type the coordinates and which ships are what type at any time\n\n");
                                return SUCCESS;
                        } else {
                                printf("Unknown argument: \"%s\"!\n", argv[i]);
                                return INPUT_ERROR;
                        }
                }
        }

        /* Make sure the ships can actually fit into the field */
        if (field.nx < field.max_ship_length && field.ny < field.max_ship_length) {
                print_bold_red(field.print_color);
                printf("Input Error: your fields (%i x %i) are smaller than the maximum ship length (%i).\n", field.nx, field.ny, field.max_ship_length);
                print_nocolor(field.print_color);
                return INPUT_ERROR;
        }

        /* Allocate memory for the fields */
        field.field_left = alloc_field(field.nx, field.ny);
        field.field_right = alloc_field(field.nx, field.ny);

        /* Check for allocation issues */
        if (field.field_left == NULL || field.field_left == NULL) {
                print_bold_red(field.print_color);
                printf("Error recieved NULL-pointer from allocation - %s line %i\n", __FILE__, __LINE__);
                print_nocolor(field.print_color);
                free_field(&field);
                return ALLOC_ERROR;
        }

        /* Bot's automatic ship choice */
        if (auto_choose_ships(field.nx, field.ny, field.n_ships_remaining_left, field.max_ship_length, field.field_left) == INPUT_ERROR) {
                free_field(&field);
                return INPUT_ERROR;
        }

        /* manual or automatic ship choice for the Player */
        if (auto_choose) {
                if (auto_choose_ships(field.nx, field.ny, field.n_ships_remaining_right, field.max_ship_length, field.field_right) == INPUT_ERROR) {
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
                        free_field(&field);
                        return status;
                }
        }

        /* The game loop - exit on 'quit' or if someone wins */
        while (1) {
                /* Player gets the first shot */
                do {
                        print_field(&field);
                        status = player_shoot(&field);
                        if (status == EXIT) {
                                break;
                        }
                        if (status < 0) {
                                free_field(&field);
                                return status;
                        }
                        if (has_somemone_won(field.n_ships_remaining_left, field.max_ship_length)) {
                                print_field(&field);
                                print_bold_green_blinky(field.print_color);
                                if (field.print_utf) {
                                        printf("╭───────────────────╮\n");
                                        printf("│ PLAYER has won!!! │\n");
                                        printf("╰───────────────────╯\n");

                                } else {
                                        printf("/-------------------\\\n");
                                        printf("| PLAYER has won!!! |\n");
                                        printf("\\-------------------/\n");
                                }
                                print_nocolor(field.print_color);
                                break;
                        }
                /* Player keeps shooting, if has hit */
                } while (status == SUCCESS_HIT);
                /* break out of the loop */
                if (has_somemone_won(field.n_ships_remaining_left, field.max_ship_length) || status == EXIT) break;

                /* Bots turn at shooting */
                bot_shoot(&field, difficulty);
                if (has_somemone_won(field.n_ships_remaining_right, field.max_ship_length)) {
                        print_field(&field);
                        print_bold_red_blinky(field.print_color);
                        if (field.print_utf) {
                                printf("╭────────────────╮\n");
                                printf("│ BOT has won!!! │\n");
                                printf("╰────────────────╯\n");
                        } else if (field.print_color) {
                                printf("/----------------\\\n");
                                printf("| BOT has won!!! |\n");
                                printf("\\----------------/\n");
                        }
                        print_nocolor(field.print_color);
                        break;
                }
        }

        print_stats(&field, n_ships_total);

        free_field(&field);
        return SUCCESS;
}

/* ************************************************************************* */

#include "scan.h"
#include "print.h"
#include "bot.h"

status_t flush_buff()
{
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        if (c == EOF) {
                printf("BUFFER ERROR - %s line %i\n", __FILE__, __LINE__);
                return BUFFER_ERROR;
        }
        return SUCCESS;
}

/* Scans one coordiante from stdin, which consists of x-coordinate in form of letter(s) and the y-coordinate as digit(s) as shown at the edge of the field
(order of coordinates is irrellevant, but without a space inbetween)
e.g.: stdin: "11e" --> *x = 4, *y = 10
 * production:
 * <coordinate>  --> <digit>[<digit>]<letter>[letter]
 * <coordinate>  --> <letter>[letter]<digit>[<digit>]
 * <digit> --> '0'|'1'|...|'9'
 * <letter> --> 'A'|'B'|...|'Z'|'a'|'b'|...|'z'
 */
status_t scan_coordinate(int *x, int *y, int nx, int ny, int print_color)
{
        /* mode: tracks which stage of parsing the function is in:
         * 0 : numbers first: 1st digit
         * 1 : numbers first: 2nd digit
         * 2 : numbers first: 1st letter
         * 3 : numbers first: 2nd letter
         * 4 : letters first: 1st letter
         * 5 : letters first: 2nd letter
         * 6 : letters first: 1st digit
         * 7 : letters first: 2nd digit */
        int mode;
        int x_hold = 0;
        int y_hold = 0;

        char c = toupper(getchar()); /* toupper removes case sensitivity */
        if (c == EOF) return BUFFER_ERROR;

        /* save first character */
        if (isdigit(c)){
                mode = 1;
                y_hold = c - '0';
        } else if (isalpha(c)) {
                mode = 5;
                x_hold = c - 'A' + 1;
        } else if (c == '/') {
                return EXIT;
        } else {
                if (isspace(c)) {
                        print_bold_red(print_color);
                        printf("Input Error: Missing coordinate.\n");
                        print_nocolor(print_color);
                } else if (c == '-' && tolower((c = getchar())) == 'h') {
                        /* Geneneral instructions on how to type the coordiantes */
                        print_bold_green(print_color);
                        printf("\nInstructions on typing the coordinate:\n");
                        print_bold_blue(print_color);
                        printf("Type the x-coordinate as letter(s) and the y-coordinate as digit(s) as shown at the edge of the field (order of coordinates is irrellevant, but without a space inbetween):\n");
                        print_nocolor(print_color);
                        print_bold(print_color);
                        printf(" <coordinate> = ( <letter> [<letter>] <firstDigit> [<digit>] ) | ( <firstDigit> [<digit>] <letter> [<letter>] )\n");
                        printf(" <letter> = 'A' | 'B' | ... | 'Z' | 'a' | 'b' | ... | 'z'\n");
                        printf(" <firstDigit> = '1' | '2' | ... | '9'\n");
                        printf(" <digit> = '0' | <firstDigit>\n\n");
                        print_bold_green(print_color);
                        printf("Instructions on typing the coordinate and direction/length for placing the ships:\n");
                        print_bold_blue(print_color);
                        printf("First type the coordinate as when only typing a coordinate, then type a space and then the direction/length.\n");
                        printf("The direction/length consists of a number representing the ship's length and the other character representing the direction in which it should be placed from the coordinate:\n");
                        print_nocolor(print_color);
                        printf("Note: if you do not type direction/length or only type the direction/length as '1' your ship has length 1 and is oriented east- / rightwards.\n");
                        print_bold(print_color);
                        printf(" <directLength> =  '1' | <direction> <length> | <length> <direction>\n");
                        printf(" <length> = <firstDigit> [<digit>]\n");
                        printf(" <direction> = 'R' | 'r' | 'E' | 'e' | '>'       for signaling the direction east- / rightwards.\n");
                        printf("               'L' | 'l' | 'W' | 'w' | '<'       for signaling the direction west- / leftwards.\n");
                        printf("               'U' | 'u' | 'E' | 'n' | '^'       for signaling the direction north- / upwards.\n");
                        printf("               'D' | 'd' | 'S' | 's' | 'V' | 'v' for signaling the direction south- / downwards.\n\n");
                        print_bold_cyan(print_color);
                        printf("Type '/' to exit.\n\n");
                        print_nocolor(print_color);
                } else {
                        print_bold_red(print_color);
                        printf("Input Error: Unknown coordinate character: '%c' (coordinates must only consist of letters and digits).\n", c);
                        print_nocolor(print_color);
                }
                if (c != '\n') {
                        flush_buff();
                }
                return INPUT_ERROR;
        }
        /* Iterate through the other characters */
        while (!isspace(c = toupper(getchar())) && c != EOF) {
                if (isdigit(c)) {
                        /* skip missing second letter */
                        if (mode == 5) ++mode;

                        switch (mode) {
                                case 1: case 7:
                                        /* shift old value by a digit, before adding the new value */
                                        y_hold *= 10;
                                        y_hold += c - '0';
                                        ++mode;
                                        break;
                                case 6:
                                        y_hold += c - '0';
                                        ++mode;
                                        break;
                                default:
                                        print_bold_red(print_color);
                                        printf("Input Error: character: '%c' is not supposed to be a digit.\n", c);
                                        print_nocolor(print_color);
                                        flush_buff();
                                        return INPUT_ERROR;
                        }
                } else if (isalpha(c)) {
                        /* skips missing second digit */
                        if (mode == 1) ++mode;

                        switch (mode) {
                                case 2:
                                        x_hold += c - 'A' + 1;
                                        ++mode;
                                        break;
                                case 3: case 5:
                                        /* shift old value by a letter, before adding the new value */
                                        x_hold *= 26;
                                        x_hold += c - 'A' + 1;
                                        ++mode;
                                        break;
                                default:
                                        print_bold_red(print_color);
                                        printf("Input Error: character: '%c' is not supposed to be a letter.\n", c);
                                        print_nocolor(print_color);
                                        flush_buff();
                                        return INPUT_ERROR;
                        }
                } else {
                        print_bold_red(print_color);
                        printf("Input Error: Unknown coordinate character: '%c' (coordinates must only be made of letters and digits).\n", c);
                        print_nocolor(print_color);
                        flush_buff();
                        return INPUT_ERROR;
                }
        }
        if (c == EOF) return BUFFER_ERROR;

        /* makes sure x_hold, y_hold are within the field */
        if (0 >= x_hold || x_hold > nx || 0 >= y_hold || y_hold > ny) {
                print_bold_red(print_color);
                if (0 >= x_hold) {
                        printf("Input Error: Your x-coordinate (letters) is too small to fit on the field or your x-coordinate is missing\n");
                } else if (x_hold > nx) {
                        printf("Input Error: Your x-coordinate (letters) is too large to fit on the field\n");
                } else if (0 >= y_hold) {
                        printf("Input Error: Your y-coordinate (digits) is too small to fit on the field or your y-coordinate is missing\n");
                } else {
                        printf("Input Error: Your y-coordinate (digits) is too large to fit on the field\n");
                }
                print_nocolor(print_color);
                if (c != '\n') {
                        flush_buff();
                }
                return INPUT_ERROR;
        }
        /* Save x_hold, y_hold in x, y, while subtracting 1, due to the table starting at 1 instead of 0 */
        *x = x_hold - 1;
        *y = y_hold - 1;
        /* return extra information: whether the end of the line has been reached or not */
        return (c == '\n') ? SUCCESS_ENDL : SUCCESS;
}

/* Scans the direction/length consisting of a number representing the ship's length and the other character representing the direction in which it should be placed
returns direction, passes on the length through the pointer
       or returns negative number in case of error
production:
    <directLength> =  '1' | <direction> <length> | <length> <direction>
    <length> = <firstDigit> [<digit>]
    <direction> = 'R' | 'r' | 'E' | 'e' | '>'       for signaling the direction east- / rightwards
                  'L' | 'l' | 'W' | 'w' | '<'       for signaling the direction west- / leftwards
                  'U' | 'u' | 'E' | 'n' | '^'       for signaling the direction north- / upwards
                  'D' | 'd' | 'S' | 's' | 'V' | 'v' for signaling the direction south- / downwards

   NOTE: this function flushes the line even on a correct input */
direction_t scan_direction(int *length, int max_ship_length, int print_color)
{
        int direction = -1;
        int length_hold = 0;

        char c = toupper(getchar()); /* toupper removes case sensitivity */
        if (c == EOF) return BUFFER_ERROR_DIRECTION;

        /* starting with the digit */
        if (isdigit(c)) {
                length_hold = c - '0';
                /* check if the next character is also a digit, in case of a double digit number */
                c = toupper(getchar());
                if (c == EOF) {
                        return BUFFER_ERROR_DIRECTION;
                } else if (isdigit(c)) {
                        /* factor in the second digit */
                        length_hold *= 10;
                        length_hold += c - '0';
                        c = toupper(getchar());
                }
                /* check if the length scanned in is within the range of ship lengths */
                if (length_hold < MIN_SHIP_LENGTH || max_ship_length < length_hold) {
                        print_bold_red(print_color);
                        if (length_hold < MIN_SHIP_LENGTH) {
                                printf("Input Error: Your ship length '%i' is too small\n", length_hold);
                        } else {
                                printf("Input Error: Your ship length '%i' is too large\n", length_hold);
                        }
                        print_nocolor(print_color);
                        if (c != '\n') {
                                flush_buff();
                        }
                        return INPUT_ERROR_DIRECTION;
                }
        }
        /* incase of length 1 the direction is irrelevant */
        if (length_hold == 1) {
                *length = length_hold;
                if (c != '\n') flush_buff();
                return RIGHT;
        }
        /* Scan in the direction */
        switch (c) {
                case 'R': case 'E': case '>':
                        direction = RIGHT;
                        break;
                case 'D': case 'S': case 'V':
                        direction = DOWN;
                        break;
                case 'L': case 'W': case '<':
                        direction = LEFT;
                        break;
                case 'U': case 'N': case '^':
                        direction = UP;
                        break;
                default:
                        print_bold_red(print_color);
                        if (c == '\n') {
                                printf("Input Error: Missing direction.\n");
                        } else {
                                printf("Input Error: Invalid direction: '%c'.\n", c);
                        }
                        print_nocolor(print_color);
                        print_bold(print_color);
                        printf("Use: 'R' | 'r' | 'E' | 'e' | '>'       for signaling the direction east- / rightwards.\n");
                        printf("     'L' | 'l' | 'W' | 'w' | '<'       for signaling the direction west- / leftwards.\n");
                        printf("     'U' | 'u' | 'E' | 'n' | '^'       for signaling the direction north- / upwards.\n");
                        printf("     'D' | 'd' | 'S' | 's' | 'V' | 'v' for signaling the direction south- / downwards.\n");
                        print_nocolor(print_color);
                        if (c != '\n') flush_buff();
                        return INPUT_ERROR_DIRECTION;
        }
        /* if the length was not scanned in the first time */
        if (!length_hold && isdigit(c = toupper(getchar()))) {
                length_hold = c - '0';
                /* check if the next character is also a digit, in case of a double digit number */
                c = toupper(getchar());
                if (isdigit(c)) {
                        /* factor in the second digit */
                        length_hold *= 10;
                        length_hold += c - '0';
                }
                /* check if the length scanned in is within the range of ship lengths */
                if (length_hold < MIN_SHIP_LENGTH || max_ship_length < length_hold) {
                        print_bold_red(print_color);
                        if (length_hold < MIN_SHIP_LENGTH) {
                                printf("Input Error: Your ship length '%i' is too small\n", length_hold);
                        } else {
                                printf("Input Error: Your ship length '%i' is too large\n", length_hold);
                        }
                        print_nocolor(print_color);
                        if (c != '\n') flush_buff();
                        return INPUT_ERROR_DIRECTION;
                }
        }

        /* check if the direction and have actually been scanned */
        if (direction == -1 || !length_hold) {
                print_bold_red(print_color);
                if (direction == -1) {
                        printf("Input Error: The direction of your ship is missing\n");
                } else {
                        printf("Input Error: Your ship length is too large / too small / non existent\n");
                }
                print_nocolor(print_color);
                if (c != '\n') flush_buff();
                return INPUT_ERROR_DIRECTION;
        }
        /* copy the length into the pointer */
        *length = length_hold;

        if (c != '\n') flush_buff();
        return direction;
}

/* Handles the Player's personal choice of ships */
status_t choose_ships(play_fields_t *fld)
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        const int print_color = fld->print_color;
        shipstate_t **data_right = fld->data_right;
        const int max_ship_length = fld->max_ship_length;
        /* n_ships_remaining[i] is the number of i long ships left */
        int n_ships_remaining[MAX_SHIP_LENGTH + 1];
        /* if the last user-input had an INPUT-ERROR the field should not be reprinted */
        int printField = TRUE;
        /* using "nRemainingShips" as scratch space */
        memcpy(n_ships_remaining, fld->n_ships_remaining_right, sizeof(n_ships_remaining));

        /* loop-condition checks whether there is still ships, that have not been set */
        while (1) {
                int x, y, i, length;
                status_t status;
                direction_t direction;
                /* variable containing the whether there are any ships in the way */
                int is_free = TRUE;

                {
                        /* scan to see if any ships are still remaining (not sunk) */
                        int anyShips = FALSE;
                        for (i = MIN_SHIP_LENGTH; i <= max_ship_length && !anyShips; ++i) {
                                anyShips = n_ships_remaining[i];
                        }
                        if (!anyShips) {
                                break;
                        }
                }
                if (printField) {
                        print_field(fld);
                        /* Information on how many ships the player still has to set */
                        print_bold(print_color);
                        printf("You have ");
                        for (i = max_ship_length; i >= MIN_SHIP_LENGTH; --i) {
                                if (n_ships_remaining[i]) {
                                        printf("%i battle ship%s (length %i) ", n_ships_remaining[i], n_ships_remaining[i] == 1 ? "" : "s", i);
                                }
                        }
                        printf("remaining\n");
                        print_nocolor(print_color);
                } else {
                        printField = TRUE;
                }
                /* Explaning to the player on how to place the ships */
                print_bold(print_color);
                printf("Choose where to place your ships, by typing the start coordinate and the direction, length of your ship.\n");
                if (print_color) printf("\033[0m  e.g. '\033[31mA\033[32m1 \033[33m2\033[34ms\033[0m' a ship at coordinate (\033[31mA\033[0m, \033[32m1\033[0m) with length \033[33m2\033[0m and facing down- / \033[34ms\033[0mouthwards (for further information type '\033[35m-h\033[0m')\n");
                else printf("  e.g. 'A1 2s' a ship at coordinate (A, 1) with length 2 and facing down- / southwards (for further information type '-h')\n");

                /* scan coordinate and direction, where ship should be placed */
                status = scan_coordinate(&x, &y, nx, ny, print_color);
                switch (status) {
                        /* If the user did not add length/direction he must mean length */
                        case SUCCESS_ENDL:
                                direction = RIGHT;
                                length = 1;
                                break;
                        case SUCCESS:
                                /* scan the direction/length of the ship */
                                direction = scan_direction(&length, max_ship_length, print_color);
                                if (direction == BUFFER_ERROR_DIRECTION) {
                                        print_bold_red_blinky(print_color);
                                        printf("BUFFER ERROR - %s line %i\n", __FILE__, __LINE__);
                                        print_nocolor(print_color);
                                        return BUFFER_ERROR;
                                } else if (direction == INPUT_ERROR_DIRECTION) {
                                        printField = FALSE;
                                        continue;
                                }
                                break;
                        case EXIT:
                                /* Stop the program incase the player typed '/' */
                                return EXIT;
                        case BUFFER_ERROR:
                                print_bold_red_blinky(print_color);
                                printf("BUFFER ERROR - %s line %i\n", __FILE__, __LINE__);
                                print_nocolor(print_color);
                                return BUFFER_ERROR;
                        case INPUT_ERROR: default:
                                printField = FALSE;
                                continue;
                }

                /* check if there are ships of the selected length left */
                if (length > max_ship_length || n_ships_remaining[length] <= 0) {
                        print_bold_red(print_color);
                        printf("You don't have any ship with the length %i left to place.\n", length);
                        print_nocolor(print_color);
                        printField = FALSE;
                        continue;
                }

                /* check the boxes where the ship should be set and its surrounding box for other ships */
                for (i = 0; i < length; ++i) {
                        int x_h = x;
                        int y_h = y;
                        int j;
                        /* in-/decrease one coordinate by i in the direction */
                        switch (direction) {
                                case RIGHT:
                                        x_h = x + i;
                                        break;
                                case DOWN:
                                        y_h = y + i;
                                        break;
                                case LEFT:
                                        x_h = x - i;
                                        break;
                                case UP:
                                        y_h = y - i;
                                        break;
                                default:
                                        print_bold_red_blinky(print_color);
                                        printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                                        print_nocolor(print_color);
                                        return UNKNOWN_ERROR;
                        }
                        /* check if the ship is in the field */
                        if (0 > y_h || y_h >= ny || 0 > x_h || x_h >= nx) {
                                is_free = FALSE;
                        }
                        /* check whether the points in a 3x3 box around each box, that should be filled, are free */
                        for (j = -1; j <= 1 && is_free; ++j) {
                                int k;
                                for (k = -1; k <= 1 && is_free; ++k) {
                                        /* first check if the point is within the field (0 <= point < size) and then whether the the point is occupied */
                                        if (0 <= (y_h + k) && (y_h + k) < ny && 0 <= (x_h + j) && (x_h + j) < nx && is_unhit(data_right[y_h + k][x_h + j])) {
                                                is_free = FALSE;
                                        }
                                }
                        }
                }
                /* Let the user know he placed the ship at a wrong place */
                if (!is_free) {
                        print_bold_red(print_color);
                        printf("You placed your ship, where it would collide with a wall or touch another ship\n");
                        print_nocolor(print_color);
                        printField = FALSE;
                        continue;
                }

                /* Actual setting of the ship */
                if (length == 1) {
                        data_right[y][x] = UNHIT_SINGLE;
                } else {
                        /* place the ship onto the the boxes, by changing their state to UNHIT */
                        for (i = 0; i < length; ++i) {
                                /* in-/decrease one coordinate by i based on the direction */
                                switch (direction) {
                                        case RIGHT:
                                                data_right[y][x + i] = ((i == 0) ? UNHIT_HORIZ_LEFT : ((i == length - 1) ? UNHIT_HORIZ_RIGHT : UNHIT_HORIZ));
                                                break;
                                        case DOWN:
                                                data_right[y + i][x] = ((i == 0) ? UNHIT_VERT_TOP : ((i == length - 1) ? UNHIT_VERT_BOTTOM : UNHIT_VERT));
                                                break;
                                        case LEFT:
                                                data_right[y][x - i] = ((i == 0) ? UNHIT_HORIZ_RIGHT : ((i == length - 1) ? UNHIT_HORIZ_LEFT : UNHIT_HORIZ));
                                                break;
                                        case UP:
                                                data_right[y - i][x] = ((i == 0) ? UNHIT_VERT_BOTTOM : ((i == length - 1) ? UNHIT_VERT_TOP : UNHIT_VERT));
                                                break;
                                        default:
                                                print_bold_red_blinky(print_color);
                                                printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                                                print_nocolor(print_color);
                                                return UNKNOWN_ERROR;
                                }
                        }
                }
                /* decrement the number of length-long ships remaining */
                --n_ships_remaining[length];
        }
        return SUCCESS;
}

/* Handles the Player's shot on the Bot's ships */
status_t player_shoot(play_fields_t *fld)
{
        /* Copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        const int print_color = fld->print_color;
        shipstate_t **data_left = fld->data_left;
        int *n_ships_remaining_left = fld->n_ships_remaining_left;

        int x, y;
        point_t point;
        status_t status;

        print_bold(print_color);
        printf("Type the coordinates, where you want to shoot at (for further information type '-h').\n");
        print_nocolor(print_color);
        /* scanning coordinates, that the player wants to shoot at */
        while ((status = scan_coordinate(&x, &y, nx, ny, print_color)) == INPUT_ERROR || (status >= SUCCESS && has_been_shot(data_left[y][x])))  {
                if (status == SUCCESS) {
                        flush_buff();
                }
                if (status >= SUCCESS && has_been_shot(data_left[y][x])) {
                        print_bold_red(print_color);
                        printf("Don't waste your shots: this box has already been shot at by you.\n");
                        print_nocolor(print_color);
                }
                print_bold(print_color);
                printf("Retype the coordinates, where you want to shoot at (for further information type '-h').\n");
                print_nocolor(print_color);
        }
        if (status == BUFFER_ERROR) {
                print_bold_red_blinky(print_color);
                printf("BUFFER ERROR - %s line %i\n", __FILE__, __LINE__);
                print_nocolor(print_color);
                return BUFFER_ERROR;
        }
        /* Halt the program if the user types '/' */
        if (status == EXIT) return EXIT;

        point.x = x;
        point.y = y;
        /* Actual changing of the box state to represent shooting */
        if (data_left[y][x] == NONE) {
                data_left[y][x] = SPLASH_INVERT;
                return SUCCESS;
        } else if (is_unhit(data_left[y][x])) {
                data_left[y][x] += (HIT_HORIZ - UNHIT_HORIZ);
                data_left[y][x] = invert(data_left[y][x]);
                test_ship_status(nx, ny, n_ships_remaining_left, data_left, point);
                return SUCCESS_HIT;
        } else {
                print_bold_red_blinky(print_color);
                printf("ERROR: Should not occur - %s line %i\n", __FILE__, __LINE__);
                print_nocolor(print_color);
                return UNKNOWN_ERROR;
        }
        return SUCCESS;
}

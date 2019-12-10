#include "input.h"

/* e.g.: stdin: "11e" --> *x = 4, *y = 10
 * production:
 * <coordinate>  --> <digit>[<digit>]<letter>[letter]
 * <coordinate>  --> <letter>[letter]<digit>[<digit>]
 * <digit> --> '0'|'1'|...|'9'
 * <letter> --> 'A'|'B'|...|'Z'|'a'|'b'|...|'z'
 */
int scan_coordinate(int *x, int *y, int nx, int ny)
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
        char c = toupper(getchar()); /* toupper removes case sensitivity */
        int x_hold = 0;
        int y_hold = 0;

        if (c == EOF) {
                return BUFFER_ERROR;
        }

        /* save first digit */
        if (isdigit(c)){
                mode = 1;
                y_hold = c - '0';
        } else if (isalpha(c)) {
                mode = 5;
                x_hold = c - 'A' + 1;
        } else {
                return INPUT_ERROR;
        }
        while (!isspace(c = toupper(getchar())) && c != EOF) {
                if (isdigit(c)) {
                        /* skip missing second digit */
                        if (mode == 5) {
                                ++mode;
                        }
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
                                        return INPUT_ERROR;
                        }
                } else if (isalpha(c)) {
                        /* skips missing second letter */
                        if (mode == 1) {
                                ++mode;
                        }
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
                                        return INPUT_ERROR;
                        }
                } else {
                        return INPUT_ERROR;
                }
        }

        if (c == EOF) {
                return BUFFER_ERROR;
        }

        /* makes sure x_hold, y_hold are within the field */
        if (0 >= x_hold || x_hold > nx || 0 >= y_hold || y_hold > ny) {
                return INPUT_ERROR;
        }
        *x = x_hold - 1;
        *y = y_hold - 1;
        return SUCCESS;
}

/* returns direction passes length on through the pointer or returns negative number in case of error */
direction_t scan_direction(int *length)
{
        int direction = -1;
        int length_hold = -1;
        int i;
        char c;

        for (i = 0; i < 2; ++i) {
                c = toupper(getchar()); /* toupper removes case sensitivity */
                if (c == EOF) {
                        return BUFFER_ERROR;
                }

                if (isdigit(c)) {
                        length_hold = c - '0';
                        if (length_hold < MIN_SHIP_LENGTH || MAX_SHIP_LENGTH < length_hold) {
                                return INPUT_ERROR;
                        }
                } else {
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
                                        return INPUT_ERROR;
                        }
                }
        }

        if (direction == -1 || length_hold == -1) {
                return INPUT_ERROR;
        }
        *length = length_hold;

        return direction;
}
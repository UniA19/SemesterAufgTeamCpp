/* ---------------------------------*- C -*--------------------------------- *\
 *
\* ------------------------------------------------------------------------- */

/* #include <assert.h> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_WIDTH 99
#define MAX_HEIGHT MAX_WIDTH

#define SUCCESS_ENDL 1
#define SUCCESS 0
#define INPUT_ERROR -1
#define BUFFER_ERROR -2
#define EXIT -3
#define UNKNOWN_ERROR -4

#define TRUE 1
#define FALSE 0

/* number of ships (value) of each length (postion in array) */
#define NUM_SHIPS_INIT {0, 0, 4, 3, 2, 1}
#define MIN_SHIP_LENGTH 2
#define MAX_SHIP_LENGTH 5

/* The gap between left and right fields */
#define print_gap() (printf("        "))
#define is_unhit(box) ((box) == UNHIT_HORIZ || (box) == UNHIT_VERT)
#define is_hit(box) ((box) == HIT_HORIZ || (box) == HIT_VERT)
#define is_sunk(box) ((box) == SUNK_HORIZ || (box) == SUNK_VERT)
#define is_ship(box) (is_unhit(box) || is_hit(box) || is_sunk(box))
#define has_been_shot(box) ((box) == SPLASH || is_hit(box) || is_sunk(box))
#define is_horiz(box) ((box) == UNHIT_HORIZ || (box) == HIT_HORIZ || (box) == SUNK_HORIZ)
#define is_vert(box) ((box) == UNHIT_VERT || (box) == HIT_VERT || (box) == SUNK_VERT)

#define min(a, b) ((a) < (b) ? (a) : (b))

/* debug constants */
#define SHOW_BOTS_SHIP


static const char *const rowFormats[] =
{
        "   |",         /* 0: unhit - water */
        "~~~|",         /* 1: hit water */
        "===|",         /* 2: horizontal, unhit ship on the right field*/
        "III|",         /* 3: vertical, unhit ship on the right field*/
        "hhh|",         /* 4: horizontal, hit ship */
        "vvv|",         /* 5: vertical, hit ship */
        "---|",         /* 6: horizontal, sunken ship */
        "||||"          /* 7: vertical, sunken ship */
};

typedef enum {
        NONE = 0,       /* 0: unhit - water */
        SPLASH,         /* 1: hit water */
        UNHIT_HORIZ,    /* 2: horizontal, unhit ship on the right field*/
        UNHIT_VERT,     /* 3: vertical, unhit ship on the right field*/
        HIT_HORIZ,      /* 4: horizontal, hit ship */
        HIT_VERT,       /* 5: vertical, hit ship */
        SUNK_HORIZ,     /* 6: horizontal, sunken ship */
        SUNK_VERT       /* 7: vertical, sunken ship */
} shipstate_t;

typedef enum {
        BUFFER_ERROR_DIRECTION = BUFFER_ERROR,
        INPUT_ERROR_DIRECTION = INPUT_ERROR,
        RIGHT = 0,
        DOWN,
        LEFT,
        UP
} direction_t;

typedef struct {
        int nx;
        int ny;
        /* num_of_ship_left[i] is the number of i long ships left */
        int nShipsRemaining_left[MAX_SHIP_LENGTH + 1];
        int nShipsRemaining_right[MAX_SHIP_LENGTH + 1];
        /* 0 = unhit water, 1 = hit water, 2 = unhit ship, 3 = hit ship, 4 = sunken ship*/
        shipstate_t **data_left;
        shipstate_t **data_right;
} play_fields_t;


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
        free(fld->data_left[0]);
        free(fld->data_right[0]);
        free(fld->data_left);
        free(fld->data_right);
}

/* prints horizontal line, deviding the rows (--+---+---+...) */
void print_hline(int nx)
{
        int coli;

        printf("--+");
        for (coli = 0; coli < nx; ++coli) {
                printf("---+");
        }
}

/* prints top row (  | A | B | C | ... ) */
void print_top_row(int nx)
{
        int coli;

        printf("  |");
        /* format as "AA" etc if there are more than 26 cols */
        for (coli = 0; coli < nx; ++coli) {
                printf("%c%c |", (coli < 26 ? ' ' : (coli / 26 - 1) + 'A'), ((coli % 26) + 'A'));
        }
}

/* prints row including the numbering at the beginning of the line ( 1|~~~|XXX|~~~|...) */
void print_row(const shipstate_t row[], int nx, int row_num, int is_left_field)
{
        int i;

        printf("%2i|", row_num);

        for (i = 0; i < nx; ++i) {
                int state = row[i];
                if (is_left_field && is_unhit(state)) {
#ifndef SHOW_BOTS_SHIP
                       state = NONE; /* keep hidden */
#endif
                }
                printf(rowFormats[state]);
        }
}

/* prints both entire battleship-fields */
void print_field(play_fields_t *fld, int print_vertical)
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_left = fld->data_left;
        shipstate_t **data_right = fld->data_right;
        const char bot_title[] = "BOT's ships";
        const char player_title[] = "PLAYER's ships";
        const int nCharsX = 4 * nx + 3;

        if (print_vertical) {
                int row;
                int players_field;
                for (players_field = 0; players_field <= 1; ++players_field) {
                        /* title: e.g.:     BOT's ships     */
                        if (!players_field) {
                                printf("\n%*s%*s\n", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                        } else {
                                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                        }
                        /* top row:   | A | B |...*/
                        print_top_row(nx);
                        printf("\n");
                        for (row = 0; row < ny; ++row) {
                                /* horizontal line: --+---+---+...*/
                                print_hline(nx);
                                printf("\n");
                                /* other rows:   <row>|~~~|XXX|...*/
                                if (!players_field) {
                                print_row(data_left[row], nx, row + 1, 0);
                                } else {
                                        print_row(data_right[row], nx, row + 1, 0);
                                }
                                printf("\n");
                        }
                        /* horizontal line: --+---+---+...*/
                        print_hline(nx);
                        printf("\n\n");
                }
        } else {
                int row;
                /* title: e.g.:     BOT's ships     */
                printf("\n%*s%*s", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                print_gap();
                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                /* top row:   | A | B |...          | A | B |... */
                print_top_row(nx);
                print_gap();
                print_top_row(nx);
                printf("\n");
                for (row = 0; row < ny; ++row) {
                        /* horizontal line: --+---+---+...        --+---+---+... */
                        print_hline(nx);
                        print_gap();
                        print_hline(nx);
                        printf("\n");
                        /* other rows:   <row>|~~~|XXX|...          |   |XXX|... */
                        print_row(data_left[row], nx, row + 1, 0);
                        print_gap();
                        print_row(data_right[row], nx, row + 1, 0);
                        printf("\n");
                }
                /* horizontal line: --+---+---+...        --+---+---+... */
                print_hline(nx);
                print_gap();
                print_hline(nx);
                printf("\n\n");
        }
}

int flush_buff()
{
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        return (c == EOF) ? BUFFER_ERROR : SUCCESS;
}

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
        } else if (c == '/') {
                return EXIT;
        } else {
                if (c != '\n') {
                        flush_buff();
                }
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
                                        flush_buff();
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
                                        flush_buff();
                                        return INPUT_ERROR;
                        }
                } else {
                        flush_buff();
                        return INPUT_ERROR;
                }
        }

        if (c == EOF) {
                return BUFFER_ERROR;
        }

        /* makes sure x_hold, y_hold are within the field */
        if (0 >= x_hold || x_hold > nx || 0 >= y_hold || y_hold > ny) {
                if (c != '\n') {
                        flush_buff();
                }
                return INPUT_ERROR;
        }
        *x = x_hold - 1;
        *y = y_hold - 1;
        return (c == '\n') ? SUCCESS_ENDL : SUCCESS;
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
                                if (c != '\n') {
                                        flush_buff();
                                }
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
                                case '\n':
                                        return INPUT_ERROR;
                                default:
                                        flush_buff();
                                        return INPUT_ERROR;
                        }
                }
        }

        if (direction == -1 || length_hold == -1) {
                flush_buff();
                return INPUT_ERROR;
        }
        *length = length_hold;

        return direction;
}

int choose_ships(play_fields_t *fld, int print_vertical)
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_right = fld->data_right;
        /* nShipsRemaining[i] is the number of i long ships left */
        int nShipsRemaining[MAX_SHIP_LENGTH + 1];
        /* if the last user-input had an INPUT-ERROR the field should not be reprinted */
        int printField = TRUE;
        /* using "nRemainingShips" as scratch space */
        memcpy(nShipsRemaining, fld->nShipsRemaining_right, sizeof(nShipsRemaining));

        /* loop-condition checks whether there is still ships, that have not been set */
        while (1) {
                int i, x, y, status, length;
                direction_t direction;
                /* variable containing the whether there are any ships in the way */
                int is_free = TRUE;

                {
                        /* scan to see if any ships are still remaining (not sunk) */
                        int anyShips = FALSE;
                        for (i = MIN_SHIP_LENGTH; i <= MAX_SHIP_LENGTH && !anyShips; ++i) {
                                anyShips = nShipsRemaining[i];
                        }
                        if (!anyShips) {
                                break;
                        }
                }
                if (printField) {
                        print_field(fld, print_vertical);
                        printf("You have ");
                        for (i = MAX_SHIP_LENGTH; i >= MIN_SHIP_LENGTH; --i) {
                                if (nShipsRemaining[i]) {
                                        printf("%i battle ship%s (length %i) ", nShipsRemaining[i], nShipsRemaining[i] == 1 ? "" : "s", i);
                                }
                        }
                        printf("remaining\n");
                } else {
                        printField = TRUE;
                }
                printf("Choose where to place your ships, by typing the start coordinate and the direction, length of your ship.\n");
                printf("  e.g. 'A1 2s' a ship at coordinate a1 with length 2 and facing southwards\n");
                /* scan coordinate and direction, where ship should be placed */
                if ((status = scan_coordinate(&x, &y, nx, ny)) < 0 || (direction = scan_direction(&length)) < 0) {
                        if (status == BUFFER_ERROR || direction == BUFFER_ERROR) {
                                printf("BUFFER ERROR - %s line %i\n", __FILE__, __LINE__);
                                return BUFFER_ERROR;
                        }
                        if (status == EXIT) {
                                return EXIT;
                        }
                        printField = FALSE;
                        continue;
                }
                flush_buff();

                /* check if there are ships of the selected length left */
                if (length > MAX_SHIP_LENGTH || nShipsRemaining[length] <= 0) {
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
                                        printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
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
                if (!is_free) {
                        printField = FALSE;
                        continue;
                }

                /* place the ship onto the the boxes, by changing their state to UNHIT */
                for (i = 0; i < length; ++i) {
                        /* in-/decrease one coordinate by i based on the direction */
                        switch (direction) {
                                case RIGHT:
                                        data_right[y][x + i] = UNHIT_HORIZ;
                                        break;
                                case DOWN:
                                        data_right[y + i][x] = UNHIT_VERT;
                                        break;
                                case LEFT:
                                        data_right[y][x - i] = UNHIT_HORIZ;
                                        break;
                                case UP:
                                        data_right[y - i][x] = UNHIT_VERT;
                                        break;
                                default:
                                        printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                                        return UNKNOWN_ERROR;
                        }
                }
                /* decrement the number of length-long ships remaining */
                --nShipsRemaining[length];
        }
        return SUCCESS;
}

void auto_choose_ships(const int nx, const int ny, shipstate_t **data, int *nShipsRemaining)
{

        int length;
        /* number of times the field was reset, because of ship-jamming */
        int numOfReTries = 0;
        /* nShipsRemaining_cpy[i] is the number of i long ships left */
        int nShipsRemaining_cpy[MAX_SHIP_LENGTH + 1];
        /* using "nRemainingShips" as scratch space */
        memcpy(nShipsRemaining_cpy, nShipsRemaining, sizeof(nShipsRemaining_cpy));

        /* populate with ships - place longest ships first, while there still is enough room */
        for (length = MAX_SHIP_LENGTH; length >= MIN_SHIP_LENGTH; --length) {
                int numOfTries = 0;
                while (nShipsRemaining_cpy[length]) {
                        int i;
                        /* ships can only be set RIGHT (= 0) or DOWN (= 1) */
                        const direction_t direction = rand() % 2;
                        /* change the possible coordinates, so that the ship fully on the field */
                        int x = (direction == DOWN) ? rand() % nx : rand() % (nx - (length - 1));
                        int y = (direction == DOWN) ? rand() % (ny - (length - 1)) : rand() % ny;
                        /* variable containing the whether there are any ships in the way */
                        int is_free = TRUE;
                        ++numOfTries;
                        if (numOfTries > 1000) {
                                if (numOfReTries > 1000) {
                                        printf("\nNo legal postions for the bot's ships found.\n");
                                        printf("Increase the value of x or y coordinate, so that there is enough room for the ships.\n\n");
                                        exit(INPUT_ERROR);
                                }
                                /* reset variables */
                                length = MAX_SHIP_LENGTH + 1;
                                memcpy(nShipsRemaining_cpy, nShipsRemaining, sizeof(nShipsRemaining_cpy));
                                for (i = 0; i < nx; ++i) {
                                        int j;
                                        for (j = 0; j < nx; ++j) {
                                                data[i][j] = NONE;
                                        }
                                }
                                ++numOfReTries;
                                break;
                        }

                        /* check the boxes where the ship should be set and its surrounding box for other ships */
                        for (i = 0; i < length; ++i) {
                                /* ships can only be RIGHT (= 0) or DOWN (= 1) */
                                int x_h = (direction == DOWN) ? x : x + i;
                                int y_h = (direction == DOWN) ? y + i : y;
                                int j;

                                if (0 > y_h || y_h >= ny || 0 > x_h || x_h >= nx) {
                                        printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                                        is_free = FALSE;
                                }
                                /* check whether the points in a 3x3 box around each box, that should be filled, are free */
                                for (j = -1; j <= 1 && is_free; ++j) {
                                        int k;
                                        for (k = -1; k <= 1 && is_free; ++k) {
                                                /* first check if the point is within the field (0 <= point < size) and then whether the the point is occupied */
                                                if (0 <= (y_h + k) && (y_h + k) < ny && 0 <= (x_h + j) && (x_h + j) < nx && is_unhit(data[y_h + k][x_h + j])) {
                                                        is_free = FALSE;
                                                }
                                        }
                                }
                        }
                        if (!is_free) continue;

                        /* places the ship onto the the boxes, by changing their state to UNHIT */
                        for (i = 0; i < length; ++i) {
                                /* ships can only be RIGHT (= 0) or DOWN (= 1) */
                                if (direction == DOWN) {
                                        /* direction = DOWN */
                                        data[y + i][x] = UNHIT_VERT;
                                } else {
                                        /* direction = RIGHT */
                                        data[y][x + i] = UNHIT_HORIZ;
                                }
                        }
                        /* decrements the number of ships left of the length "length" */
                        --nShipsRemaining_cpy[length];
                }
        }
}


int test_ship_status(shipstate_t **battle_field, int nShipsRemaining[], int x, int y, int nx, int ny)
{
        if (x < 0 || x >= nx || y < 0 || y >= ny || !is_hit(battle_field[y][x])) {
                printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                return -1;
        }
        if (is_horiz(battle_field[y][x])) {
                int i;
                /* find the leftmost part of the ship and changes x to that */
                while ((0 <= (x - 1)) && is_ship(battle_field[y][x - 1])) {
                        --x;
                }
                /* iterate over the length of the ship and check if every box is hit */
                for (i = x; ((i < nx) && is_ship(battle_field[y][i])); ++i) {
                        if (!is_hit(battle_field[y][i])) {
                                if (is_sunk(battle_field[y][i])) {
                                        printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                                }
                                return FALSE;
                        }
                }
                /* iterate over the length of the ship set all of the hit boxes to sunk */
                for (i = x; ((i < nx) && is_ship(battle_field[y][i])); ++i) {
                        battle_field[y][i] = SUNK_HORIZ;
                }
                --nShipsRemaining[i - x];
                return TRUE;
        } else if (is_vert(battle_field[y][x])) {
                int i;
                /* find the topmost part of the ship and changes y to that */
                while ((0 <= (y - 1)) && is_ship(battle_field[y - 1][x])) {
                        --y;
                }
                /* iterate over the length of the ship and check if every box is hit */
                for (i = y; ((i < ny) && is_ship(battle_field[i][x])); ++i) {
                        if (!is_hit(battle_field[i][x])) {
                                if (is_sunk(battle_field[i][x])) {
                                        printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                                }
                                return FALSE;
                        }
                }
                /* iterate over the length of the ship set all of the hit boxes to sunk */
                for (i = y; ((i < ny) && is_ship(battle_field[i][x])); ++i) {
                        battle_field[i][x] = SUNK_VERT;
                }
                --nShipsRemaining[i - y];
                return TRUE;
        } else {
                printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                return FALSE;
        }
}

int player_shoot(play_fields_t *fld)
{
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_left = fld->data_left;
        int *nShipsRemaining_left = fld->nShipsRemaining_left;

        int x, y;
        int status;

        printf("Type the coordinates, where you want to shoot at.\n");
        while((status = scan_coordinate(&x, &y, nx, ny)) == INPUT_ERROR || (status >= SUCCESS && has_been_shot(data_left[y][x])))  {
                if (status == SUCCESS) {
                        flush_buff();
                }
                printf("Error: Retype the coordinates, where you want to shoot at.\n");
        }
        if(status == BUFFER_ERROR) {
                return BUFFER_ERROR;
        }
        if(status == EXIT) {
                return EXIT;
        }

        switch (data_left[y][x]) {
                case NONE:
                        data_left[y][x] = SPLASH;
                        break;
                case UNHIT_VERT:
                        data_left[y][x] = HIT_VERT;
                        test_ship_status(data_left, nShipsRemaining_left, x, y, nx, ny);
                        break;
                case UNHIT_HORIZ:
                        data_left[y][x] = HIT_HORIZ;
                        test_ship_status(data_left, nShipsRemaining_left, x, y, nx, ny);
                        break;
                default:
                        printf("ERROR: Should not occur - %s line %i\n", __FILE__, __LINE__);
                        return UNKNOWN_ERROR;
        }
        return SUCCESS;
}

void bot_shoot(play_fields_t *fld, int hit_rate)
{
#if 0
        static int x_curr = -1;
        static int y_curr = -1;
#endif
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_right = fld->data_right;
        int *nShipsRemaining_right = fld->nShipsRemaining_right;

        int x, y;

        /* determine whether to hit a ship */
        int hitship = !(rand() % hit_rate);

#if 0
        if (x_curr >= 0 && y_curr >= 0) {
                x = x_curr;
                y = y_curr;
        } else {
#endif
                do {
                        x = rand() % nx;
                        y = rand() % ny;
                } while (hitship ? !(is_unhit(data_right[y][x])) : (data_right[y][x] != NONE));
#if 0
        }
#endif

        switch (data_right[y][x]) {
                case NONE:
                        data_right[y][x] = SPLASH;
                        break;
                case UNHIT_VERT:
                        data_right[y][x] = HIT_VERT;
                        test_ship_status(data_right, nShipsRemaining_right, x, y, nx, ny);
                        break;
                case UNHIT_HORIZ:
                        data_right[y][x] = HIT_HORIZ;
                        test_ship_status(data_right, nShipsRemaining_right, x, y, nx, ny);
                        break;
                default:
                        printf("ERROR: Should not occur - %s line %i\n", __FILE__, __LINE__);
                        return;
        }
        printf("Bot shot at: %c%c%i\n", (x < 26 ? ' ' : (x / 26 - 1) + 'A'), ((x % 26) + 'A'), y);

#if 0
        if (is_hit(data_right[y][x])) {
                x_copy = x_curr;
                y_copy = y_curr;
                if (x_copy >= 0 && y_copy >= 0) {
                        /* check if which direction the previously hit boxes are */
                        if (is_hit(data_right[y][x + 1]) {
                                --x_copy;
                        } else if (is_hit(data_right[y][x + 1]) {
                                ++x_copy;
                        } else if (is_hit(data_right[y + 1][x]) {
                                --y_copy;
                        } else if (is_hit(data_right[y - 1][x]) {
                                ++y_copy;
                        } else {
                                /* no boxes have been previously hit, so either x or y is incremented or decremented */
                                rand() % 2 ? (rand() % 2 ? ++x_copy : --x_copy) : (rand() % 2 ? ++y_copy : --y_copy);
                        }
                        if (x_copy == -1 || has_been_shot(data_right[y_copy][x_copy])) {
                                do {
                                        ++x_copy;
                                } while (is_hit(data_right[y_copy][x_copy]);
                        }
                        if (y_copy == -1) {
                                do {
                                        ++y_copy;
                                } while (is_hit(data_right[y_copy][x_copy]);
                        }
                        if (x_copy == nx) {
                                do {
                                        --x_copy;
                                } while (is_hit(data_right[y_copy][x_copy]);
                        }
                        if (y_copy == ny) {
                                do {
                                        --y_copy;
                                } while (is_hit(data_right[y_copy][x_copy]);
                        }
                } else {
                        /* in case this is the first part of ship that has been shot go in a random direction*/
                        rand() % 2 ? (x_curr = x + (rand() % 2 ? 1 : -1)) : y_curr = (y + (rand() % 2 ? 1 : -1));
                        /* change direction in case of being over the edge */
                        if (x_curr == -1) {
                                x_curr += 2;
                        }
                        if (y_curr == -1) {
                                y_curr += 2;
                        }
                        if (x_curr == nx) {
                                x_curr -= 2;
                        }
                        if (y_curr == ny) {
                                y_curr -= 2;
                        }
                }
        } else if (x_curr >= 0 && y_curr >= 0) {
                if (is_sunk(data_right[y][x])) {
                        x_curr = y_curr = -1;
                }
        }
#endif
}

int has_somemone_won(int *nShipsRemaining)
{
        int i;
        for (i = 0; i <= MAX_SHIP_LENGTH; ++i) {
                if (nShipsRemaining[i] != 0)
                        return 0;
        }
        return 1;
}

int main(int argc, char *argv[])
{
        int i, status, print_vertical = FALSE, auto_choose = FALSE, difficulty = 5;
        int nShipsTotal[MAX_SHIP_LENGTH + 1] = NUM_SHIPS_INIT;
        /* Default with 10 x 10, default NUM_SHIPS_INIT, NULL pointers instead of arrays, which are initialized in alloc_field() */
        play_fields_t field = {10, 10, NUM_SHIPS_INIT, NUM_SHIPS_INIT, NULL, NULL};

        srand(time(NULL));
        rand();

        /* incomplete, should eventually read the arguments */
        for (i = 1; i < argc; ++i) {
                if ('-' == argv[i][0] && '-' != argv[i][1] ) {
                        const char *opt = &argv[i][1];
                        /* printf("process opt: %s\n", argv[i]); */
                        if (strcmp(opt, "a") == 0) {
                                auto_choose = TRUE;
                        } else if (strncmp(opt, "d=", 2) == 0) {
                                char *end;
                                long l;
                                l = strtol(opt + 2, &end, 10);
                                if (end == opt + 2 || *end || l < 1) {
                                        printf("Invalid input: number in argument: \"%s\"!\n", argv[i]);
                                        return INPUT_ERROR;
                                }
                                difficulty = (int)l;
                        } else if (strncmp(opt, "n=", 2) == 0) {
                                char *end;
                                long l;
                                l = strtol(opt + 2, &end, 10);
                                if (end == opt + 2 || *end || l < MAX_SHIP_LENGTH || l > min(MAX_WIDTH, MAX_HEIGHT)) {
                                        printf("Invalid input: number in argument: \"%s\"!\n", argv[i]);
                                        return INPUT_ERROR;
                                }
                                field.nx = (int)l;
                                field.ny = (int)l;
                        } else if (strncmp(opt, "x=", 2) == 0) {
                                char *end;
                                long l;
                                l = strtol(opt + 2, &end, 10);
                                if (end == opt + 2 || *end || l < MAX_SHIP_LENGTH || l > MAX_WIDTH) {
                                        printf("Invalid input: number in argument: \"%s\"!\n", argv[i]);
                                        return INPUT_ERROR;
                                }
                                field.nx = (int)l;
                        } else if (strncmp(opt, "y=", 2) == 0) {
                                char *end;
                                long l;
                                l = strtol(opt + 2, &end, 10);
                                if (end == opt + 2 || *end || l < MAX_SHIP_LENGTH || l > MAX_HEIGHT) {
                                        printf("Invalid input: number in argument: \"%s\"!\n", argv[i]);
                                        return INPUT_ERROR;
                                }
                                field.ny = (int)l;
                        } else if (strcmp(opt, "v") == 0) {
                                print_vertical = TRUE;
                        } else if (opt[0] == 'h') {
                                printf("\nUsage: %s [OPTION]...\n", argv[0]);
                                printf("\nOptions:\n");
                                printf(" -a     player's ships are automatically chosen\n");
                                printf(" -d=<n> sets the difficulty of the bot with 1 as <n> being extremely difficult and a large number <n> being easy\n");
                                printf(" -n=<n> sets the battle-field width and height to <n>\n");
                                printf(" -x=<n> sets the battle-field width to <n>\n");
                                printf(" -y=<n> sets the battle-field height to <n>\n");
                                printf(" -v     prints the fields above eachother\n");
                                printf("(-h)    shows this help\n\n");
                                return SUCCESS;
                        } else {
                                printf("Unknown argument: \"%s\"!\n", argv[i]);
                                return INPUT_ERROR;
                        }
                }
        }

        field.data_left = alloc_field(field.nx, field.ny);
        field.data_right = alloc_field(field.nx, field.ny);

        /* bot's automatic ship choice */
        auto_choose_ships(field.nx, field.ny, field.data_left, field.nShipsRemaining_left);
        /* manual or automatic ship choice for the player */
        if (auto_choose) {
                auto_choose_ships(field.nx, field.ny, field.data_right, field.nShipsRemaining_right);
        } else {
                status = choose_ships(&field, print_vertical);
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
                print_field(&field, print_vertical);
                status = player_shoot(&field);
                if (status == EXIT) {
                        break;
                }
                if (status < 0) {
                        printf("ERROR: Should not occur - %s line %i\n", __FILE__, __LINE__);
                        free_field(&field);
                        return status;
                }
                if (has_somemone_won(field.nShipsRemaining_left)) {
                        print_field(&field, print_vertical);
                        printf("/-------------------\\\n");
                        printf("| PLAYER has won!!! |\n");
                        printf("\\-------------------/\n");
                        break;
                }
                bot_shoot(&field, difficulty);
                if (has_somemone_won(field.nShipsRemaining_right)) {
                        print_field(&field, print_vertical);
                        printf("/----------------\\\n");
                        printf("| BOT has won!!! |\n");
                        printf("\\----------------/\n");
                        break;
                }
        }

        printf("\nPlayer's ships:\n");
        for (i = MIN_SHIP_LENGTH; i <= MAX_SHIP_LENGTH; ++i) {
                printf("    %i of %i length %i ships still floating\n", field.nShipsRemaining_right[i], nShipsTotal[i], i);
        }
        printf("\nBot's ships:\n");
        for (i = MIN_SHIP_LENGTH; i <= MAX_SHIP_LENGTH; ++i) {
                printf("    %i of %i length %i ships still floating\n", field.nShipsRemaining_left[i], nShipsTotal[i], i);
        }

        free_field(&field);

        return SUCCESS;
}

/* ************************************************************************* */

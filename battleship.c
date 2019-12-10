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

#define SUCCESS 0
#define INPUT_ERROR -1
#define BUFFER_ERROR -2

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

/* debug constants */
#define SHOW_BOTS_SHIP
#define SKIP_SHIP_CHOICE


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

enum shipstate
{
        NONE = 0,       /* 0: unhit - water */
        SPLASH,         /* 1: hit water */
        UNHIT_HORIZ,    /* 2: horizontal, unhit ship on the right field*/
        UNHIT_VERT,     /* 3: vertical, unhit ship on the right field*/
        HIT_HORIZ,      /* 4: horizontal, hit ship */
        HIT_VERT,       /* 5: vertical, hit ship */
        SUNK_HORIZ,     /* 6: horizontal, sunken ship */
        SUNK_VERT       /* 7: vertical, sunken ship */
};

typedef enum shipstate shipstate_t;

enum direction
{
        BUFFER_ERROR_DIRECTION = BUFFER_ERROR,
        INPUT_ERROR_DIRECTION = INPUT_ERROR,
        RIGHT = 0,
        DOWN,
        LEFT,
        UP
};

typedef enum direction direction_t;

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
void print_field(play_fields_t *fld)
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_left = fld->data_left;
        shipstate_t **data_right = fld->data_right;

        int row;

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
        printf("\n");
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

int choose_ships(play_fields_t *fld)
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
                        print_field(fld);
                        printf("You have ");
                        for (i = MAX_SHIP_LENGTH; i >= MIN_SHIP_LENGTH; --i) {
                                if (nShipsRemaining[i]) {
                                        printf("%i battle ship%s (length %i) ", nShipsRemaining[i], nShipsRemaining[i] == 1 ? "" : "s", i);
                                }
                        }
                        printf(" remaining\n");
                } else {
                        printField = TRUE;
                }
                printf("Choose where to place your ships, by typing the start and end coordinate of each ship.\n");
                /* scan coordinate and direction, where ship should be placed */
                if ((status = scan_coordinate(&x, &y, nx, ny)) < 0 || (direction = scan_direction(&length)) < 0) {
                        if (status == BUFFER_ERROR || direction == BUFFER_ERROR) {
                                printf("BUFFER ERROR - %s line %i\n", __FILE__, __LINE__);
                                return BUFFER_ERROR;
                        }
                        flush_buff();
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
                                        return INPUT_ERROR;
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
                                        return INPUT_ERROR;
                        }
                }
                /* decrement the number of length-long ships remaining */
                --nShipsRemaining[length];
        }
        return SUCCESS;
}

void bot_choose_ships(play_fields_t *fld)
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_left = fld->data_left;

        int length;
        /* nShipsRemaining[i] is the number of i long ships left */
        int nShipsRemaining[MAX_SHIP_LENGTH + 1];
        /* using "nRemainingShips" as scratch space */
        memcpy(nShipsRemaining, fld->nShipsRemaining_left, sizeof(nShipsRemaining));

        /* populate with ships - place longest ships first, while there still is enough room */
        for (length = MAX_SHIP_LENGTH; length >= MIN_SHIP_LENGTH; --length) {
                int numOfTries = 0;
                while (nShipsRemaining[length]) {
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
                                /* reset variables */
                                length = MAX_SHIP_LENGTH + 1;
                                memcpy(nShipsRemaining, fld->nShipsRemaining_left, sizeof(nShipsRemaining));
                                for (i = 0; i < nx; ++i) {
                                        int j;
                                        for (j = 0; j < nx; ++j) {
                                                data_left[i][j] = NONE;
                                        }
                                }
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
                                                if (0 <= (y_h + k) && (y_h + k) < ny && 0 <= (x_h + j) && (x_h + j) < nx && is_unhit(data_left[y_h + k][x_h + j])) {
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
                                        data_left[y + i][x] = UNHIT_VERT;
                                } else {
                                        /* direction = RIGHT */
                                        data_left[y][x + i] = UNHIT_HORIZ;
                                }
                        }
                        /* decrements the number of ships left of the length "length" */
                        --nShipsRemaining[length];
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
                --nShipsRemaining[x - i];
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
                --nShipsRemaining[y - i];
                putchar('Y');
                return TRUE;
        } else {
                printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                return FALSE;
        }
}

void player_shoot(play_fields_t *fld)
{
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_left = fld->data_left;
        int *nShipsRemaining_left = fld->nShipsRemaining_left;

        int x, y;
        int status;

        printf("Type the coordinates, where you want to shoot at.\n");
        while((status = scan_coordinate(&x, &y, nx, ny)) == INPUT_ERROR || has_been_shot(data_left[y][x]))  {
                printf("Error: Retype the coordinates, where you want to shoot at.\n");
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
                        return;
        }
}

int main(int argc, char *argv[])
{
        int i;
        /* Default with 10 x 10, default NUM_SHIPS_INIT, NULL pointers instead of arrays, which are initialized in alloc_field() */
        play_fields_t field = {10, 10, NUM_SHIPS_INIT, NUM_SHIPS_INIT, NULL, NULL};

        srand(time(NULL));
        rand();

        /* incomplete, should eventually read the arguments */
        for (i = 1; i < argc; ++i) {
                if ('-' == argv[i][0]) {
                        /*const char *opt = &argv[i][1];
                        printf("process opt: %s\n", argv[i]);
                        if (strncmp(opt, "n=", 2) == 0) {
                                printf("get size = %s\n", argv[i]);
                                field_left.nx = 5;
                                field_right.nx = 5;
                        }*/
                }
        }

        field.data_left = alloc_field(field.nx, field.ny);
        field.data_right = alloc_field(field.nx, field.ny);

        bot_choose_ships(&field);
#ifndef SKIP_SHIP_CHOICE
        choose_ships(&field);
#endif

        for (i = 0; i < 6; ++i) {
                print_field(&field);
                player_shoot(&field);
        }
        free_field(&field);

        return 0;
}

/* ************************************************************************* */

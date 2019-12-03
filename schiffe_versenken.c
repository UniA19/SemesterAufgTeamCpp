/* ---------------------------------*- C -*--------------------------------- *\
 *
 * Name
 *
 * Description
 *
 * Author
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

/* The gap between left and right fields */
#define print_gap() (printf("        "))


static const char * const rowFormats[] =
{
        "   |",     /* 0: unhit - water */
        "~~~|",     /* 1: hit water */
        "###|",     /* 2: unhit ship on the right field*/
        "...|",     /* 3: hit ship */
        "XXX|"      /* 4: sunken ship */
};

enum shipstate
{
        NONE = 0,   /* 0: unhit - water */
        SPLASH,     /* 1: hit water */
        UNHIT,      /* 2: unhit ship on the right field*/
        HIT,        /* 3: hit ship */
        SUNK        /* 4: sunken ship */
};

enum playField
{
        LEFT = 0,   /* Must test as FALSE */
        RIGHT = 1   /* Must test as TRUE */
};

typedef struct {
        int nx;
        int ny;
        /* num_of_ship_left[i] is the number of i long ships left */
        int num_of_ships_left_left[6];
        int num_of_ships_left_right[6];
        /* 0 = unhit water, 1 = hit water, 2 = unhit ship, 3 = hit ship, 4 = sunken ship*/
        int **data_left;
        int **data_right;
} field_t;


static int** alloc_field(int nx, int ny)
{
        int i;

        /* Room for all rows (Array of pointers) */
        int ** array2d = (int**) calloc(ny, sizeof(int*));

        /* All data as a single slab */
        int *slabs = (int*) calloc(nx * ny, sizeof(int));

        for (i = 0; i < ny; ++i) {
                array2d[i] = slabs;
                slabs += nx;
        }

        return array2d;
}

static void free_field(field_t* fld)
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

        /* left field */
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

/* prints row including the numbering at the beginning of the line
 * ( 1|~~~|XXX|~~~|...)
 */

void print_row(const int row[], int nx, int row_num, int is_left_field)
{
        int i;

        printf("%2i|", row_num);

        for (i = 0; i < nx; ++i) {
                int state = row[i];
                if (is_left_field && state == 2) {
                        state = 0; /* keep hidden */
                }

                printf(rowFormats[state]);
        }
}


void print_field(field_t * fld)
{
        const int nx = fld->nx;
        const int ny = fld->ny;
        int **data_left = fld->data_left;
        int **data_right = fld->data_right;

        int row;

        print_top_row(nx);
        print_gap();
        print_top_row(nx);
        printf("\n");
        for (row = 0; row < ny; ++row) {
                print_hline(nx);
                print_gap();
                print_hline(nx);
                printf("\n");
                print_row(data_left[row], nx, row + 1, 0);
                print_gap();
                print_row(data_right[row], nx, row + 1, 0);
                printf("\n");
        }
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

int scan_coordinate(int * x, int * y, int nx, int ny)
{
        /* mode: tracks which stage of parsing the function is in:
         * 1 : numbers first: 1st digit
         * 1 : numbers first: 2nd digit
         * 2 : numbers first: 1st letter
         * 3 : numbers first: 2nd letter
         * 4 : letters first: 1st letter
         * 5 : letters first: 2nd letter
         * 6 : letters first: 1st digit
         * 7 : letters first: 2nd digit */
        int mode;
        char c = toupper(getchar());
        int x_hold = 0;
        int y_hold = 0;

        if (c == EOF) {
                return BUFFER_ERROR;
        }

        /* saving first digit */
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
                        /* skips missing second digit */
                        if (mode == 5) {
                                ++mode;
                        }
                        switch (mode) {
                                case 1:
                                        y_hold *= 10;
                                        y_hold += c - '0';
                                        ++mode;
                                        break;
                                case 6:
                                        y_hold += c - '0';
                                        ++mode;
                                        break;
                                case 7:
                                        y_hold *= 10;
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
                                case 3:
                                        x_hold *= 26;
                                        x_hold += c - 'A' + 1;
                                        ++mode;
                                        break;
                                case 5:
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

        if (x_hold > nx || y_hold > ny) {
                return INPUT_ERROR;
        }
        *x = x_hold - 1;
        *y = y_hold - 1;
        return SUCCESS;
}

/* returns direction (up = 0, left = 1, down = 2, right = 3) or negative number in case of error */
int scan_direction(int * length)
{
        int direction = -1;
        int length_hold = -1;
        int i;
        char c;

        for (i = 0; i < 2; ++i) {
                c = toupper(getchar());
                if (c == EOF) {
                        return BUFFER_ERROR;
                }

                switch (c) {
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                                length_hold = c - '0';
                                break;
                        case 'N':
                        case 'U':
                        case '^':
                                direction = 0;
                                break;
                        case 'W':
                        case 'L':
                        case '<':
                                direction = 1;
                                break;
                        case 'S':
                        case 'D':
                        case 'V':
                                direction = 2;
                                break;
                        case 'E':
                        case 'R':
                        case '>':
                                direction = 3;
                                break;
                        default:
                                return INPUT_ERROR;
                }
        }

        if (direction == -1 || length_hold == -1) {
                return INPUT_ERROR;
        }
        *length = length_hold;

        return direction;
}

int choose_ships(field_t * fld)
{
        const int nx = fld->nx;
        const int ny = fld->ny;
        int **data_right = fld->data_right;
        /* num_of_ship_left[i] is the number of i long ships left */
        int num_of_ships_left[6];
        memcpy(num_of_ships_left, fld->num_of_ships_left_right, sizeof(num_of_ships_left));

        while (num_of_ships_left[0] != 0 || num_of_ships_left[1] != 0 || num_of_ships_left[2] != 0 || num_of_ships_left[3] != 0 || num_of_ships_left[4] != 0 || num_of_ships_left[5] != 0) {
                int i, x, y, status, direction, length;
                int is_free = TRUE;

                print_field(fld);

                printf("You have ");
                printf(num_of_ships_left[5] ? "%i battle ship (length 5) " : "", num_of_ships_left[5]);
                printf(num_of_ships_left[4] ? "%i cruisers (length 4) " : "", num_of_ships_left[4]);
                printf(num_of_ships_left[3] ? "%i destroyers (length 3) " : "", num_of_ships_left[3]);
                printf(num_of_ships_left[2] ? "%i sub-marines (length 2) " : "", num_of_ships_left[2]);
                printf("left\n");
                printf("Choose where to place your ships, by typing the start and end coordinate of each ship.\n");
                while (((status = scan_coordinate(&x, &y, nx, ny)) == INPUT_ERROR && status != BUFFER_ERROR)
                    || ((direction = scan_direction(&length)) == INPUT_ERROR && direction != BUFFER_ERROR))
                {
                        flush_buff();
                        printf("Choose where to place your ships, by typing the start and end coordinate of each ship.\n");
                }
                flush_buff();

                if (status == BUFFER_ERROR) {
                        printf("Buffer Error.");
                        return BUFFER_ERROR;
                }

                if (num_of_ships_left[length] <= 0) {
                        continue;
                }

                for (i = 0; i < length; ++i) {
                        int x_h = x;
                        int y_h = y;
                        int j;
                        switch (direction) {
                                case 0:
                                        y_h = y - i;
                                        break;
                                case 1:
                                        x_h = x - i;
                                        break;
                                case 2:
                                        y_h = y + i;
                                        break;
                                case 3:
                                        x_h = x + i;
                                        break;
                                default:
                                        printf("Something really went wrong in choose_ships(), for(), for(), switch(), default.\n");
                                        return INPUT_ERROR;
                        }
                        /* check whether the points in a 3x3 box around each to be filled box is free */
                        for (j = -1; j <= 1; ++j) {
                                int k;
                                for (k = -1; k <= 1; ++k) {
                                        /* first checks if the point is within the field and then whether the the point is occupied */
                                        if (0 <= (y_h + k) && (y_h + k) < ny && 0 <= (x_h + j) && (x_h + j) < nx && *(data_right[y_h + k] + x_h + j) == UNHIT) {
                                                is_free = FALSE;
                                                break;
                                        }
                                }
                                if (!is_free) break;
                        }
                        if (!is_free) break;
                }
                if (!is_free) continue;

                for (i = 0; i < length; ++i) {
                        switch (direction) {
                                case 0:
                                        *(data_right[y - i] + x) = UNHIT;
                                        break;
                                case 1:
                                        *(data_right[y] + x - i) = UNHIT;
                                        break;
                                case 2:
                                        *(data_right[y + i] + x) = UNHIT;
                                        break;
                                case 3:
                                        *(data_right[y] + x + i) = UNHIT;
                                        break;
                                default:
                                        printf("Something really went wrong in choose_ships(), for(), for(), switch(), default.\n");
                                        return INPUT_ERROR;
                        }
                }

                --num_of_ships_left[length];
        }
        return SUCCESS;
}

int main(int argc, char *argv[])
{
        int i;

        /* Default with 10 x 10 */
        field_t field = {10, 10, {0, 0, 4, 3, 2, 1}, {0, 0, 4, 3, 2, 1}, NULL, NULL};

        for (i = 1; i < argc; ++i) {
                if ('-' == argv[i][0]) {
                        /*const char* opt = &argv[i][1];
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

        choose_ships(&field);

        print_field(&field);

        free_field(&field);

        return 0;
}

/* ************************************************************************* */

#include "input.h"


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

int flush_buff()
{
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        return (c == EOF) ? BUFFER_ERROR : SUCCESS;
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
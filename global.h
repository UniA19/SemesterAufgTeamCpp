#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_WIDTH 500
#define MAX_HEIGHT 99

#define TRUE 1
#define FALSE 0

/* number of ships (value) of each length (position in array)
   Number of elements in NUM_SHIPS_INIT should be equal to MAX_SHIP_LENGTH + 1 */
#define NUM_SHIPS_INIT {0, 0, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
/* Initial longest ship */
#define MAX_SHIP_LENGTH_INIT 5
/* Total min / max ship length */
#define MIN_SHIP_LENGTH 1
#define MAX_SHIP_LENGTH 20

#define min(a, b) ((a) < (b) ? (a) : (b))

/* debug constants */
#undef SHOW_BOTS_SHIP


typedef enum {
        NONE = 0,               /* 0: unhit - water */
        SPLASH,                 /* 1: hit water */
        UNHIT_HORIZ,            /* 2: middle of horizontal, unhit ship on the right field */
        UNHIT_HORIZ_LEFT,       /* 3: left end of horizontal, unhit ship on the right field */
        UNHIT_HORIZ_RIGHT,      /* 4: right end of horizontal, unhit ship on the right field */
        UNHIT_VERT,             /* 5: vertical, unhit ship on the right field */
        UNHIT_VERT_TOP,         /* 6: top end of vertical, unhit ship on the right field */
        UNHIT_VERT_BOTTOM,      /* 7: bottom end of vertical, unhit ship on the right field */
        UNHIT_SINGLE,           /* 8: unhit ship on the right field of the length 1 */
        HIT,                    /* 9: hit ship (on left side) */
        HIT_HORIZ,              /* 10: horizontal, hit ship */
        HIT_HORIZ_LEFT,         /* 11: left end of horizontal, hit ship */
        HIT_HORIZ_RIGHT,        /* 12: right end of horizontal, hit ship */
        HIT_VERT,               /* 13: vertical, hit ship */
        HIT_VERT_TOP,           /* 14: top end of vertical, hit ship */
        HIT_VERT_BOTTOM,        /* 15: bottom end of vertical, hit ship */
        HIT_SINGLE,             /* 16: hit ship of the length 1 */
        SUNK_HORIZ,             /* 17: horizontal, sunken ship */
        SUNK_HORIZ_LEFT,        /* 18: left end of horizontal, sunken ship */
        SUNK_HORIZ_RIGHT,       /* 19: right end of horizontal, sunken ship */
        SUNK_VERT,              /* 20: vertical, sunken ship */
        SUNK_VERT_TOP,          /* 21: top end of vertical, sunken ship */
        SUNK_VERT_BOTTOM,       /* 22: bottom end of vertical, sunken ship */
        SUNK_SINGLE,            /* 23: sunken ship of the lenght 1 */
        NONE_INVERT,                   /* 24: unhit - water NOTE: after this point the fields are inverted */
        SPLASH_INVERT,                 /* 25: hit water */
        UNHIT_HORIZ_INVERT,            /* 26: horizontal */
        UNHIT_HORIZ_LEFT_INVERT,       /* 27: left end of horizontal, unhit ship on the right field */
        UNHIT_HORIZ_RIGHT_INVERT,      /* 28: right end of horizontal, unhit ship on the right field */
        UNHIT_VERT_INVERT,             /* 29: vertical, unhit ship on the right field */
        UNHIT_VERT_TOP_INVERT,         /* 30: top end of vertical, unhit ship on the right field */
        UNHIT_VERT_BOTTOM_INVERT,      /* 31: bottom end of vertical, unhit ship on the right field */
        UNHIT_SINGLE_INVERT,           /* 32: unhit ship on the right field of the length 1 */
        HIT_INVERT,                    /* 33: hit ship (on left side) */
        HIT_HORIZ_INVERT,              /* 34: horizontal, hit ship */
        HIT_HORIZ_LEFT_INVERT,         /* 35: left end of horizontal, hit ship */
        HIT_HORIZ_RIGHT_INVERT,        /* 36: right end of horizontal, hit ship */
        HIT_VERT_INVERT,               /* 37: vertical, hit ship */
        HIT_VERT_TOP_INVERT,           /* 38: top end of vertical, hit ship */
        HIT_VERT_BOTTOM_INVERT,        /* 39: bottom end of vertical, hit ship */
        HIT_SINGLE_INVERT,             /* 40: hit ship of the length 1 */
        SUNK_HORIZ_INVERT,             /* 41: horizontal, sunken ship */
        SUNK_HORIZ_LEFT_INVERT,        /* 42: left end of horizontal, sunken ship */
        SUNK_HORIZ_RIGHT_INVERT,       /* 43: right end of horizontal, sunken ship */
        SUNK_VERT_INVERT,              /* 44: vertical, sunken ship */
        SUNK_VERT_TOP_INVERT,          /* 45: top end of vertical, sunken ship */
        SUNK_VERT_BOTTOM_INVERT,       /* 46: bottom end of vertical, sunken ship */
        SUNK_SINGLE_INVERT            /* 47: sunken ship of the lenght 1 */
} shipstate_t;

/* Convenience macros for handling hit/unhit status */
#define is_unhit(box) ((UNHIT_HORIZ <= (box) && (box) <= UNHIT_SINGLE) || (UNHIT_HORIZ_INVERT <= (box) && (box) <= UNHIT_SINGLE_INVERT))
#define is_hit(box) ((HIT <= (box) && (box) <= HIT_SINGLE) || (HIT_INVERT <= (box) && (box) <= HIT_SINGLE_INVERT))
#define is_sunk(box) ((SUNK_HORIZ <= (box) && (box) <= SUNK_SINGLE) || (SUNK_HORIZ_INVERT <= (box) && (box) <= SUNK_SINGLE_INVERT))
#define is_ship(box) (is_unhit(box) || is_hit(box) || is_sunk(box))
#define is_left(box) ((box) == UNHIT_HORIZ_LEFT || (box) == HIT_HORIZ_LEFT || (box) == SUNK_HORIZ_LEFT || (box) == UNHIT_HORIZ_LEFT_INVERT || (box) == HIT_HORIZ_LEFT_INVERT || (box) == SUNK_HORIZ_LEFT_INVERT)
#define is_right(box) ((box) == UNHIT_HORIZ_RIGHT || (box) == HIT_HORIZ_RIGHT || (box) == SUNK_HORIZ_RIGHT || (box) == UNHIT_HORIZ_RIGHT_INVERT || (box) == HIT_HORIZ_RIGHT_INVERT || (box) == SUNK_HORIZ_RIGHT_INVERT)
#define is_top(box) ((box) == UNHIT_VERT_TOP || (box) == HIT_VERT_TOP || (box) == SUNK_VERT_TOP || (box) == UNHIT_VERT_TOP_INVERT || (box) == HIT_VERT_TOP_INVERT || (box) == SUNK_VERT_TOP_INVERT)
#define is_bottom(box) ((box) == UNHIT_VERT_BOTTOM || (box) == HIT_VERT_BOTTOM || (box) == SUNK_VERT_BOTTOM || (box) == UNHIT_VERT_BOTTOM_INVERT || (box) == HIT_VERT_BOTTOM_INVERT || (box) == SUNK_VERT_BOTTOM_INVERT)
#define has_been_shot(box) ((box) == SPLASH || (box) == SPLASH_INVERT || is_hit(box) || is_sunk(box))
#define is_horiz(box) ((UNHIT_HORIZ <= (box) && (box) <= UNHIT_HORIZ_RIGHT) || (HIT_HORIZ <= (box) && (box) <= HIT_HORIZ_RIGHT) || (SUNK_HORIZ <= (box) && (box) <= SUNK_HORIZ_RIGHT) || (UNHIT_HORIZ_INVERT <= (box) && (box) <= UNHIT_HORIZ_RIGHT_INVERT) || (HIT_HORIZ_INVERT <= (box) && (box) <= HIT_HORIZ_RIGHT_INVERT) || (SUNK_HORIZ_INVERT <= (box) && (box) <= SUNK_HORIZ_RIGHT_INVERT))
#define is_vert(box) ((UNHIT_VERT <= (box) && (box) <= UNHIT_VERT_BOTTOM) || (HIT_VERT <= (box) && (box) <= HIT_VERT_BOTTOM) || (SUNK_VERT <= (box) && (box) <= SUNK_VERT_BOTTOM) || (UNHIT_VERT_INVERT <= (box) && (box) <= UNHIT_VERT_BOTTOM_INVERT) || (HIT_VERT_INVERT <= (box) && (box) <= HIT_VERT_BOTTOM_INVERT) || (SUNK_VERT_INVERT <= (box) && (box) <= SUNK_VERT_BOTTOM_INVERT))
#define is_single(box) ((box) == UNHIT_SINGLE || (box) == HIT_SINGLE || (box) == SUNK_SINGLE || (box) == UNHIT_SINGLE_INVERT || (box) == HIT_SINGLE_INVERT || (box) == SUNK_SINGLE_INVERT)
#define INVERT_DIFF (NONE_INVERT)
#define is_invert(box) (NONE_INVERT <= (box) && (box) <= SUNK_SINGLE_INVERT)
#define invert(box) (is_invert(box) ? (box - INVERT_DIFF) : (box + INVERT_DIFF))
#define remove_invert(box) ((box) % INVERT_DIFF)

typedef enum {
        UNKNOWN_ERROR = -5,
        ALLOC_ERROR = -4,
        EXIT = -3,
        BUFFER_ERROR = -2,
        INPUT_ERROR = -1,
        SUCCESS = 0,
        SUCCESS_ENDL = 1,
        SUCCESS_HIT = 2
} status_t;

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
        int n_ships_remaining_left[MAX_SHIP_LENGTH + 1];
        int n_ships_remaining_right[MAX_SHIP_LENGTH + 1];
        int max_ship_length;
        /* 0 = unhit water, 1 = hit water, 2 = unhit ship, 3 = hit ship, 4 = sunken ship*/
        shipstate_t **data_left;
        shipstate_t **data_right;
        int print_color;
        int print_utf;
        int print_vertical;
} play_fields_t;

typedef struct {
        int x;
        int y;
} point_t;

#endif

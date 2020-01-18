/* ---------------------------------*- C -*--------------------------------- *\
 *
\* ------------------------------------------------------------------------- */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_WIDTH 99
#define MAX_HEIGHT MAX_WIDTH

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

/* The gap between left and right fields */
#define print_gap() (printf("        "))
#define printh_stat_line(print_utf, max_ship_length) \
{\
        int length;\
        print_utf ? printf("───────┼") : printf("-------|");\
        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) print_utf ? printf("───%s", (length == max_ship_length) ? "┤" : "┼") : printf("---|");\
}


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

static const char *const row_formats[] =
{
        "   |",         /* 0: unhit - water */
        "~~~|",         /* 1: hit water */
        "=O=|",         /* 2: horizontal, unhit ship on the right field */
        "<O=|",         /* 3: left end of horizontal, unhit ship on the right field */
        "=O>|",         /* 4: right end of horizontal, unhit ship on the right field */
        "|O||",         /* 5: vertical, unhit ship on the right field */
        "/o\\|",        /* 6: top end of vertical, unhit ship on the right field */
        "\\o/|",        /* 7: bottom end of vertical, unhit ship on the right field */
        "(O)|",         /* 8: unhit ship on the right field of the length 1 */
        " X |",         /* 9: hit ship (on left side) */
        "=X=|",         /* 10: horizontal, hit ship */
        "<X=|",         /* 11: left end of horizontal, hit ship */
        "=X>|",         /* 12: right end of horizontal, hit ship */
        "|X||",         /* 13: vertical, hit ship */
        "/X\\|",        /* 14: top end of vertical, hit ship */
        "\\X/|",        /* 15: bottom end of vertical, hit ship */
        "(X)|",         /* 16: hit ship of the length 1*/
        "X+X|",         /* 17: horizontal, sunken ship */
        "<+X|",         /* 18: left end of horizontal, sunken ship */
        "X+>|",         /* 19: right end of horizontal, sunken ship */
        "X+X|",         /* 20: vertical, sunken ship */
        "/+\\|",        /* 21: top end of vertical, sunken ship */
        "\\+/|",        /* 22: bottom end of vertical, sunken ship */
        "(+)|",         /* 23: sunken ship of the lenght 1 */
        "   |",         /* 24: unhit - water NOTE: after this point the fields are inverted, which doesn't work without ansi coloring so there is no change here */
        "~~~|",         /* 25: hit water */
        "=O=|",         /* 26: horizontal, unhit ship on the right field */
        "<O=|",         /* 27: left end of horizontal, unhit ship on the right field */
        "=O>|",         /* 28: right end of horizontal, unhit ship on the right field */
        "|O||",         /* 29: vertical, unhit ship on the right field */
        "/o\\|",        /* 30: top end of vertical, unhit ship on the right field */
        "\\o/|",        /* 31: bottom end of vertical, unhit ship on the right field */
        "(O)|",         /* 32: unhit ship on the right field of the length 1 */
        " X |",         /* 33: hit ship (on left side) */
        "=X=|",         /* 34: horizontal, hit ship */
        "<X=|",         /* 35: left end of horizontal, hit ship */
        "=X>|",         /* 36: right end of horizontal, hit ship */
        "|X||",         /* 37: vertical, hit ship */
        "/X\\|",        /* 38: top end of vertical, hit ship */
        "\\X/|",        /* 39: bottom end of vertical, hit ship */
        "(X)|",         /* 40: hit ship of the length 1*/
        "X+X|",         /* 41: horizontal, sunken ship */
        "<+X|",         /* 42: left end of horizontal, sunken ship */
        "X+>|",         /* 43: right end of horizontal, sunken ship */
        "X+X|",         /* 44: vertical, sunken ship */
        "/+\\|",        /* 45: top end of vertical, sunken ship */
        "\\+/|",        /* 46: bottom end of vertical, sunken ship */
        "(+)|"          /* 47: sunken ship of the lenght 1 */
};

static const char *const row_formats_utf[] =
{
        "   │",         /* 0: unhit - water */
        "≈≈≈│",         /* 1: hit water */
        "═●═│",         /* 2: horizontal, unhit ship on the right field */
        "<●═│",         /* 3: left end of horizontal, unhit ship on the right field */
        "═●>│",         /* 4: right end of horizontal, unhit ship on the right field */
        "│●││",         /* 5: vertical, unhit ship on the right field */
        "/●\\│",        /* 6: top end of vertical, unhit ship on the right field */
        "\\●/│",        /* 7: bottom end of vertical, unhit ship on the right field */
        "(●)│",         /* 8: unhit ship on the right field of the length 1 */
        " ╳ │",         /* 9: hit ship (on left side) */
        "═╳═│",         /* 10: horizontal, hit ship */
        "<╳═│",         /* 11: left end of horizontal, hit ship */
        "═╳>│",         /* 12: right end of horizontal, hit ship */
        "│╳││",         /* 13: vertical, hit ship */
        "/╳\\│",        /* 14: top end of vertical, hit ship */
        "\\╳/│",        /* 15: bottom end of vertical, hit ship */
        "(╳)│",         /* 16: hit ship of the length 1*/
        "╳+╳│",         /* 17: horizontal, sunken ship */
        "<+╳│",         /* 18: left end of horizontal, sunken ship */
        "╳+>│",         /* 19: right end of horizontal, sunken ship */
        "╳+╳│",         /* 20: vertical, sunken ship */
        "/+\\│",        /* 21: top end of vertical, sunken ship */
        "\\+/│",        /* 22: bottom end of vertical, sunken ship */
        "(+)│",         /* 23: sunken ship of the lenght 1 */
        "   │",         /* 24: unhit - water NOTE: after this point the fields are inverted, which doesn't work without ansi coloring so there is no change here */
        "≈≈≈│",         /* 25: hit water */
        "═●═│",         /* 26: horizontal, unhit ship on the right field */
        "<●═│",         /* 27: left end of horizontal, unhit ship on the right field */
        "═●>│",         /* 28: right end of horizontal, unhit ship on the right field */
        "│●││",         /* 29: vertical, unhit ship on the right field */
        "/●\\│",        /* 30: top end of vertical, unhit ship on the right field */
        "\\●/│",        /* 31: bottom end of vertical, unhit ship on the right field */
        "(●)│",         /* 32: unhit ship on the right field of the length 1 */
        " ╳ │",         /* 33: hit ship (on left side) */
        "═╳═│",         /* 34: horizontal, hit ship */
        "<╳═│",         /* 35: left end of horizontal, hit ship */
        "═╳>│",         /* 36: right end of horizontal, hit ship */
        "│╳││",         /* 37: vertical, hit ship */
        "/╳\\│",        /* 38: top end of vertical, hit ship */
        "\\╳/│",        /* 39: bottom end of vertical, hit ship */
        "(╳)│",         /* 40: hit ship of the length 1*/
        "╳+╳│",         /* 41: horizontal, sunken ship */
        "<+╳│",         /* 42: left end of horizontal, sunken ship */
        "╳+>│",         /* 43: right end of horizontal, sunken ship */
        "╳+╳│",         /* 44: vertical, sunken ship */
        "/+\\│",        /* 45: top end of vertical, sunken ship */
        "\\+/│",        /* 46: bottom end of vertical, sunken ship */
        "(+)│"         /* 47: sunken ship of the lenght 1 */
};

/* ansi-colors:
  set color: '\033[' <clr> { ';' <clr> } 'm'
    <clr>: https://en.wikipedia.org/wiki/ANSI_escape_code#3/4_bit
           https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_parameters

  unset color: \033[0m
*/
static const char *const row_formats_color[] =
{
        "   |",                         /* 0: unhit - water */
        "\033[34;1m~~~\033[0m|",        /* 1: hit water */
        "\033[32;1m=O=\033[0m|",        /* 2: horizontal, unhit ship on the right field */
        "\033[32;1m<O=\033[0m|",        /* 3: left end of horizontal, unhit ship on the right field */
        "\033[32;1m=O>\033[0m|",        /* 4: right end of horizontal, unhit ship on the right field */
        "\033[32;1m|O|\033[0m|",        /* 5: vertical, unhit ship on the right field */
        "\033[32;1m/o\\\033[0m|",       /* 6: top end of vertical, unhit ship on the right field */
        "\033[32;1m\\o/\033[0m|",       /* 7: bottom end of vertical, unhit ship on the right field */
        "\033[32;1m(O)\033[0m|",        /* 8: unhit ship on the right field of the length 1 */
        "\033[31;1m X \033[0m|",        /* 9: hit ship (on left side) */
        "\033[31;1m=X=\033[0m|",        /* 10: horizontal, hit ship */
        "\033[31;1m<X=\033[0m|",        /* 11: left end of horizontal, hit ship */
        "\033[31;1m=X>\033[0m|",        /* 12: right end of horizontal, hit ship */
        "\033[31;1m|X|\033[0m|",        /* 13: vertical, hit ship */
        "\033[31;1m/X\\\033[0m|",       /* 14: top end of vertical, hit ship */
        "\033[31;1m\\X/\033[0m|",       /* 15: bottom end of vertical, hit ship */
        "\033[31;1m(X)\033[0m|",        /* 16: hit ship of the length 1*/
        "\033[35;1mX+X\033[0m|",        /* 17: horizontal, sunken ship */
        "\033[35;1m<+X\033[0m|",        /* 18: left end of horizontal, sunken ship */
        "\033[35;1mX+>\033[0m|",        /* 19: right end of horizontal, sunken ship */
        "\033[35;1mX+X\033[0m|",        /* 20: vertical, sunken ship */
        "\033[35;1m/+\\\033[0m|",       /* 21: top end of vertical, sunken ship */
        "\033[35;1m\\+/\033[0m|",       /* 22: bottom end of vertical, sunken ship */
        "\033[35;1m(+)\033[0m|",        /* 23: sunken ship of the lenght 1 */
        "   |",                         /* 24: unhit - water NOTE: after this point the fields are inverted, which doesn't work without ansi coloring so there is no change here */
        "\033[34;1;7m~~~\033[0m|",     /* 25: hit water */
        "\033[32;1;7m=O=\033[0m|",     /* 26: horizontal, unhit ship on the right field */
        "\033[32;1;7m<O=\033[0m|",     /* 27: left end of horizontal, unhit ship on the right field */
        "\033[32;1;7m=O>\033[0m|",     /* 28: right end of horizontal, unhit ship on the right field */
        "\033[32;1;7m|O|\033[0m|",     /* 29: vertical, unhit ship on the right field */
        "\033[32;1;7m/o\\\033[0m|",    /* 30: top end of vertical, unhit ship on the right field */
        "\033[32;1;7m\\o/\033[0m|",    /* 31: bottom end of vertical, unhit ship on the right field */
        "\033[32;1;7m(O)\033[0m|",     /* 32: unhit ship on the right field of the length 1 */
        "\033[31;1;7m X \033[0m|",     /* 33: hit ship (on left side) */
        "\033[31;1;7m=X=\033[0m|",     /* 34: horizontal, hit ship */
        "\033[31;1;7m<X=\033[0m|",     /* 35: left end of horizontal, hit ship */
        "\033[31;1;7m=X>\033[0m|",     /* 36: right end of horizontal, hit ship */
        "\033[31;1;7m|X|\033[0m|",     /* 37: vertical, hit ship */
        "\033[31;1;7m/X\\\033[0m|",    /* 38: top end of vertical, hit ship */
        "\033[31;1;7m\\X/\033[0m|",    /* 39: bottom end of vertical, hit ship */
        "\033[31;1;7m(X)\033[0m|",     /* 40: hit ship of the length 1*/
        "\033[35;1;7mX+X\033[0m|",     /* 41: horizontal, sunken ship */
        "\033[35;1;7m<+X\033[0m|",     /* 42: left end of horizontal, sunken ship */
        "\033[35;1;7mX+>\033[0m|",     /* 43: right end of horizontal, sunken ship */
        "\033[35;1;7mX+X\033[0m|",     /* 44: vertical, sunken ship */
        "\033[35;1;7m/+\\\033[0m|",    /* 45: top end of vertical, sunken ship */
        "\033[35;1;7m\\+/\033[0m|",    /* 46: bottom end of vertical, sunken ship */
        "\033[35;1;7m(+)\033[0m|"      /* 47: sunken ship of the lenght 1 */
};

static const char *const row_formats_color_utf[] =
{
        "   │",                         /* 0: unhit - water */
        "\033[34;1m≈≈≈\033[0m│",        /* 1: hit water */
        "\033[32;1m═●═\033[0m│",        /* 2: horizontal, unhit ship on the right field */
        "\033[32;1m<●═\033[0m│",        /* 3: left end of horizontal, unhit ship on the right field */
        "\033[32;1m═●>\033[0m│",        /* 4: right end of horizontal, unhit ship on the right field */
        "\033[32;1m│●│\033[0m│",        /* 5: vertical, unhit ship on the right field */
        "\033[32;1m/●\\\033[0m│",       /* 6: top end of vertical, unhit ship on the right field */
        "\033[32;1m\\●/\033[0m│",       /* 7: bottom end of vertical, unhit ship on the right field */
        "\033[32;1m(●)\033[0m│",        /* 8: unhit ship on the right field of the length 1 */
        "\033[31;1m ╳ \033[0m│",        /* 9: hit ship (on left side) */
        "\033[31;1m═╳═\033[0m│",        /* 10: horizontal, hit ship */
        "\033[31;1m<╳═\033[0m│",        /* 11: left end of horizontal, hit ship */
        "\033[31;1m═╳>\033[0m│",        /* 12: right end of horizontal, hit ship */
        "\033[31;1m│╳│\033[0m│",        /* 13: vertical, hit ship */
        "\033[31;1m/╳\\\033[0m│",       /* 14: top end of vertical, hit ship */
        "\033[31;1m\\╳/\033[0m│",       /* 15: bottom end of vertical, hit ship */
        "\033[31;1m(╳)\033[0m│",        /* 16: hit ship of the length 1*/
        "\033[35;1m╳+╳\033[0m│",        /* 17: horizontal, sunken ship */
        "\033[35;1m<+╳\033[0m│",        /* 18: left end of horizontal, sunken ship */
        "\033[35;1m╳+>\033[0m│",        /* 19: right end of horizontal, sunken ship */
        "\033[35;1m╳+╳\033[0m│",        /* 20: vertical, sunken ship */
        "\033[35;1m/+\\\033[0m│",       /* 21: top end of vertical, sunken ship */
        "\033[35;1m\\+/\033[0m│",       /* 22: bottom end of vertical, sunken ship */
        "\033[35;1m(+)\033[0m│",        /* 23: sunken ship of the lenght 1 */
        "   │",                         /* 24: unhit - water NOTE: after this point the fields are inverted, which doesn't work without ansi coloring so there is no change here */
        "\033[34;1;7m≈≈≈\033[0m│",     /* 25: hit water */
        "\033[32;1;7m═●═\033[0m│",     /* 26: horizontal, unhit ship on the right field */
        "\033[32;1;7m<●═\033[0m│",     /* 27: left end of horizontal, unhit ship on the right field */
        "\033[32;1;7m═●>\033[0m│",     /* 28: right end of horizontal, unhit ship on the right field */
        "\033[32;1;7m│●│\033[0m│",     /* 29: vertical, unhit ship on the right field */
        "\033[32;1;7m/●\\\033[0m│",    /* 30: top end of vertical, unhit ship on the right field */
        "\033[32;1;7m\\●/\033[0m│",    /* 31: bottom end of vertical, unhit ship on the right field */
        "\033[32;1;7m(●)\033[0m│",     /* 32: unhit ship on the right field of the length 1 */
        "\033[31;1;7m ╳ \033[0m│",     /* 33: hit ship (on left side) */
        "\033[31;1;7m═╳═\033[0m│",     /* 34: horizontal, hit ship */
        "\033[31;1;7m<╳═\033[0m│",     /* 35: left end of horizontal, hit ship */
        "\033[31;1;7m═╳>\033[0m│",     /* 36: right end of horizontal, hit ship */
        "\033[31;1;7m│╳│\033[0m│",     /* 37: vertical, hit ship */
        "\033[31;1;7m/╳\\\033[0m│",    /* 38: top end of vertical, hit ship */
        "\033[31;1;7m\\╳/\033[0m│",    /* 39: bottom end of vertical, hit ship */
        "\033[31;1;7m(╳)\033[0m│",     /* 40: hit ship of the length 1*/
        "\033[35;1;7m╳+╳\033[0m│",     /* 41: horizontal, sunken ship */
        "\033[35;1;7m<+╳\033[0m│",     /* 42: left end of horizontal, sunken ship */
        "\033[35;1;7m╳+>\033[0m│",     /* 43: right end of horizontal, sunken ship */
        "\033[35;1;7m╳+╳\033[0m│",     /* 44: vertical, sunken ship */
        "\033[35;1;7m/+\\\033[0m│",    /* 45: top end of vertical, sunken ship */
        "\033[35;1;7m\\+/\033[0m│",    /* 46: bottom end of vertical, sunken ship */
        "\033[35;1;7m(+)\033[0m│"      /* 47: sunken ship of the lenght 1 */
};

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

/* prints horizontal line, deviding the rows (--+---+---+...) */
void print_hline(int nx, int print_utf)
{
        if (print_utf) {
                if (print_utf == 1) {
                        int coli;
                        printf("──┼");
                        for (coli = 0; coli < nx; ++coli) {
                                if (coli == nx - 1) {
                                        printf("───┤");
                                } else {
                                        printf("───┼");
                                }
                        }
                } else {
                        int coli;
                        printf("──┴");
                        for (coli = 0; coli < nx; ++coli) {
                                if (coli == nx - 1) {
                                        printf("───┘");
                                } else {
                                        printf("───┴");
                                }
                        }
                }
        } else {
                int coli;
                printf("--+");
                for (coli = 0; coli < nx; ++coli) {
                        printf("---+");
                }
        }
}

/* prints top row (  | A | B | C | ... ) */
void print_top_row(int nx, int print_utf)
{
        if (print_utf) {
                int coli;
                printf("  │");
                for (coli = 0; coli < nx; ++coli) {
                        printf("%c%c │", (coli < 26 ? ' ' : (coli / 26 - 1) + 'A'), ((coli % 26) + 'A'));
                }
        } else {
                int coli;
                printf("  |");
                /* format as "AA" etc if there are more than 26 cols */
                for (coli = 0; coli < nx; ++coli) {
                        printf("%c%c |", (coli < 26 ? ' ' : (coli / 26 - 1) + 'A'), ((coli % 26) + 'A'));
                }
        }
}

/* prints row including the numbering at the beginning of the line ( 1|~~~|XXX|~~~|...) */
void print_row(shipstate_t row[], int nx, int row_num, int is_left_field, int print_color, int print_utf)
{
        int coli;

        printf("%2i%s", row_num, (print_utf ? "│" : "|"));

        for (coli = 0; coli < nx; ++coli) {
                shipstate_t state = row[coli];
#ifndef SHOW_BOTS_SHIP
                if (is_left_field) {
                        if (is_unhit(state)) {
                                state = NONE; /* keep hidden */
                        }
                        if (is_hit(state)) {
                                state = is_invert(state) ? HIT_INVERT : HIT;
                        }
                }
#endif
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

                row[coli] = remove_invert(row[coli]);
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
        const int print_vertical = fld->print_vertical;
        const int print_color = fld->print_color;
        const int print_utf = fld->print_utf;

        const char bot_title[] = "BOT's ships";
        const char player_title[] = "PLAYER's ships";
        const int nCharsX = 4 * nx + 3;

        if (print_vertical) {
                int players_field;
                for (players_field = 0; players_field <= 1; ++players_field) {
                        int row;
                        /* title: e.g.:     BOT's ships     */
                        if (!players_field) {
                                if (print_color) printf("\033[31;1m");
                                printf("\n%*s%*s\n", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                        } else {
                                if (print_color) printf("\033[32;1m");
                                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                        }
                        if (print_color) printf("\033[0m");
                        /* top row:   | A | B |...*/
                        print_top_row(nx, print_utf);
                        printf("\n");
                        for (row = 0; row < ny; ++row) {
                                /* horizontal line: --+---+---+...*/
                                print_hline(nx, print_utf);
                                printf("\n");
                                /* other rows:   <row>|~~~|XXX|...*/
                                if (!players_field) {
                                        print_row(data_left[row], nx, row + 1, 1, print_color, print_utf);
                                } else {
                                        print_row(data_right[row], nx, row + 1, 0, print_color, print_utf);
                                }
                                printf("\n");
                        }
                        /* horizontal line: --+---+---+...*/
                        print_hline(nx, 2 * print_utf);
                        printf("\n\n");
                }
        } else {
                int row;
                /* title: e.g.:     BOT's ships     */
                if (print_color) printf("\033[31;1m");
                printf("\n%*s%*s", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                print_gap();
                if (print_color) printf("\033[32;1m");
                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                if (print_color) printf("\033[0m");

                /* top row:   | A | B |...          | A | B |... */
                print_top_row(nx, print_utf);
                print_gap();
                print_top_row(nx, print_utf);
                printf("\n");
                for (row = 0; row < ny; ++row) {
                        /* horizontal line: --+---+---+...        --+---+---+... */
                        print_hline(nx, print_utf);
                        print_gap();
                        print_hline(nx, print_utf);
                        printf("\n");
                        /* other rows:   <row>|~~~|XXX|...          |   |XXX|... */
                        print_row(data_left[row], nx, row + 1, 1, print_color, print_utf);
                        print_gap();
                        print_row(data_right[row], nx, row + 1, 0, print_color, print_utf);
                        printf("\n");
                }
                /* horizontal line: --+---+---+...        --+---+---+... */
                print_hline(nx, 2 * print_utf);
                print_gap();
                print_hline(nx, 2 * print_utf);
                printf("\n\n");
        }
}

/* Counts how many ships ships of which length have how many hits */
void countShipHits(shipstate_t **field, int nx, int ny, int shipCount[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1]) {
        int i;
        int y;
        for (i = 0; i <= MAX_SHIP_LENGTH; ++i) {
                int j;
                for (j = 0; j <= MAX_SHIP_LENGTH; ++j) {
                        shipCount[i][j] = 0;
                }
        }
        for (y = 0; y < ny; ++y) {
                int x;
                for (x = 0; x < nx; ++x) {
                        if (is_left(field[y][x])) {
                                int length = 0, n_hits = 0;
                                do  {
                                        if (has_been_shot(field[y][x + length])) {
                                                ++n_hits;
                                        }
                                        ++length;
                                } while (!is_right(field[y][x + length - 1]));
                                ++shipCount[length][n_hits];
                        } else if (is_top(field[y][x])) {
                                int length = 0, n_hits = 0;
                                do {
                                        if (has_been_shot(field[y + length][x])) {
                                                ++n_hits;
                                        }
                                        ++length;
                                } while (!is_bottom(field[y + length - 1][x]));
                                ++shipCount[length][n_hits];
                        } else if (is_single(field[y][x])) {
                                has_been_shot(field[x][y]) ? ++shipCount[1][1] : ++shipCount[1][0];
                        }
                }
        }
}

void print_stats(play_fields_t *fld, int n_ships_total[])
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        const int max_ship_length = fld->max_ship_length;
        shipstate_t **data_left = fld->data_left;
        shipstate_t **data_right = fld->data_right;
        const int print_vertical = fld->print_vertical;
        const int print_color = fld->print_color;
        const int print_utf = fld->print_utf;

        const char bot_title[] = "BOT's ships";
        const char player_title[] = "PLAYER's ships";
        const int nCharsX = 4 * max_ship_length + 8;
        char vert_bar[sizeof("│")];
        int length, n_hits;

        strcpy(vert_bar, print_utf ? "│" : "|");


        if (print_vertical) {
                /* ship_count[<length>][<n_hits>] corresponds to the number of ships of length <length> that have been hit <n_hits> times */
                int ship_count[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1];
                int players_field;
                for (players_field = 0; players_field <= 1; ++players_field) {
                        if (!players_field) {
                                countShipHits(data_left, nx, ny, ship_count);
                                if (print_color) printf("\033[31;1m");
                                printf("\n%*s%*s\n", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                                if (print_color) printf("\033[0m");
                        } else {
                                countShipHits(data_right, nx, ny, ship_count);
                                if (print_color) printf("\033[32;1m");
                                printf("\n%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                                if (print_color) printf("\033[0m");
                        }

                        printf("%slength:%s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", length, vert_bar);

                        printf("\n");
                        printh_stat_line(print_utf, max_ship_length);
                        printf("\n");
                        for (n_hits = 0; n_hits <= max_ship_length; ++n_hits) {
                                printf("%s%2i hit%c%s%s", print_color ? "\033[1m" : "", n_hits, (n_hits == 1) ? ' ' : 's', print_color ? "\033[0m" : "", vert_bar);
                                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                                        if (n_hits <= length) print_color ? printf("\033[%sm%3i\033[0m%s", (n_hits == length ? "35" : "31"), ship_count[length][n_hits], vert_bar) : printf("%3i%s", ship_count[length][n_hits], vert_bar);
                                        else printf("   %s", vert_bar);
                                }
                                printf("\n");
                        }
                        printh_stat_line(print_utf, max_ship_length);
                        printf("\n%s  sum: %s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", n_ships_total[length], vert_bar);
                        printf("\n");
                }
        } else {
                /* ship_count[<length>][<n_hits>] corresponds to the number of ships of length <length> that have been hit <n_hits> times */
                int ship_count_left[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1];
                int ship_count_right[MAX_SHIP_LENGTH + 1][MAX_SHIP_LENGTH + 1];
                countShipHits(data_left, nx, ny, ship_count_left);
                countShipHits(data_right, nx, ny, ship_count_right);

                /* title: e.g.:     BOT's ships     */
                if (print_color) printf("\033[31;1m");
                printf("\n%*s%*s", (nCharsX + (int)strlen(bot_title)) / 2, bot_title, nCharsX - (nCharsX + (int)strlen(bot_title)) / 2, "");
                print_gap();
                if (print_color) printf("\033[32;1m");
                printf("%*s%*s\n", (nCharsX + (int)strlen(player_title)) / 2, player_title, nCharsX - (nCharsX + (int)strlen(player_title)) / 2, "");
                if (print_color) printf("\033[0m");

                printf("%slength:%s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", length, vert_bar);
                print_gap();
                printf("%slength:%s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) printf("%3i%s", length, vert_bar);

                printf("\n");
                printh_stat_line(print_utf, max_ship_length);
                print_gap();
                printh_stat_line(print_utf, max_ship_length);
                printf("\n");

                for (n_hits = 0; n_hits <= max_ship_length; ++n_hits) {
                        printf("%s%2i hit%c%s%s", print_color ? "\033[1m" : "", n_hits, (n_hits == 1) ? ' ' : 's', print_color ? "\033[0m" : "", vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                                if (n_hits <= length) print_color ? printf("\033[%sm%3i\033[0m%s", (n_hits == length ? "35" : "31"), ship_count_left[length][n_hits], vert_bar) : printf("%3i%s", ship_count_left[length][n_hits], vert_bar);
                                else printf("   %s", vert_bar);
                        }
                        print_gap();
                        printf("%s%2i hit%c%s%s", print_color ? "\033[1m" : "", n_hits, (n_hits == 1) ? ' ' : 's', print_color ? "\033[0m" : "", vert_bar);
                        for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) {
                                if (n_hits <= length) print_color ? printf("\033[%sm%3i\033[0m%s", (n_hits == length ? "35" : "32"), ship_count_right[length][n_hits], vert_bar) : printf("%3i%s", ship_count_right[length][n_hits], vert_bar);
                                else printf("   %s", vert_bar);
                        }
                        printf("\n");
                }
                printh_stat_line(print_utf, max_ship_length);
                print_gap();
                printh_stat_line(print_utf, max_ship_length);

                printf("\n%s  sum: %s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) print_color ? printf("\033[31m%3i\033[0m%s", n_ships_total[length], vert_bar) : printf("%3i%s", n_ships_total[length], vert_bar);
                print_gap();
                printf("%s  sum: %s%s", print_color ? "\033[1m" : "", print_color ? "\033[0m" : "", vert_bar);
                for (length = MIN_SHIP_LENGTH; length <= max_ship_length; ++length) print_color ? printf("\033[32m%3i\033[0m%s", n_ships_total[length], vert_bar) : printf("%3i%s", n_ships_total[length], vert_bar);
                printf("\n\n");
        }
}

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

/* e.g.: stdin: "11e" --> *x = 4, *y = 10
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
                if (isspace(c)) {
                        if (print_color) printf("\033[31;1m");
                        printf("Input Error: Missing coordinate.");
                        if (print_color) printf("\033[0m");
                } else if (c == '-' && tolower((c = getchar())) == 'h') {
                        if (print_color) printf("\033[32;1m");
                        printf("\nInstructions on typing the coordinate:\n");
                        if (print_color) printf("\033[34;1m");
                        printf("Type the x-coordinate as letter(s) and the y-coordinate as digit(s) as shown at the edge of the field (order of coordinates is irrellevant, but without a space inbetween):\n");
                        if (print_color) printf("\033[0;1m");
                        printf(" <coordinate> = ( <letter> [<letter>] <firstDigit> [<digit>] ) | ( <firstDigit> [<digit>] <letter> [<letter>] )\n");
                        printf(" <letter> = 'A' | 'B' | ... | 'Z' | 'a' | 'b' | ... | 'z'\n");
                        printf(" <firstDigit> = '1' | '2' | ... | '9'\n");
                        printf(" <digit> = '0' | <firstDigit>\n\n");
                        if (print_color) printf("\033[32;1m");
                        printf("Instructions on typing the coordinate and direction/length for placing the ships:\n");
                        if (print_color) printf("\033[34;1m");
                        printf("First type the coordinate as when only typing a coordinate, then type a space and then the direction/length.\n");
                        printf("The direction/length consists of a number representing the ship's length and the other character representing the direction in which it should be placed from the coordinate:\n");
                        if (print_color) printf("\033[0m");
                        printf("Note: if you do not type direction/length or only type the direction/length as '1' your ship has length 1 and is oriented east- / rightwards.\n");
                        if (print_color) printf("\033[1m");
                        printf(" <directLength> =  '1' | <direction> <length> | <length> <direction>\n");
                        printf(" <length> = <firstDigit> [<digit>]\n");
                        printf(" <direction> = 'R' | 'r' | 'E' | 'e' | '>'       for signaling the direction east- / rightwards.\n");
                        printf("               'L' | 'l' | 'W' | 'w' | '<'       for signaling the direction west- / leftwards.\n");
                        printf("               'U' | 'u' | 'E' | 'n' | '^'       for signaling the direction north- / upwards.\n");
                        printf("               'D' | 'd' | 'S' | 's' | 'V' | 'v' for signaling the direction south- / downwards.\n\n");
                        if (print_color) printf("\033[36;1m");
                        printf("Type '/' to exit.\n\n");
                        if (print_color) printf("\033[0m");
                } else {
                        if (print_color) printf("\033[31;1m");
                        printf("Input Error: Unknown coordinate character: '%c' (coordinates must only consist of letters and digits).\n", c);
                        if (print_color) printf("\033[0m");
                }
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
                                        if (print_color) printf("\033[31;1m");
                                        printf("Input Error: character: '%c' is not supposed to be a digit.\n", c);
                                        if (print_color) printf("\033[0m");
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
                                        if (print_color) printf("\033[31;1m");
                                        printf("Input Error: character: '%c' is not supposed to be a letter.\n", c);
                                        if (print_color) printf("\033[0m");
                                        flush_buff();
                                        return INPUT_ERROR;
                        }
                } else {
                        if (print_color) printf("\033[31;1m");
                        printf("Input Error: Unknown coordinate character: '%c' (coordinates must only be made of letters and digits).\n", c);
                        if (print_color) printf("\033[0m");
                        flush_buff();
                        return INPUT_ERROR;
                }
        }

        if (c == EOF) {
                return BUFFER_ERROR;
        }

        /* makes sure x_hold, y_hold are within the field */
        if (0 >= x_hold || x_hold > nx || 0 >= y_hold || y_hold > ny) {
                if (print_color) printf("\033[31;1m");
                if (0 >= x_hold) {
                        printf("Input Error: Your x-coordinate (letters) is too small to fit on the field or your x-coordinate is missing\n");
                } else if (x_hold > nx) {
                        printf("Input Error: Your x-coordinate (letters) is too large to fit on the field\n");
                } else if (0 >= y_hold) {
                        printf("Input Error: Your y-coordinate (digits) is too small to fit on the field or your y-coordinate is missing\n");
                } else {
                        printf("Input Error: Your y-coordinate (digits) is too large to fit on the field\n");
                }
                if (print_color) printf("\033[0m");
                if (c != '\n') {
                        flush_buff();
                }
                return INPUT_ERROR;
        }
        *x = x_hold - 1;
        *y = y_hold - 1;
        return (c == '\n') ? SUCCESS_ENDL : SUCCESS;
}

/* returns direction passes length on through the pointer or returns negative number in case of error
   NOTE: this function flushes the line even on a correct input */
direction_t scan_direction(int *length, int max_ship_length, int print_color)
{
        int direction = -1;
        int length_hold = 0;
        char c;

        c = toupper(getchar()); /* toupper removes case sensitivity */
        if (c == EOF) {
                return BUFFER_ERROR_DIRECTION;
        }

        if (isdigit(c)) {
                length_hold = c - '0';
                c = toupper(getchar());
                if (c == EOF) {
                        return BUFFER_ERROR_DIRECTION;
                } else if (isdigit(c)) {
                        length_hold *= 10;
                        length_hold += c - '0';
                        c = toupper(getchar());
                }
                if (length_hold < MIN_SHIP_LENGTH || max_ship_length < length_hold) {
                        if (print_color) printf("\033[31;1m");
                        if (length_hold < MIN_SHIP_LENGTH) {
                                printf("Input Error: Your ship length '%i' is too small\n", length_hold);
                        } else {
                                printf("Input Error: Your ship length '%i' is too large\n", length_hold);
                        }
                        if (print_color) printf("\033[0m");
                        if (c != '\n') {
                                flush_buff();
                        }
                        return INPUT_ERROR_DIRECTION;
                }
        }
        if (length_hold == 1) {
                *length = length_hold;
                if (c != '\n') flush_buff();
                return RIGHT;
        }
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
                        if (print_color) printf("\033[31;1m");
                        if (c == '\n') printf("Input Error: Missing direction.\n");
                        else printf("Input Error: Invalid direction: '%c'.\n", c);
                        if (print_color) printf("\033[0;1m");
                        printf("Use: 'R' | 'r' | 'E' | 'e' | '>'       for signaling the direction east- / rightwards.\n");
                        printf("     'L' | 'l' | 'W' | 'w' | '<'       for signaling the direction west- / leftwards.\n");
                        printf("     'U' | 'u' | 'E' | 'n' | '^'       for signaling the direction north- / upwards.\n");
                        printf("     'D' | 'd' | 'S' | 's' | 'V' | 'v' for signaling the direction south- / downwards.\n");
                        if (print_color) printf("\033[0m");
                        if (c != '\n') flush_buff();
                        return INPUT_ERROR_DIRECTION;
        }
        if (!length_hold && isdigit(c = toupper(getchar()))) {
                c = toupper(getchar());
                length_hold = c - '0';
                if (isdigit(c)) {
                        c = toupper(getchar());
                        length_hold *= 10;
                        length_hold += c - '0';
                }
                if (length_hold < MIN_SHIP_LENGTH || max_ship_length < length_hold) {
                        if (print_color) printf("\033[31;1m");
                        if (length_hold < MIN_SHIP_LENGTH) {
                                printf("Input Error: Your ship length '%i' is too small\n", length_hold);
                        } else {
                                printf("Input Error: Your ship length '%i' is too large\n", length_hold);
                        }
                        if (print_color) printf("\033[0m");
                        if (c != '\n') flush_buff();
                        return INPUT_ERROR_DIRECTION;
                }
        }

        if (direction == -1 || !length_hold) {
                if (print_color) printf("\033[31;1m");
                if (direction == -1) {
                        printf("Input Error: The direction of your ship is missing\n");
                } else {
                        printf("Input Error: Your ship length is too large / too small / non existent\n");
                }
                if (print_color) printf("\033[0m");
                if (c != '\n') flush_buff();
                return INPUT_ERROR_DIRECTION;
        }
        *length = length_hold;

        flush_buff();
        return direction;
}

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
                        if (print_color) printf("\033[1m");
                        printf("You have ");
                        for (i = max_ship_length; i >= MIN_SHIP_LENGTH; --i) {
                                if (n_ships_remaining[i]) {
                                        printf("%i battle ship%s (length %i) ", n_ships_remaining[i], n_ships_remaining[i] == 1 ? "" : "s", i);
                                }
                        }
                        printf("remaining\n");
                        if (print_color) printf("\033[0m");
                } else {
                        printField = TRUE;
                }
                if (print_color) printf("\033[1m");
                printf("Choose where to place your ships, by typing the start coordinate and the direction, length of your ship.\n");
                if (print_color) printf("\033[0m  e.g. '\033[31mA\033[32m1 \033[33m2\033[34ms\033[0m' a ship at coordinate (\033[31mA\033[0m, \033[32m1\033[0m) with length \033[33m2\033[0m and facing down- / \033[34ms\033[0mouthwards (for further information type '\033[35m-h\033[0m')\n");
                else printf("  e.g. 'A1 2s' a ship at coordinate (A, 1) with length 2 and facing down- / southwards (for further information type '-h')\n");

                /* scan coordinate and direction, where ship should be placed */
                status = scan_coordinate(&x, &y, nx, ny, print_color);
                switch (status) {
                        case SUCCESS_ENDL:
                                direction = RIGHT;
                                length = 1;
                                break;
                        case SUCCESS:
                                direction = scan_direction(&length, max_ship_length, print_color);
                                if (direction == BUFFER_ERROR_DIRECTION) {
                                        printf("BUFFER ERROR - %s line %i\n", __FILE__, __LINE__);
                                        return BUFFER_ERROR;
                                } else if (direction == INPUT_ERROR_DIRECTION) {
                                        printField = FALSE;
                                        continue;
                                }
                                break;
                        case EXIT:
                                return EXIT;
                        case BUFFER_ERROR:
                                printf("BUFFER ERROR - %s line %i\n", __FILE__, __LINE__);
                                return BUFFER_ERROR;
                        case INPUT_ERROR: default:
                                printField = FALSE;
                                continue;
                }

                /* check if there are ships of the selected length left */
                if (length > max_ship_length || n_ships_remaining[length] <= 0) {
                        if (print_color) printf("\033[31;1m");
                        printf("You don't have any ship with the length %i left to place.\n", length);
                        if (print_color) printf("\033[0m");
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
                        if (print_color) printf("\033[31;1m");
                        printf("You placed your ship, where it would collide with a wall or touch another ship\n");
                        if (print_color) printf("\033[0m");
                        printField = FALSE;
                        continue;
                }

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
                                                printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                                                return UNKNOWN_ERROR;
                                }
                        }
                }
                /* decrement the number of length-long ships remaining */
                --n_ships_remaining[length];
        }
        return SUCCESS;
}

status_t auto_choose_ships(const int nx, const int ny, shipstate_t **data, int *n_ships_remaining, int max_ship_length)
{

        int length;
        /* number of times the field was reset, because of ship-jamming */
        int numOfReTries = 0;
        /* n_ships_remaining_cpy[i] is the number of i long ships left */
        int n_ships_remaining_cpy[MAX_SHIP_LENGTH + 1];
        /* using "nRemainingShips" as scratch space */
        memcpy(n_ships_remaining_cpy, n_ships_remaining, sizeof(n_ships_remaining_cpy));

        /* populate with ships - place longest ships first, while there still is enough room */
        for (length = max_ship_length; length >= MIN_SHIP_LENGTH; --length) {
                int numOfTries = 0;
                while (n_ships_remaining_cpy[length]) {
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
                                        return INPUT_ERROR;
                                }
                                /* reset variables */
                                length = max_ship_length + 1;
                                memcpy(n_ships_remaining_cpy, n_ships_remaining, sizeof(n_ships_remaining_cpy));
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

                        if (length == 1) {
                                data[y][x] = UNHIT_SINGLE;
                        } else {
                                /* places the ship onto the the boxes, by changing their state to UNHIT */
                                for (i = 0; i < length; ++i) {
                                        /* ships can only be RIGHT (= 0) or DOWN (= 1) */
                                        if (direction == DOWN) {
                                                /* direction = DOWN */
                                                data[y + i][x] = ((i == 0) ? UNHIT_VERT_TOP : ((i == length - 1) ? UNHIT_VERT_BOTTOM : UNHIT_VERT));
                                        } else {
                                                /* direction = RIGHT */
                                                data[y][x + i] = ((i == 0) ? UNHIT_HORIZ_LEFT : ((i == length - 1) ? UNHIT_HORIZ_RIGHT : UNHIT_HORIZ));
                                        }
                                }
                        }
                        /* decrements the number of ships left of the length "length" */
                        --n_ships_remaining_cpy[length];
                }
        }
        return SUCCESS;
}

int has_somemone_won(int *n_ships_remaining, int max_ship_length)
{
        int shipLen;
        for (shipLen = MIN_SHIP_LENGTH; shipLen <= max_ship_length; ++shipLen) {
                if (n_ships_remaining[shipLen] != 0)
                        return FALSE;
        }
        return TRUE;
}

int test_ship_status(int nx, int ny, int n_ships_remaining[], shipstate_t **battle_field, point_t position)
{
        int x = position.x;
        int y = position.y;
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
                        battle_field[y][i] += SUNK_HORIZ - HIT_HORIZ;
                }
                --n_ships_remaining[i - x];
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
                        battle_field[i][x] += SUNK_VERT - HIT_VERT;
                }
                --n_ships_remaining[i - y];
                return TRUE;
        } else if (is_single(battle_field[y][x])) {
                battle_field[y][x] += SUNK_VERT - HIT_VERT;
                --n_ships_remaining[1];
                return TRUE;
        } else {
                printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                return FALSE;
        }
}

status_t player_shoot(play_fields_t *fld)
{
        const int nx = fld->nx;
        const int ny = fld->ny;
        const int print_color = fld->print_color;
        shipstate_t **data_left = fld->data_left;
        int *n_ships_remaining_left = fld->n_ships_remaining_left;

        int x, y;
        point_t point;
        status_t status;

        if (print_color) printf("\033[1m");
        printf("Type the coordinates, where you want to shoot at (for further information type '-h').\n");
        if (print_color) printf("\033[0m");
        while ((status = scan_coordinate(&x, &y, nx, ny, print_color)) == INPUT_ERROR || (status >= SUCCESS && has_been_shot(data_left[y][x])))  {
                if (status == SUCCESS) {
                        flush_buff();
                }
                if (status >= SUCCESS && has_been_shot(data_left[y][x])) {
                        if (print_color) printf("\033[31;1m");
                        printf("Don't waste your shots: this box has already been shot at by you.\n");
                        if (print_color) printf("\033[0m");
                }
                if (print_color) printf("\033[1m");
                printf("Retype the coordinates, where you want to shoot at (for further information type '-h').\n");
                if (print_color) printf("\033[0m");
        }
        if (status == BUFFER_ERROR) {
                return BUFFER_ERROR;
        }
        if (status == EXIT) {
                return EXIT;
        }

        point.x = x;
        point.y = y;
        if (data_left[y][x] == NONE) {
                data_left[y][x] = SPLASH_INVERT;
                return SUCCESS;
        } else if (is_unhit(data_left[y][x])) {
                data_left[y][x] += (HIT_HORIZ - UNHIT_HORIZ);
                data_left[y][x] = invert(data_left[y][x]);
                test_ship_status(nx, ny, n_ships_remaining_left, data_left, point);
                return SUCCESS_HIT;
        } else {
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
        int *n_ships_remaining_right = fld->n_ships_remaining_right;

        int x, y, hitship;
        point_t point;

        do {
                /* determine whether to hit a ship */
                hitship = (rand() % 100) < hit_rate;

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


                point.x = x;
                point.y = y;
                if (data_right[y][x] == NONE) {
                        data_right[y][x] = SPLASH_INVERT;
                } else if (is_unhit(data_right[y][x])) {
                        data_right[y][x] += (HIT_HORIZ - UNHIT_HORIZ);
                        data_right[y][x] = invert(data_right[y][x]);
                        test_ship_status(nx, ny, n_ships_remaining_right, data_right, point);
                } else {
                        printf("ERROR: Should not occur - %s line %i\n", __FILE__, __LINE__);
                        return;
                }

                if (fld->print_color) printf("\033[1m");
                printf("Bot shot at: %c%c%i\n", (x < 26 ? ' ' : (x / 26 - 1) + 'A'), ((x % 26) + 'A'), y);
                if (fld->print_color) printf("\033[0m");
        } while (is_ship(data_right[y][x]) && !has_somemone_won(n_ships_remaining_right, fld->max_ship_length));

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

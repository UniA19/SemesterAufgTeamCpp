#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

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

static const char *const rowFormats[];
enum shipstate;
enum direction;
static shipstate_t ** alloc_field(int nx, int ny);
static void free_field(play_fields_t *fld);
int flush_buff();
int main(int argc, char *argv[]);

void print_hline(int nx);
void print_top_row(int nx);
void print_row(const shipstate_t row[], int nx, int row_num, int is_left_field);
void print_field(play_fields_t *fld);

int scan_coordinate(int *x, int *y, int nx, int ny);
direction_t scan_direction(int *length);

int choose_ships(play_fields_t *fld);
void bot_choose_ships(play_fields_t *fld);
int test_ship_status(shipstate_t **battle_field, int nShipsRemaining[], int x, int y, int nx, int ny);
void player_shoot(play_fields_t *fld);

#endif
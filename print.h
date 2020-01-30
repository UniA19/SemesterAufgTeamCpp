#ifndef PRINT_H
#define PRINT_H

#include "global.h"

/* prints both entire battleship-fields */
void print_field(play_fields_t *fld);

/* prints the statistics at the end of the game */
void print_stats(play_fields_t *fld, const int n_ships_total[]);

#endif

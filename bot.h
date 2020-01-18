#ifndef BOT_H
#define BOT_H

#include "global.h"


status_t auto_choose_ships(const int nx, const int ny, shipstate_t **data, int *n_ships_remaining, int max_ship_length);

int has_somemone_won(int *n_ships_remaining, int max_ship_length);

int test_ship_status(int nx, int ny, int n_ships_remaining[], shipstate_t **battle_field, point_t position);

void bot_shoot(play_fields_t *fld, int hit_rate);

#endif

#ifndef BOT_H
#define BOT_H

#include "global.h"


/* automatically choses ships for either the Player or the Bot */
status_t auto_choose_ships(const int nx, const int ny, const int *n_ships_remaining, const int max_ship_length, shipstate_t **battle_field);

/* determines if the player/bot has won, based on if there are any ships remaining
        Note: *n_ships_remaining is from the player/bot who is currently being checked */
int has_somemone_won(int *n_ships_remaining, const int max_ship_length);

/* tests to see if the all boxes of the ship at position have been hit
        if that is the case is changes those boxes to "sunk" and return TRUE
        otherwise returns FALSE, -1 on errors */
int test_ship_status(const int nx, const int ny, int n_ships_remaining[], shipstate_t **battle_field, const point_t position);

/* handles the Bot's turn */
void bot_shoot(play_fields_t *fld, const int difficulty);

#endif

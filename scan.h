#ifndef SCAN_H
#define SCAN_H

#include "global.h"

/* Handles the Player's personal choice of ships */
status_t choose_ships(play_fields_t *fld);

/* Handles the Player's shot on the Bot's ships */
status_t player_shoot(play_fields_t *fld);

#endif

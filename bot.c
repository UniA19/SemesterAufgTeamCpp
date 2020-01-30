#include "bot.h"

/* automatically choses ships for the Player or the Bot */
status_t auto_choose_ships(const int nx, const int ny, const int *n_ships_remaining, const int max_ship_length, shipstate_t **battle_field)
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
                /* number of tries to set a ship */
                int numOfTries = 0;
                /* set all ship of length "length" */
                while (n_ships_remaining_cpy[length]) {
                        int i;
                        /* ships can only be set RIGHT (= 0) or DOWN (= 1) */
                        const direction_t direction = rand() % 2;
                        /* change the possible coordinates, so that the ship fully on the field */
                        const int x = (direction == DOWN) ? rand() % nx : rand() % (nx - (length - 1));
                        const int y = (direction == DOWN) ? rand() % (ny - (length - 1)) : rand() % ny;
                        /* variable containing the whether there are any ships in the way */
                        int is_free = TRUE;
                        ++numOfTries;
                        /* If there have been enough failed tries to set a ship the ships are cleared and the algorithm tries again */
                        if (numOfTries > 1000) {
                                int  y_iter;
                                /* If there have been enough failed attempts at setting the ships the algorithm gives up */
                                if (numOfReTries > 1000) {
                                        printf("\nNo legal postions for the Bot's ships found.\n");
                                        printf("Increase the value of x or y coordinate, so that there is enough room for the ships.\n\n");
                                        return INPUT_ERROR;
                                }
                                /* reset variables */
                                length = max_ship_length + 1;
                                memcpy(n_ships_remaining_cpy, n_ships_remaining, sizeof(n_ships_remaining_cpy));
                                for (y_iter = 0; y_iter < ny; ++y_iter) {
                                        int x_iter;
                                        for (x_iter = 0; x_iter < nx; ++x_iter) {
                                                battle_field[y_iter][x_iter] = NONE;
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
                                                if (0 <= (y_h + k) && (y_h + k) < ny && 0 <= (x_h + j) && (x_h + j) < nx && is_unhit(battle_field[y_h + k][x_h + j])) {
                                                        is_free = FALSE;
                                                }
                                        }
                                }
                        }
                        /* skip placing the ship, if the position was recognized to be illegal */
                        if (!is_free) continue;

                        /* ship placement of a ship with length 1 */
                        if (length == 1) {
                                battle_field[y][x] = UNHIT_SINGLE;
                        /* ship placement of a longer ships */
                        } else {
                                /* places the ship onto the the boxes, by changing their state to UNHIT */
                                for (i = 0; i < length; ++i) {
                                        /* ships can only be RIGHT (= 0) or DOWN (= 1) */
                                        if (direction == DOWN) {
                                                /* direction = DOWN */
                                                battle_field[y + i][x] = ((i == 0) ? UNHIT_VERT_TOP : ((i == length - 1) ? UNHIT_VERT_BOTTOM : UNHIT_VERT));
                                        } else {
                                                /* direction = RIGHT */
                                                battle_field[y][x + i] = ((i == 0) ? UNHIT_HORIZ_LEFT : ((i == length - 1) ? UNHIT_HORIZ_RIGHT : UNHIT_HORIZ));
                                        }
                                }
                        }
                        /* decrements the number of ships left of the length "length" */
                        --n_ships_remaining_cpy[length];
                }
        }
        return SUCCESS;
}

/* determines if the player/bot has won, based on if there are any ships remaining
        Note: *n_ships_remaining is from the player/bot who is currently being checked */
int has_somemone_won(int *n_ships_remaining, const int max_ship_length)
{
        int shipLen;
        /* Iterate through the number of ships remaining to see if all are zero and the player/bot has won */
        for (shipLen = MIN_SHIP_LENGTH; shipLen <= max_ship_length; ++shipLen) {
                if (n_ships_remaining[shipLen] != 0)
                        return FALSE;
        }
        return TRUE;
}

/* tests to see if the all boxes of the ship at position have been hit
        if that is the case is changes those boxes to "sunk" and return TRUE
        otherwise returns FALSE, -1 on errors */
int test_ship_status(const int nx, const int ny, int n_ships_remaining[], shipstate_t **battle_field, const point_t position)
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
                        /* as soon as there is an is a not hit box, the funtion terminates, by returning FALSE */
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
                        /* as soon as there is an is a not hit box, the funtion terminates, by returning FALSE */
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
                /* if the position is a single ship and it is hit, it must be sunk */
                if (is_hit(battle_field[y][x])) {
                        battle_field[y][x] += SUNK_VERT - HIT_VERT;
                        --n_ships_remaining[1];
                        return TRUE;
                } else {
                        /* x, y are from position, which is the position, that has been shot at */
                        printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                        return FALSE;
                }
        } else {
                printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                return FALSE;
        }
}

/* handles the Bot's turn */
void bot_shoot(play_fields_t *fld, const int difficulty)
{
        /* copies of the struct variables */
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **field_right = fld->field_right;
        int *n_ships_remaining_right = fld->n_ships_remaining_right;

        int x, y;
        point_t point;

        do {
                /* determine whether or not to hit a ship based on the difficulty/hit_rate */
                int hitship = (rand() % 100) < difficulty;

                /* generate random unhit position, to shoot at */
                do {
                        x = rand() % nx;
                        y = rand() % ny;
                } while (hitship ? !(is_unhit(field_right[y][x])) : (field_right[y][x] != NONE));

                point.x = x;
                point.y = y;
                if (field_right[y][x] == NONE) {
                        /* if the Bot hit water the box is changed to splash invert, whereby the invert signals a possible color inversion on the printout */
                        field_right[y][x] = SPLASH_INVERT;
                } else if (is_unhit(field_right[y][x])) {
                        /* if the bot hit an unhit box the box is changed to the corresponding hit, whereby the invert signals a possible color inversion on the printout */
                        field_right[y][x] += (HIT_HORIZ - UNHIT_HORIZ);
                        field_right[y][x] = invert(field_right[y][x]);
                        /* test for a possibly sunk ship at that location */
                        test_ship_status(nx, ny, n_ships_remaining_right, field_right, point);
                } else {
                        printf("ERROR: Should not occur - %s line %i\n", __FILE__, __LINE__);
                        return;
                }

                /* printout of where the Bot shot*/
                print_bold(fld->print_color);
                printf("Bot shot at: %c%c%i\n", (x < 26 ? ' ' : (x / 26 - 1) + 'A'), ((x % 26) + 'A'), y);
                print_nocolor(fld->print_color);
        } while (is_ship(field_right[y][x]) && !has_somemone_won(n_ships_remaining_right, fld->max_ship_length));
}

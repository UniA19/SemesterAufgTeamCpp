#include "bot.h"

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
                                for (i = 0; i < ny; ++i) {
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

void bot_shoot(play_fields_t *fld, int hit_rate)
{
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_right = fld->data_right;
        int *n_ships_remaining_right = fld->n_ships_remaining_right;

        int x, y, hitship;
        point_t point;

        do {
                /* determine whether to hit a ship */
                hitship = (rand() % 100) < hit_rate;

                do {
                        x = rand() % nx;
                        y = rand() % ny;
                } while (hitship ? !(is_unhit(data_right[y][x])) : (data_right[y][x] != NONE));

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
}

#include "input.h"
#include "main.c"

int choose_ships(play_fields_t *fld)
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_right = fld->data_right;
        /* nShipsRemaining[i] is the number of i long ships left */
        int nShipsRemaining[MAX_SHIP_LENGTH + 1];
        /* if the last user-input had an INPUT-ERROR the field should not be reprinted */
        int printField = TRUE;
        /* using "nRemainingShips" as scratch space */
        memcpy(nShipsRemaining, fld->nShipsRemaining_right, sizeof(nShipsRemaining));

        /* loop-condition checks whether there is still ships, that have not been set */
        while (1) {
                int i, x, y, status, length;
                direction_t direction;
                /* variable containing the whether there are any ships in the way */
                int is_free = TRUE;

                {
                        /* scan to see if any ships are still remaining (not sunk) */
                        int anyShips = FALSE;
                        for (i = MIN_SHIP_LENGTH; i <= MAX_SHIP_LENGTH && !anyShips; ++i) {
                                anyShips = nShipsRemaining[i];
                        }
                        if (!anyShips) {
                                break;
                        }
                }
                if (printField) {
                        print_field(fld);
                        printf("You have ");
                        for (i = MAX_SHIP_LENGTH; i >= MIN_SHIP_LENGTH; --i) {
                                if (nShipsRemaining[i]) {
                                        printf("%i battle ship%s (length %i) ", nShipsRemaining[i], nShipsRemaining[i] == 1 ? "" : "s", i);
                                }
                        }
                        printf(" remaining\n");
                } else {
                        printField = TRUE;
                }
                printf("Choose where to place your ships, by typing the start and end coordinate of each ship.\n");
                /* scan coordinate and direction, where ship should be placed */
                if ((status = scan_coordinate(&x, &y, nx, ny)) < 0 || (direction = scan_direction(&length)) < 0) {
                        if (status == BUFFER_ERROR || direction == BUFFER_ERROR) {
                                printf("BUFFER ERROR - %s line %i\n", __FILE__, __LINE__);
                                return BUFFER_ERROR;
                        }
                        flush_buff();
                        printField = FALSE;
                        continue;
                }
                flush_buff();

                /* check if there are ships of the selected length left */
                if (length > MAX_SHIP_LENGTH || nShipsRemaining[length] <= 0) {
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
                                        return INPUT_ERROR;
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
                        printField = FALSE;
                        continue;
                }

                /* place the ship onto the the boxes, by changing their state to UNHIT */
                for (i = 0; i < length; ++i) {
                        /* in-/decrease one coordinate by i based on the direction */
                        switch (direction) {
                                case RIGHT:
                                        data_right[y][x + i] = UNHIT_HORIZ;
                                        break;
                                case DOWN:
                                        data_right[y + i][x] = UNHIT_VERT;
                                        break;
                                case LEFT:
                                        data_right[y][x - i] = UNHIT_HORIZ;
                                        break;
                                case UP:
                                        data_right[y - i][x] = UNHIT_VERT;
                                        break;
                                default:
                                        printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                                        return INPUT_ERROR;
                        }
                }
                /* decrement the number of length-long ships remaining */
                --nShipsRemaining[length];
        }
        return SUCCESS;
}

void bot_choose_ships(play_fields_t *fld)
{
        /* copies of the struct values */
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_left = fld->data_left;

        int length;
        /* nShipsRemaining[i] is the number of i long ships left */
        int nShipsRemaining[MAX_SHIP_LENGTH + 1];
        /* using "nRemainingShips" as scratch space */
        memcpy(nShipsRemaining, fld->nShipsRemaining_left, sizeof(nShipsRemaining));

        /* populate with ships - place longest ships first, while there still is enough room */
        for (length = MAX_SHIP_LENGTH; length >= MIN_SHIP_LENGTH; --length) {
                int numOfTries = 0;
                while (nShipsRemaining[length]) {
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
                                /* reset variables */
                                length = MAX_SHIP_LENGTH + 1;
                                memcpy(nShipsRemaining, fld->nShipsRemaining_left, sizeof(nShipsRemaining));
                                for (i = 0; i < nx; ++i) {
                                        int j;
                                        for (j = 0; j < nx; ++j) {
                                                data_left[i][j] = NONE;
                                        }
                                }
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
                                                if (0 <= (y_h + k) && (y_h + k) < ny && 0 <= (x_h + j) && (x_h + j) < nx && is_unhit(data_left[y_h + k][x_h + j])) {
                                                        is_free = FALSE;
                                                }
                                        }
                                }
                        }
                        if (!is_free) continue;

                        /* places the ship onto the the boxes, by changing their state to UNHIT */
                        for (i = 0; i < length; ++i) {
                                /* ships can only be RIGHT (= 0) or DOWN (= 1) */
                                if (direction == DOWN) {
                                        /* direction = DOWN */
                                        data_left[y + i][x] = UNHIT_VERT;
                                } else {
                                        /* direction = RIGHT */
                                        data_left[y][x + i] = UNHIT_HORIZ;
                                }
                        }
                        /* decrements the number of ships left of the length "length" */
                        --nShipsRemaining[length];
                }
        }
}


int test_ship_status(shipstate_t **battle_field, int nShipsRemaining[], int x, int y, int nx, int ny)
{
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
                        battle_field[y][i] = SUNK_HORIZ;
                }
                --nShipsRemaining[x - i];
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
                        battle_field[i][x] = SUNK_VERT;
                }
                --nShipsRemaining[y - i];
                putchar('Y');
                return TRUE;
        } else {
                printf("ERROR: Should not be reached - %s line %i\n", __FILE__, __LINE__);
                return FALSE;
        }
}

void player_shoot(play_fields_t *fld)
{
        const int nx = fld->nx;
        const int ny = fld->ny;
        shipstate_t **data_left = fld->data_left;
        int *nShipsRemaining_left = fld->nShipsRemaining_left;

        int x, y;
        int status;

        printf("Type the coordinates, where you want to shoot at.\n");
        while((status = scan_coordinate(&x, &y, nx, ny)) == INPUT_ERROR || has_been_shot(data_left[y][x]))  {
                printf("Error: Retype the coordinates, where you want to shoot at.\n");
        }

        switch (data_left[y][x]) {
                case NONE:
                        data_left[y][x] = SPLASH;
                        break;
                case UNHIT_VERT:
                        data_left[y][x] = HIT_VERT;
                        test_ship_status(data_left, nShipsRemaining_left, x, y, nx, ny);
                        break;
                case UNHIT_HORIZ:
                        data_left[y][x] = HIT_HORIZ;
                        test_ship_status(data_left, nShipsRemaining_left, x, y, nx, ny);
                        break;
                default:
                        printf("ERROR: Should not occur - %s line %i\n", __FILE__, __LINE__);
                        return;
        }
}
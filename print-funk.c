#include "input.h"


/* prints horizontal line, deviding the rows (--+---+---+...) */
void print_hline(int nx)
{
        int coli;

        printf("--+");
        for (coli = 0; coli < nx; ++coli) {
                printf("---+");
        }
}

/* prints top row (  | A | B | C | ... ) */
void print_top_row(int nx)
{
        int coli;

        printf("  |");
        /* format as "AA" etc if there are more than 26 cols */
        for (coli = 0; coli < nx; ++coli) {
                printf("%c%c |", (coli < 26 ? ' ' : (coli / 26 - 1) + 'A'), ((coli % 26) + 'A'));
        }
}

/* prints row including the numbering at the beginning of the line ( 1|~~~|XXX|~~~|...) */
void print_row(const shipstate_t row[], int nx, int row_num, int is_left_field)
{
        int i;

        printf("%2i|", row_num);

        for (i = 0; i < nx; ++i) {
                int state = row[i];
                if (is_left_field && is_unhit(state)) {
#ifndef SHOW_BOTS_SHIP
                       state = NONE; /* keep hidden */
#endif
                }
                printf(rowFormats[state]);
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

        int row;

        /* top row:   | A | B |...          | A | B |... */
        print_top_row(nx);
        print_gap();
        print_top_row(nx);
        printf("\n");
        for (row = 0; row < ny; ++row) {
                /* horizontal line: --+---+---+...        --+---+---+... */
                print_hline(nx);
                print_gap();
                print_hline(nx);
                printf("\n");
                /* other rows:   <row>|~~~|XXX|...          |   |XXX|... */
                print_row(data_left[row], nx, row + 1, 0);
                print_gap();
                print_row(data_right[row], nx, row + 1, 0);
                printf("\n");
        }
        /* horizontal line: --+---+---+...        --+---+---+... */
        print_hline(nx);
        print_gap();
        print_hline(nx);
        printf("\n");
}
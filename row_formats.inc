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
        "◀●═│",         /* 3: left end of horizontal, unhit ship on the right field */
        "═●▶│",         /* 4: right end of horizontal, unhit ship on the right field */
        "│●││",         /* 5: vertical, unhit ship on the right field */
        "╱●╲│",        /* 6: top end of vertical, unhit ship on the right field */
        "╲●╱│",        /* 7: bottom end of vertical, unhit ship on the right field */
        "❰●❱│",         /* 8: unhit ship on the right field of the length 1 */
        " ╳ │",         /* 9: hit ship (on left side) */
        "═╳═│",         /* 10: horizontal, hit ship */
        "◀╳═│",         /* 11: left end of horizontal, hit ship */
        "═╳▶│",         /* 12: right end of horizontal, hit ship */
        "│╳││",         /* 13: vertical, hit ship */
        "╱╳╲│",        /* 14: top end of vertical, hit ship */
        "╲╳╱│",        /* 15: bottom end of vertical, hit ship */
        "❰╳❱│",         /* 16: hit ship of the length 1*/
        "╳╪╳│",         /* 17: horizontal, sunken ship */
        "◀╪╳│",         /* 18: left end of horizontal, sunken ship */
        "╳╪▶│",         /* 19: right end of horizontal, sunken ship */
        "╳╫╳│",         /* 20: vertical, sunken ship */
        "╱╫╲│",        /* 21: top end of vertical, sunken ship */
        "╲╫╱│",        /* 22: bottom end of vertical, sunken ship */
        "❰┼❱│",         /* 23: sunken ship of the lenght 1 */
        "   │",         /* 24: unhit - water NOTE: after this point the fields are inverted, which doesn't work without ansi coloring so there is no change here */
        "≈≈≈│",         /* 25: hit water */
        "═●═│",         /* 26: horizontal, unhit ship on the right field */
        "◀●═│",         /* 27: left end of horizontal, unhit ship on the right field */
        "═●>│",         /* 28: right end of horizontal, unhit ship on the right field */
        "│●││",         /* 29: vertical, unhit ship on the right field */
        "╱●╲│",        /* 30: top end of vertical, unhit ship on the right field */
        "╲●╱│",        /* 31: bottom end of vertical, unhit ship on the right field */
        "❰●❱│",         /* 32: unhit ship on the right field of the length 1 */
        " ╳ │",         /* 33: hit ship (on left side) */
        "═╳═│",         /* 34: horizontal, hit ship */
        "◀╳═│",         /* 35: left end of horizontal, hit ship */
        "═╳>│",         /* 36: right end of horizontal, hit ship */
        "│╳││",         /* 37: vertical, hit ship */
        "╱╳╲│",        /* 38: top end of vertical, hit ship */
        "╲╳╱│",        /* 39: bottom end of vertical, hit ship */
        "❰╳❱│",         /* 40: hit ship of the length 1*/
        "╳╪╳│",         /* 41: horizontal, sunken ship */
        "◀╪╳│",         /* 42: left end of horizontal, sunken ship */
        "╳╪▶│",         /* 43: right end of horizontal, sunken ship */
        "╳╫╳│",         /* 44: vertical, sunken ship */
        "╱╫╲│",        /* 45: top end of vertical, sunken ship */
        "╲╫╱│",        /* 46: bottom end of vertical, sunken ship */
        "❰┼❱│"         /* 47: sunken ship of the lenght 1 */
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
        "\033[32;1m◀●═\033[0m│",        /* 3: left end of horizontal, unhit ship on the right field */
        "\033[32;1m═●▶\033[0m│",        /* 4: right end of horizontal, unhit ship on the right field */
        "\033[32;1m│●│\033[0m│",        /* 5: vertical, unhit ship on the right field */
        "\033[32;1m╱●╲\033[0m│",       /* 6: top end of vertical, unhit ship on the right field */
        "\033[32;1m╲●╱\033[0m│",       /* 7: bottom end of vertical, unhit ship on the right field */
        "\033[32;1m❰●❱\033[0m│",        /* 8: unhit ship on the right field of the length 1 */
        "\033[31;1m ╳ \033[0m│",        /* 9: hit ship (on left side) */
        "\033[31;1m═╳═\033[0m│",        /* 10: horizontal, hit ship */
        "\033[31;1m◀╳═\033[0m│",        /* 11: left end of horizontal, hit ship */
        "\033[31;1m═╳▶\033[0m│",        /* 12: right end of horizontal, hit ship */
        "\033[31;1m│╳│\033[0m│",        /* 13: vertical, hit ship */
        "\033[31;1m╱╳╲\033[0m│",       /* 14: top end of vertical, hit ship */
        "\033[31;1m╲╳╱\033[0m│",       /* 15: bottom end of vertical, hit ship */
        "\033[31;1m❰╳❱\033[0m│",        /* 16: hit ship of the length 1*/
        "\033[35;1m╳╪╳\033[0m│",        /* 17: horizontal, sunken ship */
        "\033[35;1m◀╪╳\033[0m│",        /* 18: left end of horizontal, sunken ship */
        "\033[35;1m╳╪▶\033[0m│",        /* 19: right end of horizontal, sunken ship */
        "\033[35;1m╳╫╳\033[0m│",        /* 20: vertical, sunken ship */
        "\033[35;1m╱╫╲\033[0m│",       /* 21: top end of vertical, sunken ship */
        "\033[35;1m╲╫╱\033[0m│",       /* 22: bottom end of vertical, sunken ship */
        "\033[35;1m❰┼❱\033[0m│",        /* 23: sunken ship of the lenght 1 */
        "   │",                         /* 24: unhit - water NOTE: after this point the fields are inverted, which doesn't work without ansi coloring so there is no change here */
        "\033[34;1;7m≈≈≈\033[0m│",     /* 25: hit water */
        "\033[32;1;7m═●═\033[0m│",     /* 26: horizontal, unhit ship on the right field */
        "\033[32;1;7m◀●═\033[0m│",     /* 27: left end of horizontal, unhit ship on the right field */
        "\033[32;1;7m═●▶\033[0m│",     /* 28: right end of horizontal, unhit ship on the right field */
        "\033[32;1;7m│●│\033[0m│",     /* 29: vertical, unhit ship on the right field */
        "\033[32;1;7m╱●╲\033[0m│",    /* 30: top end of vertical, unhit ship on the right field */
        "\033[32;1;7m╲●╱\033[0m│",    /* 31: bottom end of vertical, unhit ship on the right field */
        "\033[32;1;7m❰●❱\033[0m│",     /* 32: unhit ship on the right field of the length 1 */
        "\033[31;1;7m ╳ \033[0m│",     /* 33: hit ship (on left side) */
        "\033[31;1;7m═╳═\033[0m│",     /* 34: horizontal, hit ship */
        "\033[31;1;7m◀╳═\033[0m│",     /* 35: left end of horizontal, hit ship */
        "\033[31;1;7m═╳▶\033[0m│",     /* 36: right end of horizontal, hit ship */
        "\033[31;1;7m│╳│\033[0m│",     /* 37: vertical, hit ship */
        "\033[31;1;7m╱╳╲\033[0m│",    /* 38: top end of vertical, hit ship */
        "\033[31;1;7m╲╳╱\033[0m│",    /* 39: bottom end of vertical, hit ship */
        "\033[31;1;7m❰╳❱\033[0m│",     /* 40: hit ship of the length 1*/
        "\033[35;1;7m╳╪╳\033[0m│",     /* 41: horizontal, sunken ship */
        "\033[35;1;7m◀╪╳\033[0m│",     /* 42: left end of horizontal, sunken ship */
        "\033[35;1;7m╳╪▶\033[0m│",     /* 43: right end of horizontal, sunken ship */
        "\033[35;1;7m╳╫╳\033[0m│",     /* 44: vertical, sunken ship */
        "\033[35;1;7m╱╫╲\033[0m│",    /* 45: top end of vertical, sunken ship */
        "\033[35;1;7m╲╫╱\033[0m│",    /* 46: bottom end of vertical, sunken ship */
        "\033[35;1;7m❰┼❱\033[0m│"      /* 47: sunken ship of the lenght 1 */
};

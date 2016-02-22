/**
 * @file    tetris.c
 * @author  Joel Wachsler (wachsler@kth.se)
 * @author  Marcus Werlinder (werli@kth.se)
 * @date    2016
 * @copyright For copyright and licensing, see file COPYING
 *
 * The area of the tetris play-field is 10x16 squares in the original game.
 * We will use the same width in our version but the height will be double.
 * Each x and y coord represents a 3x3 square.
 */

#include <stdint.h>         /* Declarations of uint_32 and the like */
#include <pic32mx.h>        /* Declarations of system-specific addresses etc */
#include "declaration.h"    /* Declarations of project specific functions */

/**
 * Do tasks before everything starts
 */
void init(void) {
    display_init(); // Initalize display

    /*display_string(0, "TETRIS!!!");*/
    /*display_update();*/

    display_update();

    draw_borders();

    Shape shape;
    shape.piece_type = 1;
    create_shape(&shape);
    rotate_shape(&shape);
    display_shape(&shape);

    render();
}

/**
 * This function is called over and over again
 */
void update(void) {
}

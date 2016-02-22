/**
 * @file    tetris.c
 * @author  Joel Wachsler (wachsler@kth.se)
 * @author  Marcus Werlinder (werli@kth.se)
 * @date    2016
 * @copyright For copyright and licensing, see file COPYING
 *
 * This file was written 2016 by Joel wachsler and Marcus Werlinder.
 */

#include <stdint.h>         /* Declarations of uint_32 and the like */
#include <pic32mx.h>        /* Declarations of system-specific addresses etc */
#include "declaration.h"    /* Declarations of project specific functions */

uint8_t buffer[512] = {0};    // Display buffer

// Create the different shapes

/**
 * This function will change the shape of the item passed to it
 *
 * @param [in] shape Pointer to the shape that will be rotated
 */
void rotate(Shape *shape) {
}

/**
 * Creates the shape based on the id pass to this function.
 * 0: L left
 * 1: L right
 * 2: Big rectangle
 * 3: Long piece laying down
 * 4: Triangle part
 * 5: Zigzag left
 * 6: Zigzag right
 *
 * @param [in] id The id of the shape
 * @param [in] shape Pointer to the shape where we should create it (do it like this so we don't have to care about cleaning up)
 */
void create_shape(char id, Shape *shape) {
}

/**
 * Do tasks before everything starts
 */
void init(void) {
    display_init(); // Initalize display

    /*display_string(0, "TETRIS!!!");*/
    /*display_update();*/

    display_update();

    draw_borders();
    Rectangle rect;

    // This is a test for all the pieces
    // L left
    rect.x = 3;
    rect.y = 2;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 2;
    draw_rectangle(&rect);
    rect.x = 5;
    rect.y = 2;
    draw_rectangle(&rect);
    rect.x = 3;
    rect.y = 3;
    draw_rectangle(&rect);
    // L right
    rect.x = 3;
    rect.y = 13;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 13;
    draw_rectangle(&rect);
    rect.x = 5;
    rect.y = 13;
    draw_rectangle(&rect);
    rect.x = 5;
    rect.y = 14;
    draw_rectangle(&rect);
    // Big rectangle
    rect.x = 3;
    rect.y = 6;
    draw_rectangle(&rect);
    rect.x = 3;
    rect.y = 5;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 6;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 5;
    draw_rectangle(&rect);
    // Long piece laying down
    rect.x = 3;
    rect.y = 8;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 8;
    draw_rectangle(&rect);
    rect.x = 5;
    rect.y = 8;
    draw_rectangle(&rect);
    rect.x = 6;
    rect.y = 8;
    draw_rectangle(&rect);
    // Triangle part
    rect.x = 3;
    rect.y = 10;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 10;
    draw_rectangle(&rect);
    rect.x = 5;
    rect.y = 10;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 11;
    draw_rectangle(&rect);
    // Zigzag left
    rect.x = 3;
    rect.y = 17;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 17;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 16;
    draw_rectangle(&rect);
    rect.x = 5;
    rect.y = 16;
    draw_rectangle(&rect);
    // Zigzag right
    rect.x = 5;
    rect.y = 20;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 20;
    draw_rectangle(&rect);
    rect.x = 4;
    rect.y = 19;
    draw_rectangle(&rect);
    rect.x = 3;
    rect.y = 19;
    draw_rectangle(&rect);

    render();
}

/**
 * This function is called over and over again
 */
void update(void) {
}

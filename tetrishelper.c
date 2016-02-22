/**
 * @file    tetrishelper.c
 * @author  Joel Wachsler (wachsler@kth.se)
 * @author  Marcus Werlinder (werli@kth.se)
 * @date    2016
 * @copyright For copyright and licensing, see file COPYING
 *
 * Helper functions for the tetris project
 */

#include <stdint.h>         /* Declarations of uint_32 and the like */
#include <pic32mx.h>        /* Declarations of system-specific addresses etc */
#include "declaration.h"    /* Declarations of project specific functions */

// Define the start origin on the game field
#define ORIGIN_X 4
#define ORIGIN_Y 26

/**
 * Creates the shape based on the id pass to this function.
 * We have to pass the result shape to the function because
 * we cannot dynamicly allocate memory.
 * These are the different piece_types and their corresponding id.
 * 0: Long piece (I)
 * 1: L
 * 2: J
 * 3: Big square (O)
 * 4: Triangle (T)
 * 5: Zigzag left (z)
 * 6: Zigzag right (s)
 *
 * @param [in] shape Pointer to the resulting struct with the id already defined
 */
void create_shape(Shape *shape) {
    // Origin of the current piece
    shape->x = ORIGIN_X;
    shape->y = ORIGIN_Y;
    // The pieces are always created at the same origin coordinate (4, 28)
    // 5 coords from the top.
    switch(shape->piece_type) {
        case 0:
            // Bottom
            shape->piece[0].x = ORIGIN_X;
            shape->piece[0].y = ORIGIN_Y;
            // Left
            shape->piece[1].x = ORIGIN_X-1;
            shape->piece[1].y = ORIGIN_Y;
            // Two left
            shape->piece[2].x = ORIGIN_X-2;
            shape->piece[2].y = ORIGIN_Y;
            // Right
            shape->piece[3].x = ORIGIN_X+1;
            shape->piece[3].y = ORIGIN_Y;
            break;
        case 1:
            // Center
            shape->piece[0].x = ORIGIN_X;
            shape->piece[0].y = ORIGIN_Y;
            // Right
            shape->piece[1].x = ORIGIN_X+1;
            shape->piece[1].y = ORIGIN_Y;
            // Left
            shape->piece[2].x = ORIGIN_X-1;
            shape->piece[2].y = ORIGIN_Y;
            // Bottom left
            shape->piece[3].x = ORIGIN_X-1;
            shape->piece[3].y = ORIGIN_Y-1;
            break;
        case 2:
            // Center
            shape->piece[0].x = ORIGIN_X;
            shape->piece[0].y = ORIGIN_Y;
            // Right
            shape->piece[1].x = ORIGIN_X+1;
            shape->piece[1].y = ORIGIN_Y;
            // Left
            shape->piece[2].x = ORIGIN_X-1;
            shape->piece[2].y = ORIGIN_Y;
            // Bottom right
            shape->piece[3].x = ORIGIN_X+1;
            shape->piece[3].y = ORIGIN_Y-1;
            break;
        case 3:
            // Bottom left
            shape->piece[0].x = ORIGIN_X;
            shape->piece[0].y = ORIGIN_Y;
            // Bottom right
            shape->piece[1].x = ORIGIN_X + 1;
            shape->piece[1].y = ORIGIN_Y;
            // Top left
            shape->piece[2].x = ORIGIN_X;
            shape->piece[2].y = ORIGIN_Y + 1;
            // Top right
            shape->piece[3].x = ORIGIN_X + 1;
            shape->piece[3].y = ORIGIN_Y + 1;
            break;
        case 4:
            // Top center
            shape->piece[0].x = ORIGIN_X;
            shape->piece[0].y = ORIGIN_Y;
            // Bottom center
            shape->piece[1].x = ORIGIN_X;
            shape->piece[1].y = ORIGIN_Y-1;
            // Top left
            shape->piece[2].x = ORIGIN_X-1;
            shape->piece[2].y = ORIGIN_Y;
            // Top right
            shape->piece[3].x = ORIGIN_X+1;
            shape->piece[3].y = ORIGIN_Y;
            break;
        case 5:
            // Bottom center
            shape->piece[0].x = ORIGIN_X;
            shape->piece[0].y = ORIGIN_Y;
            // Top center
            shape->piece[1].x = ORIGIN_X;
            shape->piece[1].y = ORIGIN_Y+1;
            // Top left
            shape->piece[2].x = ORIGIN_X-1;
            shape->piece[2].y = ORIGIN_Y+1;
            // Bottom right
            shape->piece[3].x = ORIGIN_X+1;
            shape->piece[3].y = ORIGIN_Y;
            break;
        case 6:
            // Bottom center
            shape->piece[0].x = ORIGIN_X;
            shape->piece[0].y = ORIGIN_Y;
            // Top center
            shape->piece[1].x = ORIGIN_X;
            shape->piece[1].y = ORIGIN_Y+1;
            // Top right
            shape->piece[2].x = ORIGIN_X+1;
            shape->piece[2].y = ORIGIN_Y+1;
            // Bottom left
            shape->piece[3].x = ORIGIN_X-1;
            shape->piece[3].y = ORIGIN_Y;
            break;
    }

    // Add these to the before so we can render it freely
    int i;
    for(i = 0; i < 4; i++) {
        shape->piece[i].x_before = shape->piece[i].x;
        shape->piece[i].y_before = shape->piece[i].y;
    }
}

/**
 * This function will change the shape of the item passed to it
 *
 * @param [in] shape Pointer to the shape which will be rotated
 */
void rotate_shape(Shape *shape) {
    int i, x, y;
    // Go through each square.
    // The first square is always the origin of the piece
    // so we don't have to do anything about that one.
    for(i = 1; i < 4; i++) {
        // Applying the tetris formula for rotation
        // x' = xc + yc - y
        // y' = yc - xc + x
        // where (xc, yc) are the origin coordinates
        x = shape->piece[i].x;
        y = shape->piece[i].y;
        shape->piece[i].x = shape->x + shape->y - y;
        shape->piece[i].y = shape->y - shape->x + x;
    }
}

/**
 * Moves the shape one step towards the bottom of the screen.
 *
 * @param [in] shape The shape to move
 */
void gravity(Shape *shape) {
    int i;
    for(i = 0; i < 4; i++)
        shape->piece[i].y -= 1;
}

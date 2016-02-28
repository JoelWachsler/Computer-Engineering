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
* Create an 33 * 11 boolean array that will represent the grid
* for tetris.
*/
bool grid[(32+1)*(10+2)] = {false}; //create the grid

/**
* Create the borders for the grid and set everything else to false
*/
void setGrid(void)
{
    int x;
    int y;
    for(y=0; y < 32; y++)
    for(x=0; x < 12; x++)
    {
        if(x == 0 || x == 11 || y == 0)//To declare all the borders as true
        grid[x + y * 12] = true;
        else
        grid[x + y * 12] = false;//Everyting else will be false at start
    }
}

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
}

/**
 * This function will change the shape of the item passed to it
 *
 * @param [in] shape Pointer to the shape which will be rotated
 */
void rotate_shape(Shape *shape) {
    unsigned char i, x, y, xc, yc;
    xc = shape->piece[0].x;
    yc = shape->piece[0].y;
    // Go through each square.
    // The first square is always the origin of the piece
    // so we don't have to do anything about that one.
    for(i = 1; i < 4; i++){
        // Applying the tetris formula for rotation
        // x' = xc + yc - y
        // y' = yc - xc + x
        // where (xc, yc) are the origin coordinates
        x = shape->piece[i].x;
        y = shape->piece[i].y;
        shape->piece[i].x = xc + yc - y;
        shape->piece[i].y = yc - xc + x;
    }
}

/**
 * Moves the shape one step towards the bottom of the screen.
 *
 * @param [in] shape The shape to move
 */
void gravity(Shape *shape) {
    // Gravity doesn't play well with rotate_shape
    int i;
    for(i = 0; i < 4; i++){
        shape->piece[i].y -= 1;
    }
}

/**
 * Moves the shape either right or left on the screen.
 *
 * @param [in] shape The shape to move
 * @param [in] way decides if we go left or right
 */
void moveSideways(Shape *shape, int way) {
    // Gravity doesn't play well with rotate_shape
    int i;
    for(i = 0; i < 4; i++){
        shape->piece[i].x += way;
    }
}

Shape gridShape; //To be able to write out grid
Shape rotateCopy; //Is going to be a rotate dummy

/**
* This function will check if something is below
* the Tetris piece. It also sends back 1 or 0 if we
* should interrupt gravity and start a new piece
* @param [in] shape has 4 squares that all have x and y that we want to check
*/
bool belowCheck(Shape *shape){
    int i;//Loop array to set all squares as true if something below
    int j;//-||-

    int x;//The y and x cordinates that we will translate into the right in
    int y;//grid
    for(i = 0; i < 4; i++){
        x = shape->piece[i].x;//Test every square in the shape if it collides
        y = shape->piece[i].y;
        if(grid[(x + 1) + y * 12]){//Check if the arraybox below is true or not
            for(j = 0; j < 4; j++){
                x = shape->piece[j].x;//Set every square as true in grid
                y = shape->piece[j].y;
                grid[(x + 1) + (y * 12) + 12] = true;// This will set all the shapes boxes
            }
            return false;                          // as true
        }
    }
    return true;
}

/**
* This function will check if we hit a wall from the right or left
* @param [in] shape has 4 squares that all have x and y that we want to check
* @param [in] LorR is an int that tells if we want to go left or right
*/
bool sideCheck(Shape *shape, int LorR){
    int i;//Loop array to set all squares as true if something below

    int x;//The y and x cordinates that we will translate into the right box in
    int y;//grid

    for(i = 0; i < 4; i++){
        x = shape->piece[i].x;//Set every square as true in grid
        y = shape->piece[i].y;
        if(grid[(x + 1) + (y * 12) + 12 + LorR])//To check one box to the left
        return false;
    }

    return true;
}

/**
* This function will check if we can actually rotate the piece
* @param [in] the shape we want to check for rotation
*/
bool rotateCheck(Shape *shape){
    int i;
    rotateCopy.piece_type = shape->piece_type;
    create_shape(&rotateCopy);//The dummy is alive MUAHAHAHHAHAH
    for(i = 0; i < 4; i++){
        rotateCopy.piece[i].x = shape->piece[i].x;
        rotateCopy.piece[i].y = shape->piece[i].y;
    }
    rotate_shape(&rotateCopy);
    for(i = 0; i < 4; i++){
        if(grid[(rotateCopy.piece[i].x + 1) + (rotateCopy.piece[i].y * 12) + 12]
           || rotateCopy.piece[i].x < 0 || rotateCopy.piece[i].x > 9 ||
           rotateCopy.piece[i].y < 0){

            return false;
        }
    }
    return true;
}

/**
*This function will draw all the true in the gridbox
*/
void draw_grid_pieces(){

    int x;
    int y;
    gridShape.piece_type = 1;
    create_shape(&gridShape);
    for(y = 0; y < 32; y++){
        for(x = 0; x < 10; x++){
            if(grid[(x + 1) + (y * 12) + 12]){
                gridShape.piece[0].x = x;
                gridShape.piece[0].y = y;

                draw_square(&gridShape.piece[0]);//To draw out all the squares
            }
        }
    }

}

/**
 *Fixer is just a bad name for the algoritm that checks if there is a full row
 * @param [in] y tells us what y cordinate we are on
 * @param [in] b tells us if it's case 1 (row not at the bottom) or case 2 when it is at the bottom
 * @param [in] antalRow just tells us how many rows we need to remove
 */
void fixer(int y, int b, int antalRow){
    int x;
    int y2;
        for(y2 = y + b; y2 < (y + b + antalRow); y2++){//Remove the full rows
            for(x = 0; x < 10; x++)
                grid[(x + 1) + (y2 * 12) + 12] = false;
        }
        for(y2 = y + b + antalRow; y2 < 32; y2++){//Shift everthing down
            for(x = 0; x < 10; x++){
                if(grid[(x + 1) + (y2 * 12) + 12]){
                    grid[(x + 1) + (y2 * 12) + 12] = false;
                    grid[(x + 1) + (y2 * 12) + 12 - antalRow * 12] = true;
                }
            }
        }
}
/**
 * This program will check if we have mutlipel full rows and will delet them
 * And move the above down
 */
void fullRow(){
    int x;
    int y;
    int fullrow = 0;
    int antalRow = 0;

    for(y = 31; y > -1; y--){//Going through all rows to see if we have a full row
        fullrow = 0;
        for(x = 0; x < 10; x++){
            if(grid[(x + 1) + (y * 12) + 12])
                fullrow++;
        }
        if(fullrow == 10){//If we have a full row we add one to the amount of rows to remove
            antalRow++;
        }
        if(fullrow < 10)//1 case when we have a row not at the bottom
            if(antalRow > 0){
                fixer(y, 1, antalRow);
                antalRow = 0;
        }
        if(y == 0)//2 case when the full row is at the bottom
            if(antalRow > 0){
                fixer(y, 0, antalRow);
                antalRow = 0;
            }
    }
}

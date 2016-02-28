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
#include <stdbool.h>        /* To be able to use boolean */
#include <stdlib.h>         /* abs */

int btns;
int btns2;
int getbtns(void) {
    return PORTD >> 5 & 0b1111;
}
int getbtns2(void){//We want to get input from button 1
    return PORTF >> 1 & 0x01;
}

/**
* Hardware timer init
*/
static void timer_init(void) {
    // TIMER
    T2CON = 0x70;                   // Stop timer and set prescale to 1:256
    PR2 = (80000000 / 256) / 10;    // Set period to flag 10 times a second
    T2CONSET = 0x8000;              // Start the timer (the bit to start the
        // timer's located at bit 15)
}

/**
* Hardware button init
*/
static void btn_init(void) {
    // Init port D
    // Set bits 11 through 5 to 1 (input)
    TRISDSET = 0b1111111 << 5;
}

// Create a dummy shape for now
Shape shape;
/**
* Do tasks before everything starts
*/
void init(void) {
    display_init(); // Initalize display
    timer_init();

    /*display_string(0, "TETRIS!!!");*/
    display_update();

    draw_borders();

    setGrid();//To set the borders in the grid to true

    shape.piece_type = 0;

    create_shape(&shape);
    //create_shape(&shape1);
    /*rotate_shape(&shape);*/
    //draw_shape(&shape);
    //draw_shape(&shape1);

    render();
}

unsigned int gametick = 0;
unsigned int other = 0;
/**
* This function is called over and over again
*/
void update(void) {
    btns = getbtns();
    btns2 = getbtns2();

    if (IFS(0) & 0x100) {
        IFS(0) = 0; // Reset timer flag

        if(btns2 == 1){//If we want to get fast down
            if(belowCheck(&shape))
                gravity(&shape);
            else
                create_shape(&shape);
        }

        if(btns == 1) //Now we want to check if we can actually go to the sides
            if(sideCheck(&shape, 1))
                moveSideways(&shape, 1);

        if(btns == 2)//Btn3 Controls this
            if(sideCheck(&shape, -1))
                moveSideways(&shape, -1);

        if(btns == 4) //Now we want to check if we can rotate
            if(rotateCheck(&shape))
                rotate_shape(&shape);
                
        // Tick once a second
        if (gametick++ % 10 == 0) {
            /*if (btns & 0x1) {*/
            /*}*/
            //rotate_shape(&shape);

            if(belowCheck(&shape))
                gravity(&shape);
            else
                create_shape(&shape);
                //Reset shapes cordinates and change shape
            fullRow();

            /*if (other++ % 2 == 0) {
            rotate_shape(&shape1);
            gravity(&shape1);
        }*/
        }
        // Update the screen 10 times a second
        draw_grid_pieces();
        draw_shape(&shape);
        //draw_shape(&shape1);
        render();
    }
}

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

static unsigned char btns;
static unsigned char menuPointer;
static unsigned int score = 0;
static unsigned char lvl = 10;

static uint64_t gametick = 0;
static uint64_t seed = 0;

static Shape shape;
static Shape menuSelect;

typedef enum {
    MAIN_MENU,
    GAME,
    HISCORE
} Game_Screen;

// The current game screen
Game_Screen current_game_screen;

// Get all buttons
unsigned char getbtns(void) {
    return PORTD >> 4 & 0b1110 | PORTF >> 1 & 0x01;
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

static void game_init(void) {
    current_game_screen = GAME;

    // Set seed
    rng.state = 0U;
    rng.inc = (seed << 1u) | 1u;
    pcg32_random_r(&rng);
    rng.state += seed;
    pcg32_random_r(&rng);

    draw_borders();
    setGrid();//To set the borders in the grid to true
    create_shape(&shape);

    render();
}

static void main_menu_init(void) {
    current_game_screen = MAIN_MENU;

    menuPointer = 0;
    menuSelect.piece_type = 0;
    create_shape(&menuSelect);
    menuSelect.piece[0].x = 1;
    menuSelect.piece[0].y = 29;

    render();
}

static void hiscore_init(void) {
    current_game_screen = HISCORE;

    render();
}

// Create a dummy shape for now
/**
* Do tasks before everything starts
*/
void init(void) {
    display_init(); // Initalize display
    timer_init();
    display_update();

    main_menu_init();   // Start the main menu
}

static void main_menu(void) {
    switch(btns) {
        case 2:
            if (!menuPointer)
                game_init();
            else
                hiscore_init();

            break;
        case 4:
            menuSelect.piece[0].y = 25;
            menuPointer = 1;
            break;
        case 8:
             menuSelect.piece[0].y = 29;
             menuPointer = 0;
             break;
    }

    draw_square(&menuSelect.piece[0]);
    draw_menu();
}

static void game(void) {
    switch(btns) {
        case 1:
            if(belowCheck(&shape)){
                gravity(&shape);
                score += 10;
            }
            else
                create_shape(&shape);
            break;
        case 2:
            if(sideCheck(&shape, 1)) //Now we want to check if we can actually go to the sides
                moveSideways(&shape, 1);
            break;
        case 4:
            if(sideCheck(&shape, -1))
                moveSideways(&shape, -1);
            break;
        case 8:
            if(rotateCheck(&shape))
                rotate_shape(&shape);
            break;

    }

    // Tick once a second
    if (gametick++ % 2 == 0) {
        if(belowCheck(&shape))
            gravity(&shape);
        else {
            create_shape(&shape);
        }
        //Reset shapes cordinates and change shape
        score += (1000/(4/fullRow()));
        /*if(score > 100)*/
            /*lvl = 2;*/
    }

    draw_shape(&shape);
    draw_grid_pieces();
    draw_borders();
}

static void hiscore(void) {
    if (btns)
        main_menu_init();
}

/**
* This function is called over and over again
*/
void update(void) {
    btns = getbtns();
    seed++;

    if (IFS(0) & 0x100) {
        IFS(0) = 0; // Reset timer flag

        switch(current_game_screen) {
            case MAIN_MENU:
                main_menu();
                break;
            case GAME:
                game();
                break;
            case HISCORE:
                hiscore();
                break;
        }

        // Update the screen 10 times a second
        render();
    }
}

#ifndef DECLARATION_H_URHXV5O2
#define DECLARATION_H_URHXV5O2

/**
 * @file    declaration.h
 * @author  F Lundevall
 * @author  Axel Isaksson
 * @author  Joel Wachsler (wachsler@kth.se)
 * @author  Marcus Werlinder (werli@kth.se)
 * @date    2015-2016
 * @copyright For copyright and licensing, see file COPYING
 *
 * This header file was written in 2016 by Joel wachsler and Marcus Werlinder.
 * Some of the content of this file was based on the lab files written
 * by F Lundevall and Axel Isaksson.
 */

#include <stdint.h>         /* Declarations of uint_32 and the like */
#include <pic32mx.h>        /* Declarations of system-specific addresses etc */
#include <stdbool.h>        /* To be able to use boolean */

// Declare different types of pieces
typedef enum {
    I,
    L,
    J,
    O,
    T,
    Z,
    S
} Piece_Type;

// Declare rectangle for abstraction between
// the programmer and the oled screen
typedef struct {
    unsigned char x;
    unsigned char y;
} Square;

// This shape will be used to build the different blocks
typedef struct {
    Square piece[4];   // There's always 4 pieces
    Piece_Type piece_type;
} Shape;

// Used for holding the highscores
typedef struct {
    unsigned int* scores;
} Highscore;

/* Declare display-related functions from display.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);
void render(void);
void draw_shape(const Shape *shape);
void draw_square(const Square *square);
void draw_grid_pieces(void);
void draw_menu(void);
void draw_borders(void);
void draw_gameScreen(void);
void draw_number(unsigned const char num, unsigned const char x, unsigned const char y);
void draw_score(unsigned int num, unsigned const short y);
void draw_hiscore(void);
void draw_punctuation(const unsigned char y);
void animation_start(void);


/* Declare functions used for easier creation of tetris */
void create_shape(Shape *shape);
void adapt_piece(Shape *shape);
void rotate_shape(Shape *shape);
void moveSideways(Shape *shape, int way);
bool belowCheck(Shape *shape); //Is needed, to know when we are at bottom
bool sideCheck(Shape *shape, int LorR);
bool rotateCheck(Shape *shape);
int fullRow(void);
void randomize_piece(Shape *shape);

/* Declare functions from helper.c */
unsigned int pow(unsigned const char base, unsigned char exponent);
/*char *itoaconv(int num);*/
/*void quicksleep(int cyc);*/

// Random
typedef struct {
    uint64_t state;
    uint64_t inc;
} pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t* rng);
// Make this variable global to we can seed it
extern pcg32_random_t rng;

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Game over death scree font*/
extern const unit8_t const game_over_font[512]; 
/* More stuff to game display*/
extern const uint8_t const gameFont[512];
/* Sexy menu */
extern const uint8_t const menuFont[512];
/* Score font */
extern const uint8_t const numFont[5*2*10];
// "HISCORE" text
extern const uint8_t const highscoreFont[8*4];
/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

/* Game specific declarations */
void update();
void init();

#endif /* end of include guard: DECLARATION_H_URHXV5O2 */

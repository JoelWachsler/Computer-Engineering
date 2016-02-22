/**
 * @file    tetris.h
 * @author  F Lundevall
 * @author  Axel Isaksson
 * @author  Joel Wachsler (wachsler@kth.se)
 * @author  Marcus Werlinder (werli@kth.se)
 * @date    2015-2016
 * @copyright For copyright and licensing, see file COPYING
 *
 * This header file was written 2016 by Joel wachsler and Marcus Werlinder
 * Content of this file is based on the lab files written by F Lundevall
 * and Axel Isaksson
 */

// Declare rectangle for abstraction between
// the programmer and the oled screen
typedef struct {
    unsigned char x;
    unsigned char y;
} Rectangle;

// This shape will be used to build the different blocks
typedef struct {
    unsigned char rotation;
    unsigned char piece;
    struct Rectangle *pieces;
} Shape;

/* Declare display-related functions from display.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);
void render(void);
void draw_rectangle(const Rectangle *rectangle);

/* Declare lab-related functions from helper.c */
char *itoaconv(int num);
void quicksleep(int cyc);
void tick(unsigned int * timep);
void clear(uint8_t *item);

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

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

/* Game specific declarations */
void update();
void init();

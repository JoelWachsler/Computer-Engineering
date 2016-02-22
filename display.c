/**
 * @file    display.c
 * @author  F Lundevall
 * @author  Axel Isaksson
 * @author  Joel Wachsler (wachsler@kth.se)
 * @author  Marcus Werlinder (werli@kth.se)
 * @date    2015-2016
 * @copyright For copyright and licensing, see file COPYING
 *
 * This file was written 2016 by Joel wachsler and Marcus Werlinder
 * Content of this file is based on the file "mipslabwork.c" 2015 by F Lundevall
 * Some parts are original code written by Axel Isaksson
 */

#include <stdint.h>         /* Declarations of uint_32 and the like */
#include <pic32mx.h>        /* Declarations of system-specific addresses etc */
#include "declaration.h"    /* Declarations of project specific functions */
#include <stdbool.h>    // bool

// Declare static buffer for the screen
static uint8_t buffer[512] = {0};

/* Declare a helper function which is local to this file */
static void num32asc(char *s, int);

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/**
 * @brief A function to help debugging.
 *
 * After calling display_debug,
 * the two middle lines of the display show
 * an address and its current contents.
 * Note: When you use this function, you should comment out any
 * repeated calls to display_image; display_image overwrites
 * about half of the digits shown by display_debug.
 *
 * @param [in] addr The address to read and display.
 */
void display_debug(volatile int *const addr) {
    display_string( 1, "Addr" );
    display_string( 2, "Data" );
    num32asc( &textbuffer[1][6], (int) addr );
    num32asc( &textbuffer[2][6], *addr );
    display_update();
}

/**
 * SPI2 helper function to determine if the interface is ready.
 */
uint8_t spi_send_recv(uint8_t data) {
    while(!(SPI2STAT & 0x08));
    SPI2BUF = data;
    while(!(SPI2STAT & 1));
    return SPI2BUF;
}

/**
 * Commands to initalize the oled sqreen. If they are not done
 * in this order the screen could be damaged!
 */
void display_init(void) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    quicksleep(10);
    DISPLAY_ACTIVATE_VDD;
    quicksleep(1000000);

    spi_send_recv(0xAE);
    DISPLAY_ACTIVATE_RESET;
    quicksleep(10);
    DISPLAY_DO_NOT_RESET;
    quicksleep(10);

    spi_send_recv(0x8D);
    spi_send_recv(0x14);

    spi_send_recv(0xD9);
    spi_send_recv(0xF1);

    DISPLAY_ACTIVATE_VBAT;
    quicksleep(10000000);

    spi_send_recv(0xA1);
    spi_send_recv(0xC8);

    spi_send_recv(0xDA);
    spi_send_recv(0x20);

    spi_send_recv(0xAF);
}

/**
 * Simple display string function(will probably not be used).
 *
 * @param [in] line The line which the string should be written
 * @param [in] s Pointer to the string which should be written
 */
void display_string(int line, char *s) {
    int i;
    if(line < 0 || line >= 4)
        return;
    if(!s)
        return;

    for(i = 0; i < 16; i++)
        if(*s) {
            textbuffer[line][i] = *s;
            s++;
        } else
            textbuffer[line][i] = ' ';
}

/**
 * Simple display function for custom graphics(can only handle 32x32 image).
 *
 * @param [in] x Coordinate from the left small side of the screen
 * @param [in] data A pointer to the data which will be written
 */
void display_image(const int x, const uint8_t *data) {
    int page, j;

    // 4 stripes across the display called pages
    // each stripe is 8 pixels high and can hold 128 bytes
    for(page = 0; page < 4; page++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);    // Command to set the page
        spi_send_recv(page);    // Set the current page

        // Don't really know how these work
        spi_send_recv(x & 0xF);
        spi_send_recv(0x10 | ((x >> 4) & 0xF));

        DISPLAY_CHANGE_TO_DATA_MODE;

        // j is the x axis of the current page
        for(j = 0; j < 32; j++)
            // Each byte sent to this function is a 8 pixel high column on the display
            // the lsb is the top most pixel and the msb is the most bottom pixel
            spi_send_recv(~data[page*32 + j]);
    }
}

/**
 * This function puts the data from the buffer provided to it and sends it to
 * the screen. The buffer is always 4 * 128 bytes (512 bytes) big.
 * Gets its data from the static buffer.
 */
void render() {
    int page, j;

    // 4 stripes across the display called pages
    // each stripe is 8 pixels high and can hold 128 bytes
    for(page = 0; page < 4; page++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);    // Command to set the page
        spi_send_recv(page);    // Set the current page

        // Don't really know how these work
        // But they have something to do with the x value from
        // the left side of the screen
        spi_send_recv(0x0);
        spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;

        // j is the x axis of the current page
        for(j = 0; j < 128; j++)
            // Each byte sent to this function is a 8 pixel high column on the display
            // the lsb is the top most pixel and the msb is the most bottom pixel
            /*spi_send_recv(data[page + j]);*/
            spi_send_recv(buffer[page*128 + j]);
    }
}

/**
 * Draws a 3x3 square at the given x and y coord (origin is at the top right corner of the screen).
 * The x and y coord is at the bottom left of the square.
 * A little bit of abstraction :).
 *
 * @param [in] square The item to draw
 * @param [in] remove Should the item be removed or drawn? (true = remove, false = draw)
 */
static void draw_square(Square *square, bool remove) {
    // Is this a valid x or y coord?
    // 0 <= x <= 9
    // 0 <= y <= 31
    if (square->x > 9 || square->x < 0 || square->y > 31 || square->y < 0)
        return;

    unsigned char page, value;
    // Hard coded x values (2 and 7 are missing because they
    // require to pages to render, special cases below has been
    // created for them).
    switch(square->x) {
        case 9:
            page = 0;
            value = 14;
            break;
        case 8:
            page = 0;
            value = 112;
            break;
        case 6:
            page = 1;
            value = 28;
            break;
        case 5:
            page = 1;
            value = 224;
            break;
        case 4:
            page = 2;
            value = 7;
            break;
        case 3:
            page = 2;
            value = 56;
            break;
        case 1:
            page = 3;
            value = 14;
            break;
        case 0:
            page = 3;
            value = 112;
            break;
    }

    // Calulcate the y coordinates and put the data in the
    // display buffer so we can render it
    // First remove the item from the place it was before
    // and then add it to its new position
    int origin, origin_before;
    switch(square->x) {
        case 7:
            // Draw item at the new position
            origin = 128 + 127 - square->y*3 - 1;
            buffer[origin]      |= 3;
            buffer[origin-1]    |= 3;
            buffer[origin-2]    |= 3;

            // Draw item at the new position
            origin = 127 - square->y*3 - 1;
            buffer[origin]      |= 128;
            buffer[origin-1]    |= 128;
            buffer[origin-2]    |= 128;
            break;
        case 2:
            // Draw item at the new position
            origin = 2*128 + 127 - square->y*3 - 1;
            buffer[origin]      |= 192;
            buffer[origin-1]    |= 192;
            buffer[origin-2]    |= 192;

            // Draw item at the new position
            origin = 3*128 + 127 - square->y*3 - 1;
            buffer[origin]      |= 1;
            buffer[origin-1]    |= 1;
            buffer[origin-2]    |= 1;
            break;
        default:
            // The page's calculated in the switch statement above
            // and we just add three lines, the first from the origin
            // and the second two over it so we get a nice 3x3 triangle
            origin = page*128 + 127 - square->y*3 - 1;
            buffer[origin]      |= value;
            buffer[origin-1]    |= value;
            buffer[origin-2]    |= value;
    }

    // Set the x_before and y_before to the current ones
    square->x_before = square->x;
    square->y_before = square->y;
}

/**
 * Draws the shape given to this function by the pointer passed to it.
 * It goes through each square(always 4) and sends it to the draw_shape function
 * which renders it at the given destination.
 *
 * @param [in] shape Pointer to shape which to render
 */
void draw_shape(Shape *shape) {
    int i;
    for(i = 0; i < 4; i++) {
        draw_square(&shape->piece[i], false);
    }
}

/**
 * Draw the borders
 */
void draw_borders(void) {
    int i;
    // Draw the borders
    // Will draw them from the bottom and up
    for(i = 0; i < 96; i++) {
        // Draw the right border
        buffer[127 - i] |= 1;
        // Draw the left border
        buffer[511 - i] |= 128;
    }

    // Go through each page and put a line there
    for (i = 0; i < 4; i++) {
        // Draw top
        buffer[i * 128 + 4*8] |= 255;
        // Draw bottom
        buffer[(i + 1) * 128 - 1] |= 255;
    }
}

/**
 * Don't really know how this one works but it clears the screen
 * of items not being used.
 */
void display_update(void) {
    int i, j, k;
    int c;
    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0x0);
        spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;

        for(j = 0; j < 16; j++) {
            c = textbuffer[i][j];
            if(c & 0x80)
                continue;

            for(k = 0; k < 8; k++)
                spi_send_recv(font[c*8 + k]);
        }
    }
}

/**
 * Helper function, local to this file.
 * Converts a number to hexadecimal ASCII digits.
 *
 * @param [in] s the char will be placed in this pointer
 * @param [in] n the number to convert
 */
static void num32asc( char * s, int n ) {
    int i;
    for( i = 28; i >= 0; i -= 4 )
        *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

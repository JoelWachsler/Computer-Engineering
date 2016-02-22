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
#include <stdbool.h>        // bool

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

uint8_t spi_send_recv(uint8_t data) {
    while(!(SPI2STAT & 0x08));
    SPI2BUF = data;
    while(!(SPI2STAT & 1));
    return SPI2BUF;
}

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
 * Draws a 3x3 rectangle at the given x and y coord (origin is at the top right corner of the screen).
 * The x and y coord is at the bottom left of the rectangle.
 *
 * @param [in] x x-coordinate
 * @param [in] y y-coordinate
 */
void draw_rectangle(const Rectangle *rectangle) {
    // Determine if we can draw this shape
    if (rectangle->x > 9 || rectangle->x < 0)
        return;

    // Determine where to put the first line
    // First calculate the pages and values needed needed
    char page, value;

    // Hard coded x values
    switch(rectangle->x) {
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

    // Check if this item is the only one that requires two
    // pages to render
    if (rectangle->x == 7) {
        buffer[128 + 127 - rectangle->y*3] |= 3;
        buffer[128 + 127 - rectangle->y*3 - 1] |= 3;
        buffer[128 + 127 - rectangle->y*3 - 2] |= 3;

        buffer[127 - rectangle->y*3] |= 128;
        buffer[127 - rectangle->y*3 - 1] |= 128;
        buffer[127 - rectangle->y*3 - 2] |= 128;
    } else if (rectangle->x == 2) {
        buffer[2*128 + 127 - rectangle->y*3] |= 192;
        buffer[2*128 + 127 - rectangle->y*3 - 1] |= 192;
        buffer[2*128 + 127 - rectangle->y*3 - 2] |= 192;

        buffer[3*128 + 127 - rectangle->y*3] |= 1;
        buffer[3*128 + 127 - rectangle->y*3 - 1] |= 1;
        buffer[3*128 + 127 - rectangle->y*3 - 2] |= 1;
    } else {
        buffer[page*128 + 127 - rectangle->y*3] |= value;
        buffer[page*128 + 127 - rectangle->y*3 - 1] |= value;
        buffer[page*128 + 127 - rectangle->y*3 - 2] |= value;
    }
}

/**
 * Draws the shape given to this function by the pointer passed to it
 *
 * @param [in] shape Pointer to shape which to render
 */
void draw_shape(const Shape *shape) {
}

/**
 * Draws the borders
 */
void draw_borders(void) {
    int i;

    // Draw the borders
    for(i = 0; i < 128; i++) {
        // Draw the right border
        buffer[i] |= 1;
        // Draw the left border
        buffer[511-i] |= 128;
    }
}

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
 * @brief Helper function, local to this file.
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

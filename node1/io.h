#ifndef IO_H
#define IO_H

#include <stdint.h>


typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t btn;
} io_joystick_t;


typedef struct
{
    uint8_t right;
    uint8_t left;
    uint8_t nav;
} io_buttons_t;


/*
 * Read joystick through the
 * AVR on the I/O board.
 */
io_joystick_t io_read_joystick(void);


/*
 * Read all buttons on the
 * I/O board.
 */
io_buttons_t io_read_buttons(void);


#endif
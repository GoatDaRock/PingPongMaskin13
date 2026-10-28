#include <avr/io.h>
#include <util/delay.h>

#include "io.h"
#include "spi.h"


#define IO_CMD_JOYSTICK    0x03
#define IO_CMD_BUTTONS     0x04


io_joystick_t io_read_joystick(void)
{
    io_joystick_t joystick;

    /*
     * Select AVR on I/O board.
     */
    spi_select_slave(SPI_SLAVE_IO);


    /*
     * Send command:
     *
     * 0x03 = read joystick
     */
    spi_transfer(IO_CMD_JOYSTICK);


    /*
     * Minimum 40 us between command
     * and first returned data byte.
     */
    _delay_us(40);


    /*
     * Send dummy byte to generate SCK.
     * Returned byte contains X.
     */
    joystick.x = spi_transfer(0x00);

    _delay_us(2);


    /*
     * Y
     */
    joystick.y = spi_transfer(0x00);

    _delay_us(2);


    /*
     * Joystick button
     */
    joystick.btn = spi_transfer(0x00);


    /*
     * Release I/O-board AVR.
     */
    spi_deselect_all();


    return joystick;
}


io_buttons_t io_read_buttons(void)
{
    io_buttons_t buttons;


    /*
     * Select AVR on I/O board.
     */
    spi_select_slave(SPI_SLAVE_IO);


    /*
     * 0x04 = read buttons
     */
    spi_transfer(IO_CMD_BUTTONS);


    /*
     * Required delay after command.
     */
    _delay_us(40);


    /*
     * First byte:
     *
     * [0 0 R6 R5 R4 R3 R2 R1]
     */
    buttons.right = spi_transfer(0x00);


    _delay_us(2);


    /*
     * Second byte:
     *
     * [0 L7 L6 L5 L4 L3 L2 L1]
     */
    buttons.left = spi_transfer(0x00);


    _delay_us(2);


    /*
     * Third byte:
     *
     * [0 0 0 Up Down Left Right Btn]
     */
    buttons.nav = spi_transfer(0x00);


    spi_deselect_all();


    return buttons;
}
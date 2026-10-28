#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "oled.h"
#include "spi.h"


/*
 * DISP_D/C
 *
 * LOW  = command
 * HIGH = display data
 */
#define OLED_DC_PORT    PORTE
#define OLED_DC_DDR     DDRE
#define OLED_DC_PIN     PE2


/*
 * Simple 5x7 ASCII font.
 *
 * Characters from ASCII 32 (' ')
 * through ASCII 90 ('Z').
 *
 * Stored in FLASH using PROGMEM.
 */
static const uint8_t font5x7[][5] PROGMEM =
{
    {0x00,0x00,0x00,0x00,0x00}, // Space
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x07,0x00,0x07,0x00}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x55,0x22,0x50}, // &
    {0x00,0x05,0x03,0x00,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x14,0x08,0x3E,0x08,0x14}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x50,0x30,0x00,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /

    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9

    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x08,0x14,0x22,0x41,0x00}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x00,0x41,0x22,0x14,0x08}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?

    {0x32,0x49,0x79,0x41,0x3E}, // @

    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x3F,0x40,0x38,0x40,0x3F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}  // Z
};


void oled_write_command(uint8_t command)
{
    /*
     * D/C = LOW
     * Incoming SPI byte is interpreted
     * as an SSD1309 command.
     */
    OLED_DC_PORT &= ~(1 << OLED_DC_PIN);

    spi_select_slave(SPI_SLAVE_DISPLAY);

    spi_write(command);

    spi_deselect_all();
}


void oled_write_data(uint8_t data)
{
    /*
     * D/C = HIGH
     * Incoming SPI byte is interpreted
     * as pixel data.
     */
    OLED_DC_PORT |= (1 << OLED_DC_PIN);

    spi_select_slave(SPI_SLAVE_DISPLAY);

    spi_write(data);

    spi_deselect_all();
}


void oled_init(void)
{
    /*
     * D/C is output.
     */
    OLED_DC_DDR |= (1 << OLED_DC_PIN);

    /*
     * Start in command mode.
     */
    OLED_DC_PORT &= ~(1 << OLED_DC_PIN);


    /*
     * Recommended minimal initialization
     * from the lab instructions.
     */

    oled_write_command(0xA1);   // Segment remap

    oled_write_command(0xC8);   // Reverse COM scan direction

    oled_write_command(0xAF);   // Display ON
}


void oled_goto_line(uint8_t line)
{
    /*
     * There are 8 pages:
     *
     * 0 - 7
     */

    if (line > 7)
    {
        line = 7;
    }

    /*
     * B0 = page 0
     * B1 = page 1
     * ...
     * B7 = page 7
     */
    oled_write_command(0xB0 | line);
}


void oled_goto_column(uint8_t column)
{
    if (column > 127)
    {
        column = 127;
    }

    /*
     * Set lower nibble of column address.
     *
     * Commands 00h - 0Fh
     */
    oled_write_command(column & 0x0F);


    /*
     * Set upper nibble of column address.
     *
     * Commands 10h - 1Fh
     */
    oled_write_command(0x10 | (column >> 4));
}


void oled_clear(void)
{
    uint8_t page;
    uint8_t column;

    /*
     * 128 x 64 display
     *
     * 8 pages x 128 columns.
     */

    for (page = 0; page < 8; page++)
    {
        oled_goto_line(page);
        oled_goto_column(0);

        for (column = 0; column < 128; column++)
        {
            oled_write_data(0x00);
        }
    }

    /*
     * Return cursor to upper-left.
     */
    oled_goto_line(0);
    oled_goto_column(0);
}


void oled_write_char(char c)
{
    uint8_t i;
    uint8_t index;
    uint8_t column;

    /*
     * Font currently contains:
     *
     * ASCII 32 (' ')
     * through
     * ASCII 90 ('Z')
     */

    if (c < ' ' || c > 'Z')
    {
        c = '?';
    }


    /*
     * Convert ASCII number into
     * font array index.
     *
     * Example:
     *
     * 'A' = ASCII 65
     * 65 - 32 = 33
     */
    index = c - ' ';


    /*
     * Character consists of
     * five vertical pixel columns.
     */
    for (i = 0; i < 5; i++)
    {
        column =
            pgm_read_byte(&font5x7[index][i]);

        oled_write_data(column);
    }


    /*
     * One empty column creates
     * spacing between characters.
     */
    oled_write_data(0x00);
}


void oled_print(const char *text)
{
    while (*text)
    {
        oled_write_char(*text);

        text++;
    }
}


void oled_printf(const char *format, ...)
{
    /*
     * Temporary RAM buffer containing
     * the formatted string.
     */
    char buffer[64];

    va_list args;


    /*
     * Generate formatted string.
     */
    va_start(args, format);

    vsnprintf(
        buffer,
        sizeof(buffer),
        format,
        args
    );

    va_end(args);


    /*
     * Send resulting string
     * to OLED.
     */
    oled_print(buffer);
}
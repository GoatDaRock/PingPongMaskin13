#include <avr/io.h>
#include "spi.h"


/*
 * ATmega162 hardware SPI:
 *
 * PB4 = SS
 * PB5 = MOSI
 * PB6 = MISO
 * PB7 = SCK
 */


/* Display chip select */
#define DISP_CS_PORT    PORTB
#define DISP_CS_DDR     DDRB
#define DISP_CS_PIN     PB4


/* I/O board AVR chip select */
#define IO_CS_PORT      PORTE
#define IO_CS_DDR       DDRE
#define IO_CS_PIN       PE0


void spi_init(void)
{
    /*
     * PB4 = output
     * PB5 = MOSI output
     * PB7 = SCK output
     */
    DDRB |= (1 << PB4) |
            (1 << PB5) |
            (1 << PB7);

    /*
     * PB6 = MISO input
     */
    DDRB &= ~(1 << PB6);


    /*
     * IO_CS output
     */
    IO_CS_DDR |= (1 << IO_CS_PIN);


    /*
     * Initially no slave selected.
     */
    spi_deselect_all();


    /*
     * Enable SPI.
     * Configure ATmega162 as master.
     *
     * SPR1 = 0
     * SPR0 = 0
     *
     * SPI clock = f_CPU / 4
     */
    SPCR = (1 << SPE) |
           (1 << MSTR);
}


void spi_deselect_all(void)
{
    /*
     * Chip selects are active LOW.
     */
    DISP_CS_PORT |= (1 << DISP_CS_PIN);
    IO_CS_PORT   |= (1 << IO_CS_PIN);
}


void spi_select_slave(uint8_t slave)
{
    /*
     * First deselect everybody.
     */
    spi_deselect_all();


    switch (slave)
    {
        case SPI_SLAVE_DISPLAY:

            DISP_CS_PORT &= ~(1 << DISP_CS_PIN);

            break;


        case SPI_SLAVE_IO:

            IO_CS_PORT &= ~(1 << IO_CS_PIN);

            break;


        default:

            /*
             * Invalid slave number.
             * Nobody remains selected.
             */
            break;
    }
}


uint8_t spi_transfer(uint8_t data)
{
    /*
     * Put byte into SPI Data Register.
     * This starts the transmission.
     */
    SPDR = data;


    /*
     * Wait until transmission is complete.
     */
    while (!(SPSR & (1 << SPIF)))
    {
    }


    /*
     * SPI simultaneously receives a byte.
     */
    return SPDR;
}


void spi_write(uint8_t data)
{
    /*
     * We don't care about received byte.
     */
    spi_transfer(data);
}


uint8_t spi_read(void)
{
    /*
     * Send dummy byte to generate
     * eight SPI clock pulses.
     */
    return spi_transfer(0x00);
}


void spi_write_n(uint8_t *data, uint8_t length)
{
    uint8_t i;

    for (i = 0; i < length; i++)
    {
        spi_write(data[i]);
    }
}


void spi_read_n(uint8_t *data, uint8_t length)
{
    uint8_t i;

    for (i = 0; i < length; i++)
    {
        data[i] = spi_read();
    }
}
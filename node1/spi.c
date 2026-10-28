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


/* CAN controller chip select */
#define CAN_CTRL_PORT  PORTD
#define CAN_CTRL_DDR   DDRD
#define CAN_CTRL_PIN   PD4


/* Display chip select */
#define DISP_CS_PORT   PORTB
#define DISP_CS_DDR    DDRB
#define DISP_CS_PIN    PB4


/* I/O board AVR chip select */
#define IO_CS_PORT     PORTE
#define IO_CS_DDR      DDRE
#define IO_CS_PIN      PE0


void spi_init(void)
{
    /*
     * Hardware SPI pins:
     *
     * PB4 = SS   output
     * PB5 = MOSI output
     * PB6 = MISO input
     * PB7 = SCK  output
     */
    DDRB |= (1 << PB4) |
            (1 << PB5) |
            (1 << PB7);

    DDRB &= ~(1 << PB6);


    /*
     * Chip-select pins are outputs.
     */
    DISP_CS_DDR |= (1 << DISP_CS_PIN);
    IO_CS_DDR   |= (1 << IO_CS_PIN);
    CAN_CTRL_DDR |= (1 << CAN_CTRL_PIN);


    /*
     * Initially no SPI slave is selected.
     */
    spi_deselect_all();


    /*
     * Enable SPI.
     * Configure ATmega162 as SPI master.
     *
     * SPR1 = 0
     * SPR0 = 0
     *
     * SPI clock = f_CPU / 4
     *
     * With f_CPU = 4.9152 MHz:
     *
     * f_SCK = 4.9152 MHz / 4
     *       = 1.2288 MHz
     */
    SPCR = (1 << SPE) |
           (1 << MSTR);
}


void spi_deselect_all(void)
{
    /*
     * All chip-select signals are active LOW.
     *
     * HIGH = device not selected
     */
    DISP_CS_PORT |= (1 << DISP_CS_PIN);
    IO_CS_PORT   |= (1 << IO_CS_PIN);
    CAN_CTRL_PORT |= (1 << CAN_CTRL_PIN);
}


void spi_select_slave(uint8_t slave)
{
    /*
     * Deselect every slave before selecting
     * the requested device.
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


        case CAN_CTRL:

            CAN_CTRL_PORT &= ~(1 << CAN_CTRL_PIN);

            break;


        default:

            /*
             * Invalid slave.
             * All slaves remain deselected.
             */

            break;
    }
}


uint8_t spi_transfer(uint8_t data)
{
    /*
     * Writing to SPDR starts the SPI transfer.
     */
    SPDR = data;


    /*
     * Wait until the transfer has completed.
     *
     * SPIF = SPI Interrupt Flag.
     */
    while (!(SPSR & (1 << SPIF)))
    {
    }


    /*
     * SPI is full-duplex.
     *
     * While one byte was transmitted,
     * one byte was simultaneously received.
     */
    return SPDR;
}


void spi_write(uint8_t data)
{
    /*
     * Send one byte.
     * Ignore the received byte.
     */
    spi_transfer(data);
}


uint8_t spi_read(void)
{
    /*
     * SPI needs clock pulses to receive data.
     *
     * Therefore transmit a dummy byte.
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
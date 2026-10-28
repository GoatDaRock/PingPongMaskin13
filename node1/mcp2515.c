#include <stdint.h>
#include <avr/io.h>

#include "spi.h"
#include "mcp2515.h"

#define MCP2515_WRITE        0x02
#define MCP2515_READ         0x03
#define MCP2515_BIT_MODIFY   0x05
#define MCP2515_READ_STATUS  0xA0
#define MCP2515_RESET        0xC0
#define MCP2515_RTS          0x80

uint8_t mcp2515_read(uint8_t address)
{
    uint8_t data;

    spi_select_slave(CAN_CTRL);

    spi_transfer(MCP2515_READ);
    spi_transfer(address);
    data = spi_transfer(0x00);

    spi_deselect_all();

    return data;
}

void mcp2515_write(uint8_t address, uint8_t data)
{
    spi_select_slave(CAN_CTRL);

    spi_transfer(MCP2515_WRITE);
    spi_transfer(address);
    spi_transfer(data);

    spi_deselect_all();
}

void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data)
{
    spi_select_slave(CAN_CTRL);

    spi_transfer(MCP2515_BIT_MODIFY);
    spi_transfer(address);
    spi_transfer(mask);
    spi_transfer(data);

    spi_deselect_all();
}

void mcp2515_reset(void)
{
    spi_select_slave(CAN_CTRL);

    spi_transfer(MCP2515_RESET);

    spi_deselect_all();
}

void mcp2515_request_to_send(bool TXB0, bool TXB1, bool TXB2)
{
    uint8_t tx_buffer = 0;

    if (TXB0) {
        tx_buffer |= 0x01;
    }

    if (TXB1) {
        tx_buffer |= 0x02;
    }

    if (TXB2) {
        tx_buffer |= 0x04;
    }

    spi_select_slave(CAN_CTRL);

    /*
     * RTS format is 1000 0nnn.
     * tx_buffer already contains the nnn bit mask.
     */
    spi_transfer(MCP2515_RTS | tx_buffer);

    spi_deselect_all();
}

uint8_t mcp2515_read_status(void)
{
    uint8_t status;

    spi_select_slave(CAN_CTRL);

    spi_transfer(MCP2515_READ_STATUS);
    status = spi_transfer(0x00);

    spi_deselect_all();

    return status;
}

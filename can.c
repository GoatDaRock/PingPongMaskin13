#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>

#include "mcp2515.h"
#include "can.h"

/* MCP2515 registers */
#define CANSTAT      0x0E
#define CANCTRL      0x0F
#define CANINTF      0x2C

#define TXB0CTRL     0x30
#define TXB0SIDH     0x31
#define TXB0SIDL     0x32
#define TXB0DLC      0x35
#define TXB0D0       0x36

#define RXB0SIDH     0x61
#define RXB0SIDL     0x62
#define RXB0DLC      0x65
#define RXB0D0       0x66

#define RX0IF        0x01
#define TXREQ        0x08
#define CNF1         0x2A
#define CNF2         0x29
#define CNF3         0x28
#define CAN_CONFIG_MODE      0x80
#define CAN_LOOPBACK_MODE    0x40	


bool can_init(void)
{
    uint8_t status;

    /*
     * RESET places the MCP2515 in Configuration mode.
     */
    
	mcp2515_write(0x36, 0xAF);
	uint8_t value2 = mcp2515_read(0x36);
	printf("Read back value from MCP2515 register 0x36: 0x%02X\r\n", value2);

	status = mcp2515_read(CANSTAT);
	printf("CANSTAT before reset = 0x%02X\r\n", status);

	mcp2515_reset();
	
	_delay_ms(10); // Wait for the MCP2515 to reset
	
	uint8_t value3 = mcp2515_read(CANSTAT);
	printf("CANSTAT after reset = 0x%02X\r\n", value3);

	//printf("CANTRL after reset = 0x%02X\r\n", mcp2515_read(CANCTRL));

    /*
     * Verify that SPI communication works and that the
     * controller actually reached Configuration mode.
     *
     * A finite loop is used so a wiring/SPI fault does
     * not lock the program forever.
     */
    // for (uint16_t i = 0; i < 1000; i++)
    // {
    //     status = mcp2515_read(CANSTAT);

    //     if ((status & 0xE0) == CAN_CONFIG_MODE)
	mcp2515_write(CANCTRL, CAN_CONFIG_MODE);

	_delay_ms(10); // Wait for the MCP2515 to enter configuration mode
    //     {
    //         break;
    //     }
    //}

	status = mcp2515_read(CANSTAT);

	
    if ((status & 0xE0) != CAN_CONFIG_MODE)
    {
		printf("Failed to enter CAN configuration mode. Status: 0x%02X\r\n", status);
        return false;
    }
	printf("Successfully entered CAN configuration mode. Status: 0x%02X\r\n", status);

	//Fosc = 16MHz 
	mcp2515_write(CNF1, 0x03); //  
    mcp2515_write(CNF2, 0xB1); 
    mcp2515_write(CNF3, 0x05);
    /*
     * Request Loopback mode:
     * CANCTRL.REQOP<2:0> = 010.
     */
    mcp2515_write(CANCTRL, CAN_LOOPBACK_MODE);
	printf("CANSTAT after setting loopback mode = 0x%02X\r\n", mcp2515_read(CANSTAT));

    /*
     * Verify the actual mode through CANSTAT.OPMOD.
     */
    for (uint16_t i = 0; i < 1000; i++)
    {
        status = mcp2515_read(CANSTAT);

        if ((status & 0xE0) == CAN_LOOPBACK_MODE)
        {
            return true;
        }
    }
	printf("Failed to enter CAN loopback mode. Status: 0x%02X\r\n", status);

    return false;
}

bool can_send_message(const can_message_t *message)
{
    uint8_t i;

    if (message == 0)
    {
        return false;
    }

    /*
     * This implementation uses standard 11-bit IDs only.
     */
    if (message->id > 0x7FF)
    {
        return false;
    }

    /*
     * Classical CAN on MCP2515 supports 0-8 data bytes.
     */
    if (message->length > 8)
    {
        return false;
    }

    /*
     * Do not overwrite TXB0 while a transmission is pending.
     */
    if (mcp2515_read(TXB0CTRL) & TXREQ)
    {
        return false;
    }

    /*
     * Load standard 11-bit identifier.
     */
    mcp2515_write(TXB0SIDH, (uint8_t)(message->id >> 3));
    mcp2515_write(TXB0SIDL, (uint8_t)((message->id & 0x07) << 5));

    /*
     * Load Data Length Code.
     */
    mcp2515_write(TXB0DLC, message->length & 0x0F);

    /*
     * Load payload.
     */
    for (i = 0; i < message->length; i++)
    {
        mcp2515_write(TXB0D0 + i, message->data[i]);
    }

    /*
     * Request transmission from TXB0.
     */
    mcp2515_request_to_send(true, false, false);

    return true;
}

bool can_receive_message(can_message_t *message)
{
    uint8_t i;
    uint8_t sidh;
    uint8_t sidl;

    if (message == 0)
    {
        return false;
    }

    /*
     * CANINTF bit 0 is RX0IF.
     * Read CANINTF directly instead of interpreting
     * the special READ STATUS byte.
     */
    if (!(mcp2515_read(CANINTF) & RX0IF))
    {
        return false;
    }

    sidh = mcp2515_read(RXB0SIDH);
    sidl = mcp2515_read(RXB0SIDL);

    message->id =
        ((uint32_t)sidh << 3) |
        ((uint32_t)sidl >> 5);

    message->length = mcp2515_read(RXB0DLC) & 0x0F;

    if (message->length > 8)
    {
        message->length = 8;
    }

    for (i = 0; i < message->length; i++)
    {
        message->data[i] = mcp2515_read(RXB0D0 + i);
    }

    /*
     * Clear CANINTF.RX0IF.
     */
    mcp2515_bit_modify(CANINTF, RX0IF, 0x00);

    return true;
}

#include "mcp2515.h"
#include "can.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void can_init(void)
{
	/*
	 * Reset the MCP2515 to enter configuration mode.
	 */
	mcp2515_reset();

	/*
	 * Set the CAN controller to normal mode.
	 */
	mcp2515_write(CANCTRL, CAN_OPERATIONAL_MODE);
}

void can_send_message(can_message_t message)
{
	/*
	 * Load the message into the transmit buffer.
	 */
	mcp2515_write(0x31, (message.id >> 3) & 0xFF); // TXB0SIDH
	mcp2515_write(0x32, (message.id << 5) & 0xE0); // TXB0SIDL
	mcp2515_write(0x35, message.length & 0x0F);    // TXB0DLC

	for (uint8_t i = 0; i < message.length; i++)
	{
		mcp2515_write(0x36 + i, message.data[i]); // TXB0Dm
	}

	/*
	 * Request to send the message.
	 */
	mcp2515_request_to_send(true, false, false);
}

can_message_t can_receive_message(void)
{
	can_message_t message;

	/*
	 * Check if a message has been received.
	 */
	uint8_t status = mcp2515_read_status();
	if (!(status & 0x01)) // RX0IF
	{
		return (can_message_t){0, "No message received", 20}; // No message received
	}

	/*
	 * Read the message from the receive buffer.
	 */
	message.id = (mcp2515_read(0x61) << 3) | (mcp2515_read(0x62) >> 5); // RXB0SIDH and RXB0SIDL
	message.length = mcp2515_read(0x65) & 0x0F;                        // RXB0DLC

	for (uint8_t i = 0; i < message.length; i++)
	{
		message.data[i] = mcp2515_read(0x66 + i); // RXB0Dm
	}

	/*
	 * Clear the receive flag.
	 */
	mcp2515_bit_modify(0x2C, 0x01, 0x00); // CANINTF

	return message;
}
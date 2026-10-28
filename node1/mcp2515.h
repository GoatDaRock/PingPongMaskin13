#ifndef MCP2515_H
#define MCP2515_H

#include <stdint.h>
#include <stdbool.h>

/* Common MCP2515 register addresses used by the CAN driver */
#define MCP2515_CANSTAT    0x0E
#define MCP2515_CANCTRL    0x0F
#define MCP2515_CANINTF    0x2C
#define MCP2515_TXB0CTRL   0x30

uint8_t mcp2515_read(uint8_t address);
void mcp2515_write(uint8_t address, uint8_t data);
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data);
void mcp2515_reset(void);
void mcp2515_request_to_send(bool TXB0, bool TXB1, bool TXB2);
uint8_t mcp2515_read_status(void);

#endif

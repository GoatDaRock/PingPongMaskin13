#ifndef CAN_H
#define CAN_H

#include <stdint.h>
#include <stdbool.h>

#define CAN_NORMAL_MODE      0x00
#define CAN_LOOPBACK_MODE    0x40
#define CAN_CONFIG_MODE      0x80

/*
 * This driver currently implements standard 11-bit CAN identifiers.
 * MCP2515 Classical CAN frames contain at most 8 data bytes.
 */
typedef struct can_message
{
    uint32_t id;
    uint8_t data[8];
    uint8_t length;
} can_message_t;

/*
 * Returns true if the MCP2515 successfully enters loopback mode.
 */
bool can_init(void);

/*
 * Returns false if the message is invalid or TXB0 is busy.
 */
bool can_send_message(const can_message_t *message);

/*
 * Non-blocking receive.
 * Returns true if a message was available in RXB0.
 */
bool can_receive_message(can_message_t *message);

#endif

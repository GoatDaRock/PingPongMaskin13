#ifndef CAN_H
#define CAN_H

#include "mcp2515.h"

#define CAN_OPERATIONAL_MODE 0x00

typedef struct can_message
{
	uint32_t id;        // 11-bit or 29-bit identifier
	uint8_t data[128];    // Data bytes (0-128 bytes)
	uint8_t length;     // Number of data bytes (0-128)
} can_message_t;

void can_init(void);
void can_send_message(can_message_t message);
can_message_t can_receive_message(void);

#endif
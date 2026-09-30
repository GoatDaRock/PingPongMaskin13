#ifndef CAN_H
#define CAN_H

#include "mcp2515.h"

#define CAN_OPERATIONAL_MODE 0x00
#define CAN_LOOPBACK_MODE 0x40
#define CANSTAT 0x0E

typedef struct can_message
{
	uint32_t id;        // 11-bit or 29-bit identifier
	uint8_t length;     // Length of data in bytes (0-8)
	uint8_t data[8]; 
} can_message_t;

void can_init(void);
void can_send_message(can_message_t message);
can_message_t can_receive_message(void);

can_message_t make_can_message(uint32_t id, const char *data, int length);

#endif



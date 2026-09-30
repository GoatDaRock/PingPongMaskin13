#include "mcp2515.h"
#include "can.h"

#include <stdint.h>
#include <stdio.h>

#define CANCTRL   0x0F
#define CANSTAT   0x0E
#define CANINTF   0x2C

#define TXB0SIDH  0x31
#define TXB0SIDL  0x32
#define TXB0DLC   0x35
#define TXB0D0    0x36

#define RXB0SIDH  0x61
#define RXB0SIDL  0x62
#define RXB0DLC   0x65
#define RXB0D0    0x66

#define CAN_LOOPBACK_MODE 0x40


void can_init(void)
{
/*
	mcp2515_reset();

    /* Request loopback mode */
    mcp2515_write(CANCTRL, CAN_LOOPBACK_MODE);

    /* Wait until loopback mode is active */
    //while ((mcp2515_read(CANSTAT) & 0xE0) != CAN_LOOPBACK_MODE)
    //{

	//}
}


void can_send_message(can_message_t message)
{
    uint8_t i;

    if (message.length > 8)
    {
        message.length = 8;
    }

    /* Standard 11-bit identifier */
    mcp2515_write(TXB0SIDH,
                  (uint8_t)(message.id >> 3));

    mcp2515_write(TXB0SIDL,
                  (uint8_t)((message.id & 0x07) << 5));

    /* Data length */
    mcp2515_write(TXB0DLC,
                  message.length & 0x0F);

    /* Data */
    for (i = 0; i < message.length; i++)
    {
        mcp2515_write(TXB0D0 + i,
                      message.data[i]);
    }

    /* Transmit TX buffer 0 */
    mcp2515_request_to_send(true, false, false);
}


can_message_t can_receive_message(void)
{
    can_message_t message;

    uint8_t i;
    uint8_t sidh;
    uint8_t sidl;


    /* Wait for message in RX buffer 0 */
    while (!(mcp2515_read(CANINTF) & 0x01))
    {
    }


    sidh = mcp2515_read(RXB0SIDH);
    sidl = mcp2515_read(RXB0SIDL);

    message.id =
        ((uint16_t)sidh << 3) |
        (sidl >> 5);


    message.length =
        mcp2515_read(RXB0DLC) & 0x0F;

    if (message.length > 8)
    {
        message.length = 8;
    }


    for (i = 0; i < message.length; i++)
    {
        message.data[i] =
            mcp2515_read(RXB0D0 + i);
    }


    /* Clear RX0IF */
    mcp2515_bit_modify(
        CANINTF,
        0x01,
        0x00
    );


    return message;
}
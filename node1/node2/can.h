#pragma once

#include <stdint.h>

/*
 * Node 2 CAN timing for the TTK4155 lab.
 *
 * This configuration matches Node 1:
 *   MCP2515 Fosc = 16 MHz
 *   CNF1 = 0x03
 *   CNF2 = 0xB1
 *   CNF3 = 0x05
 *
 * Result on both nodes:
 *   CAN bit rate = 125 kbit/s
 *   TQ           = 0.5 us
 *   SYNC         = 1 TQ
 *   PROP         = 2 TQ
 *   PHASE1       = 7 TQ
 *   PHASE2       = 6 TQ
 *   SJW          = 1 TQ
 *
 * For the SAM3X CAN_BR fields, the register stores
 * segment length minus 1:
 *   propag = 1  -> 2 TQ
 *   phase1 = 6  -> 7 TQ
 *   phase2 = 5  -> 6 TQ
 *   sjw    = 0  -> 1 TQ
 *
 * With MCK = 84 MHz:
 *   TQ = (BRP + 1) / MCK
 *      = (41 + 1) / 84 MHz
 *      = 0.5 us
 */

typedef struct CanInit CanInit;

struct CanInit {
    uint8_t phase2;
    uint8_t propag;
    uint8_t phase1;
    uint8_t sjw;
    uint8_t brp;
    uint8_t smp;
};

/* Exact timing used to match Node 1. */
#define CAN_NODE1_125K_TIMING ((CanInit){ \
    .phase2 = 5, \
    .propag = 1, \
    .phase1 = 6, \
    .sjw    = 0, \
    .brp    = 41, \
    .smp    = 0 \
})

/*
 * Initialize CAN0.
 * If rxInterrupt != 0, an interrupt is enabled when a message is received.
 */
void can_init(CanInit init, uint8_t rxInterrupt);


/* Strict-aliasing-safe reinterpret-cast. */
#define union_cast(type, x) \
    (((union { \
        typeof(x) a; \
        type b; \
    })x).b)


typedef struct Byte8 Byte8;

struct Byte8 {
    uint8_t bytes[8];
};


typedef struct CanMsg CanMsg;

struct CanMsg {
    uint8_t id;
    uint8_t length;

    union {
        uint8_t  byte[8];
        uint32_t dword[2];
        Byte8    byte8;
    };
};


/* Send a CAN message. */
void can_tx(CanMsg m);

/*
 * Receive a CAN message.
 * Returns 0 if no message is available, 1 otherwise.
 */
uint8_t can_rx(CanMsg* m);

/* Print a CAN message using printf(). */
void can_printmsg(CanMsg m);

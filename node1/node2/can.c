#include "sam.h"
#include "can.h"
#include <stdio.h>


#define txMailbox 0
#define rxMailbox 1


void can_printmsg(CanMsg m)
{
    printf("CanMsg(id:%d, length:%d, data:{", m.id, m.length);

    if (m.length) {
        printf("%d", m.byte[0]);
    }

    for (uint8_t i = 1; i < m.length; i++) {
        printf(", %d", m.byte[i]);
    }

    printf("})\n");
}


/*
 * Build the SAM3X CAN_BR register explicitly.
 *
 * CAN_BR field positions:
 *   PHASE2 : bits  0..2
 *   PROPAG : bits  4..6
 *   PHASE1 : bits  8..10
 *   SJW    : bits 12..13
 *   BRP    : bits 16..22
 *   SMP    : bit  24
 *
 * Using an explicit register build avoids depending on compiler-specific
 * bit-field packing.
 */
static uint32_t can_build_baudrate_register(CanInit init)
{
    uint32_t reg = 0;

    reg |= ((uint32_t)(init.phase2 & 0x07u) << 0);
    reg |= ((uint32_t)(init.propag & 0x07u) << 4);
    reg |= ((uint32_t)(init.phase1 & 0x07u) << 8);
    reg |= ((uint32_t)(init.sjw    & 0x03u) << 12);
    reg |= ((uint32_t)(init.brp    & 0x7Fu) << 16);
    reg |= ((uint32_t)(init.smp    & 0x01u) << 24);

    return reg;
}


void can_init(CanInit init, uint8_t rxInterrupt)
{
    /* Disable CAN while changing bit timing. */
    CAN0->CAN_MR &= ~CAN_MR_CANEN;

    /* Clear status register by reading it. */
    __attribute__((unused)) uint32_t ul_status = CAN0->CAN_SR;

    /*
     * Select CAN0 RX/TX on PA1/PA0.
     * Peripheral A:
     *   PA1 = CANRX0
     *   PA0 = CANTX0
     */
    PIOA->PIO_ABSR &= ~(PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0);

    /* Disable normal PIO control so CAN0 can use the pins. */
    PIOA->PIO_PDR = PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0;

    /* Enable pull-ups on the CAN peripheral pins. */
    PIOA->PIO_PUER = PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0;

    /*
     * Enable the CAN0 peripheral clock.
     *
     * DIV = 0 means peripheral clock = MCK.
     * With the normal Arduino Due setup, MCK = 84 MHz.
     */
    PMC->PMC_PCR =
        PMC_PCR_EN |
        (0u << PMC_PCR_DIV_Pos) |
        PMC_PCR_CMD |
        (ID_CAN0 << PMC_PCR_PID_Pos);

    PMC->PMC_PCER1 |= (1u << (ID_CAN0 - 32));

    /*
     * Match Node 1 CAN timing.
     *
     * For CAN_NODE1_125K_TIMING this becomes:
     *   CAN_BR = 0x00290615
     *
     * TQ = (41 + 1) / 84 MHz = 0.5 us
     *
     * Bit:
     *   SYNC   = 1 TQ
     *   PROP   = 2 TQ
     *   PHASE1 = 7 TQ
     *   PHASE2 = 6 TQ
     *   total  = 16 TQ = 8 us
     *
     * Bit rate = 1 / 8 us = 125 kbit/s.
     */
    CAN0->CAN_BR = can_build_baudrate_register(init);


    /* Configure transmit mailbox. */
    CAN0->CAN_MB[txMailbox].CAN_MID = CAN_MID_MIDE;
    CAN0->CAN_MB[txMailbox].CAN_MMR = CAN_MMR_MOT_MB_TX;

    /* Configure receive mailbox. */
    CAN0->CAN_MB[rxMailbox].CAN_MAM = 0; /* Accept all messages. */
    CAN0->CAN_MB[rxMailbox].CAN_MID = CAN_MID_MIDE;
    CAN0->CAN_MB[rxMailbox].CAN_MMR = CAN_MMR_MOT_MB_RX;
    CAN0->CAN_MB[rxMailbox].CAN_MCR |= CAN_MCR_MTCR;

    if (rxInterrupt) {
        CAN0->CAN_IER |= (1u << rxMailbox);
        NVIC_EnableIRQ((IRQn_Type)ID_CAN0);
    }

    /* Enable CAN after configuration is complete. */
    CAN0->CAN_MR |= CAN_MR_CANEN;
}


void can_tx(CanMsg m)
{
    while (!(CAN0->CAN_MB[txMailbox].CAN_MSR & CAN_MSR_MRDY)) {
    }

    /* Set message ID. */
    CAN0->CAN_MB[txMailbox].CAN_MID =
        CAN_MID_MIDvA(m.id) | CAN_MID_MIDE;

    /* Classical CAN payload is at most 8 bytes. */
    m.length = (m.length > 8) ? 8 : m.length;

    CAN0->CAN_MB[txMailbox].CAN_MDL = m.dword[0];
    CAN0->CAN_MB[txMailbox].CAN_MDH = m.dword[1];

    CAN0->CAN_MB[txMailbox].CAN_MCR =
        ((uint32_t)m.length << CAN_MCR_MDLC_Pos) |
        CAN_MCR_MTCR;
}


uint8_t can_rx(CanMsg* m)
{
    if (!(CAN0->CAN_MB[rxMailbox].CAN_MSR & CAN_MSR_MRDY)) {
        return 0;
    }

    m->id = (uint8_t)(
        (CAN0->CAN_MB[rxMailbox].CAN_MID & CAN_MID_MIDvA_Msk)
        >> CAN_MID_MIDvA_Pos
    );

    m->length = (uint8_t)(
        (CAN0->CAN_MB[rxMailbox].CAN_MSR & CAN_MSR_MDLC_Msk)
        >> CAN_MSR_MDLC_Pos
    );

    m->dword[0] = CAN0->CAN_MB[rxMailbox].CAN_MDL;
    m->dword[1] = CAN0->CAN_MB[rxMailbox].CAN_MDH;

    /* Reset mailbox for the next received frame. */
    CAN0->CAN_MB[rxMailbox].CAN_MMR = CAN_MMR_MOT_MB_RX;
    CAN0->CAN_MB[rxMailbox].CAN_MCR |= CAN_MCR_MTCR;

    return 1;
}


/*
 * Example CAN interrupt handler:
 *
 * void CAN0_Handler(void)
 * {
 *     uint32_t can_sr = CAN0->CAN_SR;
 *
 *     if (can_sr & (1u << rxMailbox)) {
 *         CanMsg msg;
 *         if (can_rx(&msg)) {
 *             can_printmsg(msg);
 *         }
 *     }
 *
 *     NVIC_ClearPendingIRQ((IRQn_Type)ID_CAN0);
 * }
 */

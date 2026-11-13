#include "sam.h"
#include <stdint.h>

void encoder_init(void)
{
    // 1. Enable clocks for PIOC and TC2 channel 0
    PMC->PMC_PCER0 = (1u << ID_PIOC);
    PMC->PMC_PCER1 = (1u << (ID_TC6 - 32));

    // 2. Configure PC25 and PC26 as peripheral B
    // PC25 = TIOA6 (encoder A)
    // PC26 = TIOB6 (encoder B)
    PIOC->PIO_ABSR |= (1u << 25) | (1u << 26);
    PIOC->PIO_PDR = (1u << 25) | (1u << 26);

    // 3. Disable counter during configuration
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;

    // 4. Enable quadrature decoding and position mode
    TC2->TC_BMR =
        TC_BMR_QDEN |
        TC_BMR_POSEN;

    // 5. Select XC0 clock and capture mode
    // Rising external trigger, TIOA selected
    TC2->TC_CHANNEL[0].TC_CMR =
        TC_CMR_TCCLKS_XC0 |
        TC_CMR_ETRGEDG_RISING |
        TC_CMR_ABETRG;

    // 6. Start encoder counter
    TC2->TC_CHANNEL[0].TC_CCR =
        TC_CCR_CLKEN | TC_CCR_SWTRG;
}

int32_t encoder_read(void)
{
    return TC2->TC_CHANNEL[0].TC_CV;
}
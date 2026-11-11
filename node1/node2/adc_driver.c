
#include "sam.h"
#include "adc_driver.h"

#define ADC_CHANNEL 7u

void adc_init(void)
{
    // Enable peripheral clocks
    PMC->PMC_PCER0 = (1u << ID_PIOA);
    PMC->PMC_PCER1 = (1u << (ID_ADC - 32));

    // PA16 / Arduino A0 as ADC input
    PIOA->PIO_PDR = (1u << 16);

    // Reset ADC
    ADC->ADC_CR = ADC_CR_SWRST;

    /*
     * MCK = 84 MHz
     * PRESCAL = 3 -> ADC clock = 10.5 MHz
     * STARTUP = 8 -> 512 ADC clock cycles
     * TRACKTIM = 15 -> 16 ADC clock cycles
     * TRANSFER = 1 -> 5 ADC clock cycles
     *
     * Software trigger, 12-bit resolution.
     */
    ADC->ADC_MR =
        (3u  << 8)  |
        (8u  << 16) |
        (15u << 24) |
        (1u  << 28);

    // Use only channel 7
    ADC->ADC_CHDR = 0xFFFFu;
    ADC->ADC_CHER = (1u << ADC_CHANNEL);
}

uint16_t adc_read(void)
{
    // Start conversion
    ADC->ADC_CR = ADC_CR_START;

    // Wait for end of conversion
    while (!(ADC->ADC_ISR & (1u << ADC_CHANNEL)))
    {
    }

    // Read 12-bit ADC result
    return (uint16_t)ADC->ADC_CDR[ADC_CHANNEL];
}
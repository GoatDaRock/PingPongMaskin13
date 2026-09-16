#include <avr/io.h>
#include <stdint.h>
#include "adc.h"

#define ADC_BASE ((volatile uint8_t *)0x1000)


void adc_init(void)
{
    /*
     * Generate MAX156 clock using Timer1.
     *
     * ATmega162:
     * PD5 = OC1A
     *
     * f_CPU = 4.9152 MHz
     * OCR1A = 1
     *
     * f_OC1A = 4.9152 MHz / 4
     *         = 1.2288 MHz
     */

    DDRD |= (1 << PD5);

    // Toggle OC1A on compare match
    TCCR1A = (1 << COM1A0);

    // CTC mode, prescaler = 1
    TCCR1B = (1 << WGM12) | (1 << CS10);

    OCR1A = 1;
}


adc_data_t adc_read_all(void)
{
    adc_data_t data;

    /*
     * Write to ADC address.
     * XMEM generates /CS and /WR.
     * This starts the conversion.
     */
    *ADC_BASE = 0;

    /*
     * Wait long enough for conversion to finish.
     * Temporary solution - we can use /BUSY later.
     */
    for (volatile uint16_t i = 0; i < 100; i++)
    {
    }

    /*
     * Consecutive reads generate consecutive /RD pulses.
     *
     * Your wiring:
     *
     * AIN0 = JOY_Y
     * AIN1 = JOY_X
     * AIN2 = PAD_X
     * AIN3 = PAD_Y
     */

    data.joy_y = *ADC_BASE;
    data.joy_x = *ADC_BASE;
    data.pad_x = *ADC_BASE;
    data.pad_y = *ADC_BASE;

    return data;
}
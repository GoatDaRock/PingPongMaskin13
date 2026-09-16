#ifndef ADC_H
#define ADC_H

#include <stdint.h>

typedef struct
{
    uint8_t joy_y;   // AIN0
    uint8_t joy_x;   // AIN1
    uint8_t pad_x;   // AIN2
    uint8_t pad_y;   // AIN3
} adc_data_t;

void adc_init(void);
adc_data_t adc_read_all(void);

#endif
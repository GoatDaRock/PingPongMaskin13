#include <avr/io.h>
#include <stdint.h>
#include "sram.h"

void xmem_init(void)
{
    // Enable external memory interface
    MCUCR |= (1 << SRE);

    // Release PC4-PC7 from XMEM.
    // Keep PC0-PC3 as A8-A11.
    SFIOR &= ~((1 << XMM2) | (1 << XMM1) | (1 << XMM0));
    SFIOR |= (1 << XMM2);
}

void sram_write(uint16_t address, uint8_t data)
{
    volatile uint8_t *ptr = (volatile uint8_t *)address;

    *ptr = data;
}

uint8_t sram_read(uint16_t address)
{
    volatile uint8_t *ptr = (volatile uint8_t *)address;

    return *ptr;
}

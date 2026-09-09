#ifndef SRAM_H
#define SRAM_H

#include <stdint.h>

void xmem_init(void);

void sram_write(uint16_t address, uint8_t data);
uint8_t sram_read(uint16_t address);

#endif
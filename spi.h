#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#define SPI_SLAVE_DISPLAY   0
#define SPI_SLAVE_IO        1

void spi_init(void);

void spi_select_slave(uint8_t slave);
void spi_deselect_all(void);

uint8_t spi_transfer(uint8_t data);
void spi_write(uint8_t data);
uint8_t spi_read(void);

void spi_write_n(uint8_t *data, uint8_t length);
void spi_read_n(uint8_t *data, uint8_t length);

#endif
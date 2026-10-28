#ifndef OLED_H
#define OLED_H

#include <stdint.h>

void oled_init(void);

void oled_write_command(uint8_t command);
void oled_write_data(uint8_t data);

void oled_goto_line(uint8_t line);
void oled_goto_column(uint8_t column);

void oled_clear(void);

void oled_write_char(char c);
void oled_print(const char *text);
void oled_printf(const char *format, ...);

#endif
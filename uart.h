#ifndef UART_H_
#define UART_H_

#include <stdio.h>

void uart_init(void);
void uart_transmit(char data);
char uart_receive(void);

int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);

#endif

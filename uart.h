#ifndef UART_H_
#define UART_H_

void uart_init(void);

void uart_transmit(char data);

char uart_receive(void);

#endif
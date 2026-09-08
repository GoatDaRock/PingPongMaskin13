#include <avr/io.h>
#include <stdio.h>
#include "uart.h"

void uart_init(void)
{
    // 4.9152 MHz clock, 9600 baud
    UBRR1H = 0;
    UBRR1L = 31;

    // Enable USART1 receiver and transmitter
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);

    // Asynchronous, 8 data bits, no parity, 1 stop bit
    UCSR1C = (1 << URSEL1) |
             (1 << UCSZ11) |
             (1 << UCSZ10);
}

void uart_transmit(char data)
{
    // Wait until transmit buffer is ready
    while (!(UCSR1A & (1 << UDRE1)))
    {
    }

    // Send character
    UDR1 = data;
}

char uart_receive(void)
{
    // Wait until data has been received
    while (!(UCSR1A & (1 << RXC1)))
    {
    }

    // Return received character
    return UDR1;
}

int uart_putchar(char c, FILE *stream)
{
    uart_transmit(c);
    return 0;
}

int uart_getchar(FILE *stream)
{
    return uart_receive();
}
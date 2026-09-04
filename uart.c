#include <avr/io.h>
#include "uart.h"

void uart_init(void)
{
    // 9600 baud with 4.9152 MHz clock
    UBRR1H = 0;
    UBRR1L = 31;

    // Enable receiver and transmitter for USART1
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);

    // 8 data bits, no parity, 1 stop bit
    UCSR1C = (1 << URSEL1) |
             (1 << UCSZ11) |
             (1 << UCSZ10);
}

void uart_transmit(char data)
{
    while (!(UCSR1A & (1 << UDRE1)))
    {
    }

    UDR1 = data;
}

char uart_receive(void)
{
    while (!(UCSR1A & (1 << RXC1)))
    {
    }

    return UDR1;
}
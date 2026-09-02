#define F_CPU 4915200UL

#include <avr/io.h>
#include <util/delay.h>

void uart_init(void)
{
    // 9600 baud at 4.9152 MHz
    UBRR1H = 0;
    UBRR1L = 31;

    // Enable transmitter on USART1
    UCSR1B = (1 << TXEN1);

    // 8 data bits, no parity, 1 stop bit
    UCSR1C = (1 << URSEL1)
           | (1 << UCSZ11)
           | (1 << UCSZ10);
}

void uart_send(char c)
{
    while (!(UCSR1A & (1 << UDRE1)))
    {
    }

    UDR1 = c;
}

int main(void)
{
    uart_init();

    while (1)
    {
        uart_send('A');
    }

    return 0;
}
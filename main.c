#include <avr/io.h>
#include <stdio.h>
#include "uart.h"

int main(void)
{
    uart_init();

    // Connect printf() and standard input to UART
    fdevopen(uart_putchar, uart_getchar);

    while (1)
    {
        printf("Hello");
    }

    return 0;
}
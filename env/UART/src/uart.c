/* Global includes */
#include <stdint.h> /* int8_t */
#include <avr/io.h>

/* Local includes */
#include "uart.h"

/* Defines */

/* We'll just assume the clock is 16MHz (arduino uno mini) */
#define UART_CLOCK_SPEED   16000000U


static uint32_t _uart_find_baudrate_clock (void)
{
   uint32_t clock = UART_CLOCK_SPEED;

   return clock;
}

int8_t uart_init(uint32_t i_baudrate)
{
    int8_t ret = 0;
    uint32_t clock = _uart_find_baudrate_clock();
    (void) i_baudrate;

    /* Set baudrate based on internal clock */
    uint16_t brr = ((clock / i_baudrate) >> 4) -1;

    UBRR0H = (uint8_t) (brr >> 8);
    UBRR0L = (uint8_t) (brr >> 0);

    /* Enable transmitter and receiver */
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (1<<USBS0) | (3<<UCSZ00);


    while (!(UCSR0A & (1<<UDRE0)))
       ;
    /* Put data into buffer, sends the data */
    UDR0 = 'a';

    return ret;
}

int8_t uart_send_message(char *m, uint16_t i_size)
{
    int8_t ret = 0;

    if (0 == m)
        ret = -1;

    if (0 == ret)
    {
        if (i_size > 0)
            (void) m;
    }

    return ret;
}


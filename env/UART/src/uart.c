/* Global includes */
#include <stdint.h> /* intX_t */
#include <string.h> /* memset */
#include <avr/io.h>
#include <util/delay.h>
#include <util/atomic.h>

/* Local includes */
#include "uart.h"

/* Defines */

/* We'll just assume the clock is 16MHz (arduino uno mini) */
#ifndef F_CPU
#define UART_CLOCK_SPEED   16000000U
#endif

#define UART_SIZE_BUFFER   256

/* Static definitions */
static uart_t_reg _uart_reg = { 0 };

static uint8_t _uart_tx_buffer[UART_SIZE_BUFFER];
static uint16_t _uart_tx_buffer_head;
static uint16_t _uart_tx_buffer_tail;

static void _uart_write (uint8_t data);
static uint8_t _uart_receive (void);
static void _uart_blink(uint16_t ms);

void _uart_tx_irq (void);

static uint32_t _uart_find_baudrate_clock (void)
{
#ifdef F_CPU
   return F_CPU;
#else
   return UART_CLOCK_SPEED;
#endif
}

int8_t uart_init(uart_t_reg *i_reg, uint32_t i_baudrate)
{
    int8_t ret = 0;
    uint32_t clock = _uart_find_baudrate_clock();

    /* Init local variables */
    _uart_tx_buffer_head = 0;
    _uart_tx_buffer_tail = 0;

    memset(_uart_tx_buffer, 0, UART_SIZE_BUFFER);
    memcpy(&_uart_reg, i_reg, sizeof(_uart_reg));

    /* Set baudrate based on internal clock */
    uint16_t brr = (uint16_t) ((clock / 4 / i_baudrate) - 1) / 2;
    *_uart_reg.ucsra = (1 << U2X0);
    //uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;

    *_uart_reg.ubrrh = (uint8_t) (brr >> 8);
    *_uart_reg.ubrrl = (uint8_t) (brr >> 0);

    /* Enable transmitter and receiver */
    *_uart_reg.ucsrb = (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
    *_uart_reg.ucsrc = (1<<USBS0) | (3<<UCSZ00);

    return ret;
}

int8_t uart_send_message(uint8_t *m, uint16_t i_size)
{
    int8_t ret = 0;
    uint16_t ii;

    if (0 == m)
        ret = -1;

//    _uart_write('h');
//    _uart_write('e');
//    _uart_write('l');
//    _uart_write('l');
//    _uart_write('o');

    if (0 == ret)
    {
        if (i_size > 0)
        {
            for (ii = 0; ii < i_size; ii++)
                _uart_write(m[ii]);
        }
    }

    return ret;
}

static void _uart_write (uint8_t data)
{
    //while ( !(*_uart_reg.ucsra & (1 << UDRE0)) );
    /* First check if data register is empty and we have nothing 
       else to write first */
    if ((_uart_tx_buffer_tail == _uart_tx_buffer_head) && (*_uart_reg.ucsra & (1 << UDRE0)))
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            *_uart_reg.udr = data;
            *_uart_reg.ucsra = (uint8_t) ( ((*_uart_reg.ucsra) & ((1 << U2X0) | (1 << MPCM0))) | (1 << TXC0) );
        }

        return;
    }

    /* Now we can fill the buffer */
    uint16_t ii = (_uart_tx_buffer_head + 1) % UART_SIZE_BUFFER;

    /* If the buffer is full */
    while (ii == _uart_tx_buffer_tail)
    {
        if (!(SREG & (1 << SREG_I)))
        {
            /* Interrupts are disabled, we need to poll */
            if (*_uart_reg.ucsra & (1 << UDRE0))
                _uart_tx_irq();
        }
        else
        {
            /* Nothing to do, interrupts will do the job */
        }
    }

    /* Put the data in the buffer */
    _uart_tx_buffer[_uart_tx_buffer_tail] = data;

    /* Update the head of the buffer atomically to avoid retransmission */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        _uart_tx_buffer_head = ii;
        *_uart_reg.ucsrb |= (1 << UDRIE0);
    }
}

static uint8_t _uart_receive (void)
{
    /* Wait for data to be received */
    while ( !(*_uart_reg.ucsra & (1 << RXC0)) );

    return *_uart_reg.udr;
}

static void _uart_blink(uint16_t ms)
{
        PORTB |= _BV(PORTB5);

        /*Wait 3000 ms */
        _delay_ms(ms);

        /*Set to zero the fifth bit of PORTB
         **Set to LOW the pin 13 */
        PORTB &= (uint8_t) ~_BV(PORTB5);

        /*Wait 3000 ms */
        _delay_ms(ms);
}

/* ISR */
void _uart_tx_irq (void)
{
    uint8_t c = _uart_tx_buffer[_uart_tx_buffer_tail];
    _uart_tx_buffer_tail = (_uart_tx_buffer_tail + 1) % UART_SIZE_BUFFER;

    /* Send the data */
    *_uart_reg.udr = c;
    *_uart_reg.ucsra = (uint8_t) ( ((*_uart_reg.ucsra) & ((1 << U2X0) | (1 << MPCM0))) | (1 << TXC0) );

    /* disable irq if empty */
    if (_uart_tx_buffer_head == _uart_tx_buffer_tail)
    {
        *_uart_reg.ucsrb &= (uint8_t) ~(1 << UDRIE0);
    }
}

ISR(USART_UDRE_vect) { _uart_tx_irq(); }


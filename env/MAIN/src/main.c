/* Global includes */
#include <stdint.h> /* int8_t */
#include <avr/io.h>
#include <avr/interrupt.h> /* sei */
#include <util/delay.h>

/* Local includes */
#include "uart.h"

#include "main_state.h"
#include "main_types.h"
#include "main.h"

/* Defines */
#define MS_DELAY            500U

/* Static variables */
static uint8_t test = 0;
static uart_t_reg _main_uart_reg = 
{
    .ubrrh = &UBRR0H,
    .ubrrl = &UBRR0L,
    .ucsra = &UCSR0A,
    .ucsrb = &UCSR0B,
    .ucsrc = &UCSR0C,
    .udr = &UDR0,
};

/* Static functions declarations */
static int8_t _main_init(void);
static int8_t _main_registers_init(void);
static int8_t _main_shutdown(void);
static int8_t _main_loop(void);

/* Functions */

int main (void)
{
    int8_t ret = 0;

    /* Init all the modules */
    //if (0 == ret)
        ret = _main_init();

    if (0 == ret)
        ret = _main_loop();

    if (0 == ret)
        _main_shutdown();

    ret = _main_loop();
    return ret;
}

/* Static functions definitions */
static int8_t _main_init(void)
{
    int8_t ret = 0;

    //if (0 == ret)
       ret = _main_registers_init();

    if (0 == ret)
        ret = uart_init(&_main_uart_reg, main_get_baudrate());

    if (0 == ret)
    {
        test = 1;
        main_set_state(MAIN_STATE_RUNNING);
    }

    return ret;
}

static int8_t _main_registers_init(void)
{
    int8_t ret = 0;

    sei();

    /* Specific registers for Atmega 328*/
    TCCR0A |= (1 << WGM01) | (1 << WGM00);
    TCCR0B |= (1 << CS01) | (1 << CS00);

    /* Timer 0 overflow interrupt */
    TIMSK0 |= (1 << TOIE0);

    /* Proper handling of timer 1&2 */
    TCCR1B = (1 << CS11) | (1 << CS10);

    /* Phase correct pwm mode for timer 1 */
    TCCR1A |= (1 << WGM10);

    /* Prescale factor 64 for timer 2 */
    TCCR2B |= (1 << CS22);

    /* Phase correct PWM for timer 2 */
    TCCR2A |= (1 << WGM20);

    /* Configure A2D prescaler to be inside the 50-200 kHz range */
#if F_CPU >= 16000000
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
#elif F_CPU >= 8000000
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
    ADCSRA &= ~(1 << ADPS0);
#endif

    /* Disconnect pins 0 and 1 from the USART */
    UCSR0B = 0;

    return ret;
}

static int8_t _main_shutdown(void)
{
    int8_t ret = 0;

    return ret;
}

static int8_t _main_loop(void)
{
    int8_t ret = 0;
    uint8_t m[] = "test";
    uint16_t size = 4;

    if (0 == ret)
        ret = uart_send_message(m, size);

    /*Set to one the fifth bit of DDRB to one
     **Set digital pin 13 to output mode */
    DDRB |= _BV(DDB5);

    main_set_state(MAIN_STATE_RUNNING);
    while (MAIN_STATE_RUNNING == main_get_state()) {
        /*Set to one the fifth bit of PORTB to one
         **Set to HIGH the pin 13 */
        PORTB |= _BV(PORTB5);

        /*Wait 3000 ms */
        _delay_ms(MS_DELAY);

        /*Set to zero the fifth bit of PORTB
         **Set to LOW the pin 13 */
        PORTB &= (int8_t) ~_BV(PORTB5);

        /*Wait 3000 ms */
        _delay_ms(MS_DELAY);
    }

    return ret;
}

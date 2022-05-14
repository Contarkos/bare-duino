/* Global includes */
#include <stdint.h> /* int8_t */
#include <avr/io.h>
#include <util/delay.h>

/* Local includes */
#include "uart.h"

#include "main_types.h"

/* Defines */
#define MS_DELAY            3000

#define MAIN_UART_BAUDRATE  115200


/* Static variables */
static main_internal_t main_config =
{
    .uart_baudrate = MAIN_UART_BAUDRATE,
    .is_running = MAIN_STATE_INIT,
};

/* Static functions declarations */
static int _main_init(void);
static int _main_shutdown(void);
static int _main_loop(void);

/* Functions */

int main (void)
{
    int ret = 0;

    /* Init all the modules */
    if (0 == ret)
        ret = _main_init();

    if (0 == ret)
        ret = _main_loop();

    if (0 == ret)
        _main_shutdown();

    return ret;
}

/* Static functions definitions */
static int _main_init(void)
{
    int ret = 0;

    if (0 == ret)
        ret = uart_init(main_config.uart_baudrate);

    if (0 == ret)
        main_config.is_running = MAIN_STATE_RUNNING;

    return ret;
}

static int _main_shutdown(void)
{
    int ret = 0;

    return ret;
}

static int _main_loop(void)
{
    int ret = 0;
    char m[] = "test";
    uint16_t size = 4;

    if (0 == ret)
        ret = uart_send_message(m, size);

    /*Set to one the fifth bit of DDRB to one
     **Set digital pin 13 to output mode */
    DDRB |= _BV(DDB5);

    while (MAIN_STATE_RUNNING == main_config.is_running) {
        /*Set to one the fifth bit of PORTB to one
         **Set to HIGH the pin 13 */
        PORTB |= _BV(PORTB5);

        /*Wait 3000 ms */
        _delay_ms(MS_DELAY);

        /*Set to zero the fifth bit of PORTB
         **Set to LOW the pin 13 */
        PORTB &= ~_BV(PORTB5);

        /*Wait 3000 ms */
        _delay_ms(MS_DELAY);
    }

    return ret;
}

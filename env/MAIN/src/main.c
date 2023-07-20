/* Global includes */
#include <stdint.h> /* int8_t */
#include <avr/io.h>
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

/* Static functions declarations */
static int8_t _main_init(void);
static int8_t _main_shutdown(void);
static int8_t _main_loop(void);

/* Functions */

int main (void)
{
    int8_t ret = 0;

    /* Init all the modules */
    if (0 == ret)
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

    if (0 == ret)
        ret = uart_init(main_get_baudrate());

    if (0 == ret)
    {
        test = 1;
        main_set_state(MAIN_STATE_RUNNING);
    }

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
    char m[] = "test";
    uint16_t size = 4;

    if (0 == ret)
        ret = uart_send_message(m, size);

    /*Set to one the fifth bit of DDRB to one
     **Set digital pin 13 to output mode */
    DDRB |= _BV(DDB5);

    //main_config.is_running = MAIN_STATE_RUNNING;
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


/* Global includes */
#include <stdint.h> /* int8_t, int32_t */

/* Local includes */
#include "main_types.h"

#include "main_state.h"

/* Defines */
#define MAIN_UART_BAUDRATE  9600U

/* Static variables */
static main_internal_t main_config =
{
    .uart_baudrate = MAIN_UART_BAUDRATE,
    .is_running = MAIN_STATE_INIT,
};

/* Functions */
int8_t main_set_state(main_state_t i_state)
{
    int8_t ret = 0;

    /* TODO : check transition */
    switch (i_state)
    {
        case MAIN_STATE_INIT:
        case MAIN_STATE_RUNNING:
        case MAIN_STATE_SHUTDOWN:
            main_config.is_running = i_state;
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}

main_state_t main_get_state(void)
{
    return main_config.is_running;
}

uint32_t main_get_baudrate(void)
{
    return main_config.uart_baudrate;
}

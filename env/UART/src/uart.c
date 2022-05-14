/* Global includes */
#include <stdint.h> /* int8_t */

/* Local includes */
#include "uart.h"

int8_t uart_init(uint32_t i_baudrate)
{
    int8_t ret = 0;

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


/* Basic utilities */

#ifndef _UTILS_H
#define _UTILS_H

#include <util/delay.h>

#define _do_blink(ms) do { \
    PORTB |= _BV(PORTB5); \
    _delay_ms(ms); \
    PORTB &= (uint8_t) ~_BV(PORTB5); \
} while (0);

        
#endif /* _UTILS_H */
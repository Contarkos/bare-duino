#include "main_types.h"

#ifndef _MAIN_STATE_H
#define _MAIN_STATE_H

int8_t main_set_state(main_state_t i_state);
main_state_t main_get_state(void);

uint32_t main_get_baudrate(void);

#endif


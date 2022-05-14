#ifndef _MAIN_TYPES_H
#define _MAIN_TYPES_H

typedef enum main_state_e
{
    MAIN_STATE_INIT = 0,
    MAIN_STATE_RUNNING = 1,
    MAIN_STATE_SHUTDOWN = 2
} main_state_t;

typedef struct main_internal_s
{
    uint32_t uart_baudrate;
    main_state_t is_running;
} main_internal_t;

#endif /* _MAIN_TYPES_H */

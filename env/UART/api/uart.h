#ifndef _UART_H
#define _UART_H

int8_t uart_init(uint32_t i_baudrate);

int8_t uart_send_message(char *m, uint16_t i_size);

int8_t uart_read(char *m, uint16_t *o_size);

#endif /* _UART_H */

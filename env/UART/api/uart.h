#ifndef _UART_H
#define _UART_H

typedef struct {
    volatile uint8_t *ubrrh;
    volatile uint8_t *ubrrl;
    volatile uint8_t *ucsra;
    volatile uint8_t *ucsrb;
    volatile uint8_t *ucsrc;
    volatile uint8_t *udr;
} uart_t_reg;

int8_t uart_init(uart_t_reg *i_reg, uint32_t i_baudrate);

int8_t uart_send_message(uint8_t *m, uint16_t i_size);

int8_t uart_read(uint8_t *m, uint16_t *o_size);

#endif /* _UART_H */

#ifndef UART_LAYER_H
#define	UART_LAYER_H

#include <xc.h> 
#include <stdint.h>
#include <stdbool.h>
#include <pic18f4520.h>

#define MAX_SEGMENTS 40

extern volatile bool snake_updated;
extern volatile uint8_t snake_len_rx;
extern volatile uint8_t snake_x_rx[MAX_SEGMENTS];
extern volatile uint8_t snake_y_rx[MAX_SEGMENTS];
extern volatile bool apple_eaten;

void uart_init(uint16_t gen_reg, unsigned sync,unsigned brgh, unsigned brg16);
void uart_send(uint8_t c);
void  uart_rx_from_player(uint8_t c);
void uart_send_array(uint8_t *c,uint16_t len);
void uart_send_string(uint8_t *c);


#endif	
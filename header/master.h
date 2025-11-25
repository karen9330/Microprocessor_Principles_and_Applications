#ifndef MASTER_H
#define	MASTER_H

#include <xc.h> 
#include <stdint.h>
#include <stdbool.h>
#include <pic18f4520.h>

void system_init(void);
void timer0_init(void) ;
void generate_apple(uint8_t *x, uint8_t *y);
void send_apple_pos(uint8_t x, uint8_t y);
void handle_game_over();

#endif	

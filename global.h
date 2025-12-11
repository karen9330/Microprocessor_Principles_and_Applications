#ifndef GLOBAL_H
#define	GLOBAL_H

#include <xc.h> 
#include <stdint.h>
#include <stdbool.h>
#include <pic18f4520.h>

#define MAX_SEGMENTS 50
#define GRID_SIZE 16
#define GAME_OVER 0
#define SNAKE 1
#define START 1
#define APPLE 2
#define EMPTY 3

typedef struct {
    uint8_t x;
    uint8_t y;
}Pos;

extern volatile Pos pos[5];
extern volatile uint16_t sec_counter;
extern volatile bool game_start;
extern volatile bool is_game_over;
extern volatile bool apple_eaten;
extern volatile bool snake_updated;


// test with putty
extern volatile bool  uart_line_ready;
extern volatile char  uart_rx_buf[32];
extern volatile uint8_t uart_rx_idx;
extern char buff[32];

#endif
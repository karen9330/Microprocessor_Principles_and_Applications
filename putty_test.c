#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "uart_layer.h"

static void feed_snake_packet(uint8_t *pos)
{
    uart_rx_from_player(SNAKE);
    for(int i=0; i<6; i++) {
        uart_rx_from_player(pos[i]);
    }
}

void parse_command_from_pc(char *line)
{   
    // receive from putty as format: S,0,0,0,1,0,2,0 
    // [msg_type][posx(0~5)][posy(0~5)][flag]
    
    if (line[0] == 'G') {
        // Game over
        uart_rx_from_player(GAME_OVER);
    }
    else if (line[0] == 'S') {
        uint8_t pos[6], idx = 0, flag;
        
        char *p = line + 2;
        char *token = strtok(p, ",\r\n");
        while (token != NULL) {
            int val = atoi(token);
            pos[idx++] = (uint8_t)val;
            token = strtok(NULL, ",\r\n");
        }
        feed_snake_packet(pos);
    }
}

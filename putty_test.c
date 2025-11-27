#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "uart_layer.h"

static void feed_snake_packet(uint8_t *pos, uint8_t flag)
{
    uart_rx_from_player(SNAKE);
    for(int i=0; i<6; i++) {
        uart_rx_from_player(pos[i]);
    }
    uart_rx_from_player(flag);
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
        for(int i=1; line[i] != '\0'; i++) {
            if(line[i] == ',') continue;
            pos[idx++] = line[i] - 48;
        }
        flag = line[strlen(line) - 1] - 48;
        feed_snake_packet(pos, flag);
    }
}

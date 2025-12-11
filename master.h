#ifndef MASTER_H
#define	MASTER_H

#include <xc.h> 
#include <stdint.h>
#include <stdbool.h>
#include <pic18f4520.h>
#include "global.h"

void system_init(void);
void play_game(int apple_x, int apple_y); 

#endif	
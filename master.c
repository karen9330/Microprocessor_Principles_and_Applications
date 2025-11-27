#include <xc.h>
#include <pic18f4520.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "conbits.h"
#include "master.h"
#include "uart_layer.h"
#include "global.h"

static void send_apple_pos(uint8_t x, uint8_t y) {
    uint8_t buff[2];
    buff[0] = x;
    buff[1] = y; 
    uart_send_array(buff, 2);

    // test with putty
    /*char buff[32];
    sprintf(buff, "APPLE x=%u y=%u\r\n", x, y);
    uart_send_array((uint8_t *)buff, strlen(buff));*/
}

void timer0_init(void) {
    // seting RD4
    TRISDbits.RD5 = 0;
    LATDbits.LATD5 = 0;
    
    // seting timer0 
    T0CONbits.PSA = 0;      // prescaler: 1:256
    T0CONbits.T0PS = 0b111;
    T0CONbits.T08BIT = 0;   // using 16 bits
    T0CONbits.T0CS = 0;     // internal instruction cycle clock 
    
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    INTCON2bits.TMR0IP = 1;
    TMR0H = 0xE1;   // every 1 sec issues an intrrupt
    TMR0L = 0x7C;
}

void system_init(void) {
    // set oscillation
    OSCCONbits.IDLEN = 0;   // Device enters Sleep mode on SLEEP instruction
    OSCCONbits.IRCF = 0x07; // 8 MHz;
    OSCCONbits.SCS = 0x03;  // Internal oscillator
    
    // using RB0 as button input
    ADCON1 = 0x0F;      // use digital I/O
    TRISBbits.RB0 = 1;    
    LATBbits.LATB0 = 0;
    INTCONbits.INT0IE = 1;
    INTCONbits.INT0IF = 0;
    
    // interrupt setting
    RCONbits.IPEN = 1;   // turn on the priority
    INTCONbits.GIEH = 1; // Enables all high-priority interrupts
    INTCONbits.GIEL = 1; // Enables all low-priority peripheral interrupts
}

void generate_apple(uint8_t *x, uint8_t *y) {
    uint16_t max_x = 480 / GRID_SIZE - 1;
    uint16_t max_y = 320 / GRID_SIZE - 1;
    
    *x = rand() % max_x;
    *y = rand() % max_y;
    send_apple_pos(*x, *y); 
}

void handle_game_over() {
    // tft_draw(game over)
    // send signal to music
    
    // test for putty
    /*char buff[32];
    sprintf(buff, "GAME OVER\r\n");
    uart_send_array((uint8_t *)buff, strlen(buff));*/
}


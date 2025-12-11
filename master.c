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
#include "putty_test.h"

static bool first_frame = true;

// Not used
static void send_apple_pos(uint8_t x, uint8_t y) {
    /*uint8_t buff[2];
    buff[0] = x;
    buff[1] = y; 
    uart_send_array(buff, 2);*/

    // test with putty
    sprintf(buff, "APPLE x=%u y=%u\r\n", x, y);
    uart_send_array((uint8_t *)buff, strlen(buff));
}

static void timer_init(void) {
    // seting timer0 
    T0CONbits.PSA = 0;      // prescaler: 1:256
    T0CONbits.T0PS = 0b111;
    T0CONbits.T08BIT = 0;   // using 16 bits
    T0CONbits.T0CS = 0;     // internal instruction cycle clock 
    
    INTCONbits.TMR0IF = 0;
    INTCON2bits.TMR0IP = 1;
    T0CONbits.TMR0ON = 1; // turn on timer0
    TMR0H = 0xE1;   // every 1 sec issues an intrrupt
    TMR0L = 0x7C;
    
    // setting timer1 for srand
    T1CONbits.T1CKPS = 0b01;    // presaclar = 1:2
    T1CONbits.T1OSCEN = 1;
    T1CONbits.TMR1CS = 0;
    T1CONbits.TMR1ON = 1;   // turn on timer1
}

static void io_init(void) {
    // using RB0 as button input
    ADCON1 = 0x0F;      // use digital I/O
    TRISBbits.RB0 = 1;    
    LATBbits.LATB0 = 0;
    INTCONbits.INT0IE = 1;
    INTCONbits.INT0IF = 0;
    
    // using RD4 as ouput to test game start
    TRISDbits.RD4 = 0;
    LATDbits.LATD4 = 1;
    
    // using RD5 ~ RD7
    TRISDbits.RD5 = 0;
    LATDbits.LATD5 = 0;
    
    TRISDbits.RD6 = 0;
    LATDbits.LATD6 = 0;
    
    TRISDbits.RD7 = 0;
    LATDbits.LATD7 = 0;
}

static void generate_apple(uint8_t *x, uint8_t *y, Pos *pos, uint8_t len) {
    static uint16_t max_x = CELL_W;
    static uint16_t max_y = CELL_H;
    bool is_different = false;
    
    while(!is_different) {
        *x = rand() % max_x;
        *y = rand() % max_y;
        is_different = true;
        for(int i=0; i< len; i++) {
            if((pos[i].x == *x) && (pos[i].y == *y)) {
                is_different = false;
                break;
            }
        } 
    }
}

static bool check_apple_eaten(uint8_t apple_x,  uint8_t apple_y, Pos *pos) {
    if((pos[0].x == apple_x) && (pos[0].y == apple_y))
        return true;
    return false;
}

static void srand_init() {
    uint16_t seed = (TMR1H << 8) | TMR1L;
    srand(seed);
    //sprintf(buff, "seed=%u\r\n", seed);
    //uart_send_array((uint8_t *)buff, strlen(buff));
}

static void notice_to_music(int type) {
    if(type == START) {     // start
        LATDbits.LATD5 = 1;
        LATDbits.LATD5 = 0;
    }
    else if(type == APPLE) {    // apple
        LATDbits.LATD6 = 1;
        LATDbits.LATD6 = 0;
    }
    else if(type == GAME_OVER){
        LATDbits.LATD7 = 1;     // game over
        LATDbits.LATD7 = 0;
    }
    
}

static void hand_game_start() {
    srand_init();
    notice_to_music(START);
    uart_send(1);
    INTCONbits.TMR0IE = 1;  // start timer0 interrupt
}

static void handle_game_over() {
    uart_send(0);
    notice_to_music(GAME_OVER);
    
    LATDbits.LATD4 = 1;     // test for game start
    INTCONbits.TMR0IE = 0;  // disable timer0 interrupt
    
    // reset the states
    sec_counter = 0;
    apple_eaten = false;
    is_game_over = false;
    snake_updated = false;
    game_start = false;
    first_frame = true;
    
    // test for putty
    //uart_send_array("GAME OVER\r\n", 12);
}

void system_init(void) {
    // set oscillation
    OSCCONbits.IDLEN = 0;   // Device enters Sleep mode on SLEEP instruction
    OSCCONbits.IRCF = 0x07; // 8 MHz;
    OSCCONbits.SCS = 0x03;  // Internal oscillator
    
    timer_init();
    io_init();
    
    // interrupt setting
    RCONbits.IPEN = 1;   // turn on the priority
    INTCONbits.GIEH = 1; // Enables all high-priority interrupts
    INTCONbits.GIEL = 1; // Enables all low-priority peripheral interrupts
}

void play_game(int apple_x, int apple_y) {
    while(!game_start); // wait for starting
    hand_game_start();
    LATDbits.LATD4 = 0;  // test for game start
    //uart_send_array("GAME START\r\n", 12);
    
    while(1){
        // test with putty
        /*if(uart_line_ready) {
            parse_command_from_pc(uart_rx_buf);
            uart_line_ready = false;
            for(int i = 0; i < 32 ; i++)
               uart_rx_buf[i] = '\0';
            uart_rx_idx = 0;
        }*/
        
        if(snake_updated) {
            if (first_frame) {
                generate_apple(&apple_x, &apple_y, pos, 3);

                first_frame = false;
            } 
            else {

            }
            // test for putty
            /*sprintf(buff, "APPLE=%u, %u\r\n", apple_x, apple_y);
            uart_send_array((uint8_t *)buff, strlen(buff));
            for(int i=0; i<3; i++) {
                sprintf(buff, "%u,%u\r\n", pos[i].x, pos[i].y);
                uart_send_array((uint8_t *)buff, strlen(buff));
            }*/
            
            // check apple is eaten
            if(check_apple_eaten(apple_x, apple_y, pos)) {
                uart_send_array("APPLE EATEN\r\n", 13);
                apple_eaten = true;
            }
            snake_updated = false;
        }
        
        if(apple_eaten) {
            // tft_draw(apple_x, apple_y, BACKGROUND);
            generate_apple(&apple_x, &apple_y, pos, 3);
            notice_to_music(APPLE);
            // tft_draw(apple_x, apple_y);
            apple_eaten = false;
        }
        
        if(is_game_over) {
            handle_game_over();
            break;
        }
    } 
}
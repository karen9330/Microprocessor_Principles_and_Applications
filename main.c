#include <xc.h>
#include <pic18f4520.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "conbits.h"
#include "uart_layer.h"
#include "master.h"
#include "global.h"

#include "putty_test.h"
static volatile uint16_t sec_counter = 0;
static volatile bool game_start = false;

volatile uint8_t snake_len_rx = 3;
volatile uint8_t snake_x_rx[MAX_SEGMENTS] = {0};
volatile uint8_t snake_y_rx[MAX_SEGMENTS] = {0};
volatile bool apple_eaten = false;
volatile bool is_game_over = false;
volatile bool snake_updated = false;
    
// test for putty
volatile bool  uart_line_ready = false;
volatile char  uart_rx_buf[32];
volatile uint8_t uart_rx_idx = 0;

void __interrupt() high_isr(void){
    if(PIR1bits.RCIF){  // uart interrupt
        if(RCSTAbits.FERR){
            uint8_t er = RCREG;
        }
        else if(RCSTAbits.OERR){
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
        /*else{
            uint8_t c = RCREG;
            uart_rx_from_player(c);
        }*/
        else {
            char c = RCREG;
            if(!uart_line_ready) {
                if(c == '\r') {
                    uart_rx_buf[uart_rx_idx] = '\0';
                    uart_send('\r');
                    uart_send('\n');
                    uart_line_ready = true;
                }
                else {
                    uart_rx_buf[uart_rx_idx++] = c;
                    uart_send(c);
                }
            }
        }
        PIR1bits.RCIF=0;
    }
    if(INTCONbits.TMR0IF) { // timer0 interrupt
        TMR0H = 0xE1;
        TMR0L = 0x7C;
        LATDbits.LATD2 = 0;
        
        sec_counter++;
        if (sec_counter >= 2 * 60) { // when count to 120s, the game is stopped
            sec_counter = 0;
            // using RD2 to notify player?
            LATDbits.LATD2 = 1;
            uart_send_array( "TIME\r\n", 6);
            is_game_over = true;
        }
        INTCONbits.TMR0IF = 0;
    }
    if(INTCONbits.INT0IF) { // button interrupt
        game_start = true;
        INTCONbits.INT0IF = 0;
    }
}

void __interrupt(low_priority) low_isr(void){
    if(0){

    }
}

void main(void){
    
    uint8_t prev_tail_x = 0,  prev_tail_y = 0;
    uint8_t apple_x = 0, apple_y = 0;
    bool first_frame = true;

    system_init();
    
    uart_init(51,0,1,0); //baud 9600
    timer0_init();
    __delay_ms(200);
    
    TRISDbits.RD4 = 0;
    LATDbits.LATD4 = 1;
    while(!game_start); // wait for starting
    LATDbits.LATD4 = 0;
    T0CONbits.TMR0ON = 1; 
    
    uint16_t seed = (TMR0H << 8) | TMR0L;
    srand(seed);
    
    generate_apple(&apple_x, &apple_y);
    send_apple_pos(apple_x, apple_y); 
    // tft_draw(apple_x * GRID_SIZE, apple_y * GRID_SIZE, RED)
    while(1){
        // test for putty
        if(uart_line_ready) {
            parse_command_from_pc(uart_rx_buf);
            uart_line_ready = false;
            for(int i = 0; i < 32 ; i++)
               uart_rx_buf[i] = '\0';
            uart_rx_idx = 0;
        }
        if(snake_updated) {
            if (first_frame) {

                for (uint8_t i = 0; i < snake_len_rx; i++) {
                    //tft_draw(snake_x_rx[i], snake_y_rx[i], GREEN);
                }

                prev_tail_x = snake_x_rx[snake_len_rx - 1];
                prev_tail_y = snake_y_rx[snake_len_rx - 1];
                first_frame = false;
            } 
            else {
                //tft_draw(snake_x_rx[0], snake_y_rx[0], GREEN)
                //tft_draw(prev_tail_x, prev_tail_x, BACKGROUND)

                prev_tail_x = snake_x_rx[snake_len_rx - 1];
                prev_tail_y = snake_y_rx[snake_len_rx - 1];
            }
            
            snake_updated = false;
        }
        
        if(apple_eaten) {
            // tft_draw(apple_x * GRID_SIZE, apple_y * GRID_SIZE, BACKGROUND);
            // If the apple is eaten, then send the apple position to player
            generate_apple(&apple_x, &apple_y);
            send_apple_pos(apple_x, apple_y); 
            
            // tft_draw(apple_x * GRID_SIZE, apple_y * GRID_SIZE, RED);
            apple_eaten = false;
        }
        
        if(is_game_over) {
            uart_send_array("MAIN\r\n", 6);
            handle_game_over();
            break;
        }
    } 
    
    while(1) {
        // for test
        TRISDbits.RD7 = 0;
        LATDbits.LATD7 = 1;
    }
}

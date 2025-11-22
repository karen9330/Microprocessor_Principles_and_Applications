#include <xc.h>
#include <pic18f4520.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "conbits.h"
#include "uart_layer.h"
#include "master.h"

volatile uint16_t sec_counter = 0;
volatile bool game_start = false;
volatile uint8_t uart_char;

void __interrupt() high_isr(void){
    if(PIR1bits.RCIF){  // uart interrupt
        if(RCSTAbits.FERR){
            uint8_t er = RCREG;
        }
        else if(RCSTAbits.OERR){
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
        else{
            uint8_t c = RCREG;
            uart_rx_from_player(c);
        }
        PIR1bits.RCIF=0;
    }
    if(INTCONbits.TMR0IF) { // timer0 interrupt
        TMR0H = 0xE1;
        TMR0L = 0x7C;
        LATDbits.LATD2 = 0;
        
        sec_counter++;
        if (sec_counter >= 120) { // when count to 120s, the game is stopped
            sec_counter = 0;
            // using RD2 to notify player
            LATDbits.LATD2 = 1;
            
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
    system_init();
    uint16_t apple_x, apple_y;

    uart_init(51,0,1,0); //baud 9600
    timer0_init();
    
    __delay_ms(2000);
    
    TRISDbits.RD1 = 0;
    LATDbits.LATD1 = 1;
    while(!game_start); // wait for starting
    LATDbits.LATD1 = 0;
    
    generate_apple(&apple_x, &apple_y);
    send_apple_pos(apple_x, apple_y); 
    // tft_draw(apple_x * 16, apple_y * 16, RED)
    while(1){
        if(snake_updated) {
            for (uint8_t i = 0; i < snake_len_rx; i++) {
                uint16_t px = snake_x_rx[i] * 16;
                uint16_t py = snake_y_rx[i] * 16;
                
                //tft_draw(px, py * 16, GREEN)
            }
            
            snake_updated = false;
        }
        
        if(apple_eaten) {
            // tft_draw(apple_x * 16, apple_y * 16, BACKGROUND);
            // If the apple is eaten, then send the apple position to player
            generate_apple(&apple_x, &apple_y);
            send_apple_pos(apple_x, apple_y); 
            
            // tft_draw(apple_x * 16, apple_y * 16, RED);
            apple_eaten = false;
        }
        
    } 
}

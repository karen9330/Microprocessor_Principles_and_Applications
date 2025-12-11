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

volatile uint16_t sec_counter = 0;   // timer0 counter
volatile bool apple_eaten = false;
volatile bool is_game_over = false;
volatile bool snake_updated = false;
volatile bool game_start = false;
volatile Pos pos[5];

// test for putty
volatile bool  uart_line_ready = false;
volatile char  uart_rx_buf[32];
volatile uint8_t uart_rx_idx = 0;
char buff[32];

void __interrupt() high_isr(void){
    if(INTCONbits.TMR0IF) { // timer0 interrupt
        TMR0H = 0xE1;
        TMR0L = 0x7C;
        
        sec_counter++;
        if (sec_counter >= 2 * 60) { // when count to 120s, the game is stopped
            sec_counter = 0;
            
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
        // test with putty
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
}

void main(void){
    uint8_t apple_x = 0,  apple_y = 0;

    system_init(); // osscon = 8MHz
    
    uart_init(51,0,1,0); //baud 9600
    __delay_ms(200);
    
    while(1) {
        play_game(apple_x, apple_y);
        __delay_ms(200);
        uart_send_array("RESTART\r\n", 9);
    }
    
}

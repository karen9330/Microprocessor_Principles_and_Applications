#include <xc.h>
#include <string.h>
#include <stdio.h>
#include "uart_layer.h"
#include "global.h"

typedef enum{
    RX_WAIT_MSG_TYPE = 0,
    //RX_WAIT_LEN,
    RX_WAIT_DATA,
    RX_WAIT_APPLE,
} uart_rx_state_t;

void uart_init(uint16_t gen_reg, unsigned sync,unsigned brgh, unsigned brg16){
    TRISCbits.RC7 = 1;
    TRISCbits.RC6 = 1;
    
    // baud rate
    SPBRGH = (gen_reg & 0xFF00) >> 8;
    SPBRG = gen_reg & 0x00FF;
    
    RCSTAbits.CREN = 1;     // continue receive
    RCSTAbits.SPEN = 1;     // serial port enable
    BAUDCONbits.BRG16 = brg16;
    
    TXSTAbits.SYNC = sync;
    TXSTAbits.BRGH = brgh;
    TXSTAbits.TXEN = 1;     // transmition enable
    
    IPR1bits.RCIP = 0;     // low priority
    PIE1bits.RCIE = 1;  
    PIR1bits.RCIF = 0;
}

void uart_send(uint8_t c){
    while(TXSTAbits.TRMT==0){
        Nop();
    }
    TXREG = c;
}

void  uart_rx_from_player(uint8_t c) {
    static volatile uart_rx_state_t rx_state = RX_WAIT_MSG_TYPE;
    static volatile uint8_t rx_index = 0;
    
    switch(rx_state) {
        case RX_WAIT_MSG_TYPE:
            if(c == GAME_OVER) {
                is_game_over = true;
                rx_state = RX_WAIT_MSG_TYPE;
                uart_send_array("UART\r\n", 6);
            }
            else {
                rx_index = 0;
                rx_state = RX_WAIT_DATA;
                //uart_send_array("STATE 1\r\n", 9);
            }
            break;
        case RX_WAIT_DATA:
            if((rx_index & 1) == 0) {   // even: x
                pos[rx_index/2].x = c;
            }
            else {  // odd: y
                pos[rx_index/2].y = c;
            }
            //uart_send_array("STATE 2\r\n", 9);
            rx_index++;
            if(rx_index >= 6) {
                rx_state = RX_WAIT_MSG_TYPE;
                snake_updated = true;   
            }
            break;
        case RX_WAIT_APPLE:     // Not used
            snake_updated = true;   
            uart_send_array("STATE 3\r\n", 9);
            rx_state = RX_WAIT_MSG_TYPE;
            break;
    }
}

void uart_send_array(uint8_t *c, uint16_t len){
    for(uint8_t i = 0; i < len;i++){
        uart_send(c[i]);
    }
}

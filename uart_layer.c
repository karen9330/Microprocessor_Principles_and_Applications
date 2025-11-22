#include <xc.h>
#include <string.h>
#include "uart_layer.h"

typedef enum{
    RX_WAIT_HEADER = 0,
    RX_WAIT_LEN,
    RX_WAIT_DATA,
    RX_WAIT_APPLE,
} uart_rx_state_t;

volatile uart_rx_state_t rx_state = RX_WAIT_HEADER;
volatile uint8_t rx_index = 0;
volatile uint8_t snake_len_rx = 0;
volatile uint8_t snake_x_rx[MAX_SEGMENTS] = {0};
volatile uint8_t snake_y_rx[MAX_SEGMENTS] = {0};
volatile bool apple_eaten = false;
volatile bool snake_updated = false;

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
    
    IPR1bits.RCIP = 1;
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
    switch(rx_state) {
        case RX_WAIT_HEADER:
            if(c == 0xFF) { 
                rx_state = RX_WAIT_LEN; 
                // clear after each transmission
                memset(snake_x_rx,0,sizeof(snake_x_rx));
                memset(snake_y_rx,0,sizeof(snake_y_rx));
            }
            break;
        case RX_WAIT_LEN:
            snake_len_rx = c;
            rx_index = 0;
            rx_state = RX_WAIT_DATA;
            break;
        case RX_WAIT_DATA:
            if((rx_index & 1) == 0) {   // even: x
                snake_x_rx[rx_index/2] = c;
            }
            else {  // odd: y
                snake_y_rx[rx_index/2] = c;
            }
            rx_index++;
            if(rx_index >= snake_len_rx * 2) {
                rx_state = RX_WAIT_APPLE;
            }
            break;
        case RX_WAIT_APPLE:
            if(c == 1) apple_eaten = true;
            snake_updated = true;   
            rx_state = RX_WAIT_HEADER;
            break;
    }
}

void uart_send_array(uint8_t *c, uint16_t len){
    for(uint8_t i = 0; i < len;i++){
        uart_send(c[i]);
    }
}

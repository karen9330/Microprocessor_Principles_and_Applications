#include <xc.h>
#include <pic18f4520.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "conbits.h"
#include "master.h"
#include "uart_layer.h"

void timer0_init(void) {
    // seting RD2
    TRISDbits.RD2 = 0;
    LATDbits.LATD2 = 0;
    
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
    T0CONbits.TMR0ON = 1; 
}

void system_init(void) {
    // set oscillation
    OSCCONbits.IDLEN = 0;   // Device enters Sleep mode on SLEEP instruction
    OSCCONbits.IRCF = 0x07; // 8 MHz;
    OSCCONbits.SCS = 0x03;  // Internal oscillator block
    while(OSCCONbits.IOFS!=1);
    
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

void generate_apple(uint16_t *x, uint16_t *y) {
    uint16_t max_x = 480;
    uint16_t max_y = 320;
    uint16_t min = 0;
    
    srand(time(NULL));
    *x = rand() % (max_x - min + 1) + min;
    *y = rand() % (max_y - min + 1) + min;
}

void send_apple_pos(uint16_t x, uint16_t y) {
    uint8_t buff[4];
    buff[0] = (x >> 8) & 0xFF;  // high bytes x
    buff[1] = x & 0xFF;     // low bytes x
    buff[2] = (y >> 8) & 0xFF;  // high bytes y
    buff[3] = y & 0xFF;     // low bytes y
    uart_send_array(buff, 4);
}


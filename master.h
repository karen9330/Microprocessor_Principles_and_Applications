#ifndef MASTER_H
#define	MASTER_H

#define SPI_DIR_SDO TRISCbits.RC5   // output to slave (data)
#define SPI_DIR_CLK TRISCbits.RC3   // clock
#define SPI_DIR_SDI TRISCbits.RC4   // input form slave (not used)
#define SPI_DIR_CS  TRISEbits.RE2   // Master mode do not need to use Slave Select pin
#define SPI_CS LATEbits.LE2

#include <xc.h> 
#include <stdint.h>
#include <stdbool.h>
#include <pic18f4520.h>

extern volatile bool spi_rx_data_ready;

void system_init(void);
void timer0_init(void) ;
void generate_apple(uint16_t *x, uint16_t *y);
void send_apple_pos(uint16_t x, uint16_t y);

#endif	
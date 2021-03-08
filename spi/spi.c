#include <avr/io.h>

#include "spi.h"

void spi_init( ) {
    // config these pin as output
    DDRB |=
        _BV( DDB5 ) | // sck
        _BV( DDB3 ) | // mosi
        _BV( DDB2 ); // ss
    PORTB |= _BV( PORTB2 ); // ss default high
    SPCR =
        _BV( SPE ) | // enable SPI
        _BV( MSTR ) | // set as master
        _BV( SPR1 ); // select prescaler 64
}

unsigned char spi_txrx( unsigned char d ) {
    // write data to register to load it and start shifting
    SPDR = d;
    // wait until shifting finish
    loop_until_bit_is_set( SPSR, SPIF );
    // data received from slave is also shift into our register while we shifted out
    return SPDR;
}

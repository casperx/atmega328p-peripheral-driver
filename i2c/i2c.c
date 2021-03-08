#include <avr/io.h>

#include "i2c.h"

void i2c_init( ) {
    // enable pin pullup
    DDRC &= ~(
        _BV( DDC4 ) | // sda
        _BV( DDC5 ) // scl
    );
    PORTC |=
        _BV( PORTC4 ) | // sda
        _BV( PORTC5 ); // scl
    TWBR = 8; // bit rate 100 K @ clock freq 8 M
    TWSR = _BV( TWPS0 ); // prescaler 4
    TWCR |= _BV( TWEN ); // enable i2c
}

unsigned char i2c_status( ) {
    // extract only status
    return ( TWSR & 0xF8 );
}

void i2c_wait( ) {
    // wait until something happen
    loop_until_bit_is_set( TWCR, TWINT );
}

void i2c_start( ) {
    TWCR |=
        _BV( TWINT ) | // start operation
        _BV( TWSTA ); // send start condition
    i2c_wait( );
}

void i2c_stop( ) {
    TWCR |=
        _BV( TWINT ) | // start operation
        _BV( TWSTO ); // send stop condition
}

void i2c_write( unsigned char data ) {
    TWDR = data;
    // do not send start and stop condition
    TWCR &= ~(
        _BV( TWSTA ) |
        _BV( TWSTO )
    );
    TWCR |= _BV( TWINT ); // start operation
    i2c_wait( );
}

unsigned char i2c_read( unsigned char last ) {
    // set ack for next data received
    if ( last )
        TWCR &= ~_BV( TWEA );
    else
        TWCR |= _BV( TWEA );
    i2c_wait( );
    // get data
    return TWDR;
}

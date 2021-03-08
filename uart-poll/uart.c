#include <avr/io.h>

#include "uart.h"

void uart_init( ) {
    UBRR0 = 12; // uart0 bit rate 38.4K @ clock freq 8 MHz
    UCSR0B =
        _BV( RXEN0 ) | // enable uart0 rx
        _BV( TXEN0 ); // enable uart0 tx
    UCSR0C =
        _BV( UCSZ00 ) | // uart0 8 data bit, 1 stop bit
        _BV( UCSZ01 );
}

void uart_putc( const char c ) {
    // wait until buffer empty
    loop_until_bit_is_set( UCSR0A, UDRE0 );
    // write data to register to start sending
    UDR0 = c;
}

char uart_getc( ) {
    // Wait for data to be received
    loop_until_bit_is_set( UCSR0A, RXC0 );
    // Get and return received data from buffer
    return UDR0;
}

void uart_puts( const char *s ) {
    // send NUL-terminated string
    while ( *s ) uart_putc( *s++ );
}

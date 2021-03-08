#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"

enum calculated {
    TX_LOG2_SZ = 4,
    RX_LOG2_SZ = 6,

    TX_SZ = 1 << TX_LOG2_SZ,
    RX_SZ = 1 << RX_LOG2_SZ,

    TX_MSK = TX_SZ - 1,
    RX_MSK = RX_SZ - 1
};

static volatile char txdat[ TX_SZ ];
static volatile char rxdat[ RX_SZ ];

static volatile unsigned char txhead, txtail;
static volatile unsigned char rxhead, rxtail;

// transmitter register is empty
ISR( USART_UDRE_vect ) {
    // transmit buffer is empty
    if ( txhead == txtail ) {
        // disable interrupt until new data arrived
        UCSR0B &= ~_BV( UDRIE0 );
        return;
    }
    // load transmitter register with new data
    UDR0 = txdat[ txtail ];
    txtail = ( txtail + 1 ) & TX_MSK;
}

// receiver register data available
ISR( USART_RX_vect ) {
    unsigned char head;
    head = ( rxhead + 1 ) & RX_MSK;
    // receive queue is full
    if ( head == rxtail ) {
        // disable interrupt until have some space
        UCSR0B &= ~_BV( RXCIE0 );
        return;
    }
    // add data to receive buffer
    rxdat[ rxhead ] = UDR0;
    rxhead = head;
}

void uart_init( ) {
    UBRR0 = 12; // uart0 bit rate 38.4K @ clock freq 8 MHz
    UCSR0B =
        _BV( UDRIE0 ) | // enable uart0 tx interrupt
        _BV( RXCIE0 ) | // enable uart0 rx interrupt
        _BV( RXEN0 ) | // enable uart0 rx
        _BV( TXEN0 ); // enable uart0 tx
    UCSR0C =
        _BV( UCSZ00 ) | // uart0 8 data bit, 1 stop bit
        _BV( UCSZ01 );
    // initialize cursors
    txhead = 0;
    txtail = 0;
    rxhead = 0;
    rxtail = 0;
}

unsigned char uart_tx( char c ) {
    unsigned char head;
    head = ( txhead + 1 ) & TX_MSK;
    // transmit buffer is full
    if ( head == txtail ) return 0;
    // add data to transmit buffer
    txdat[ txhead ] = c;
    txhead = head;
    // start transmitter
    UCSR0B |= _BV( UDRIE0 );
    return 1;
}

unsigned char uart_rx( char c[ 1 ] ) {
    // receive buffer is empty
    if ( rxhead == rxtail ) return 0;
    // copy data to output
    *c = rxdat[ rxtail ];
    rxtail = ( rxtail + 1 ) & RX_MSK;
    // start receivier
    UCSR0B |= _BV( RXCIE0 );
    return 1;
}

unsigned char uart_avail( ) {
    // get data size in receive buffer
    return ( rxhead - rxtail ) & RX_MSK;
}

unsigned char uart_remain( ) {
    // get data size in transmit buffer
    return ( txhead - txtail ) & TX_MSK;
}

void uart_putc( const char c ) {
    // try sending until success
    while ( ! uart_tx( c ) );
}

char uart_getc( ) {
    char c;
    // try receiving until success
    while ( ! uart_rx( &c ) );
    return c;
}

void uart_puts( const char *s ) {
    // send NUL-terminated string
    while ( *s ) uart_putc( *s++ );
}

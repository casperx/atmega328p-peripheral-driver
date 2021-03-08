#include <avr/io.h>

#include "adc.h"

void adc_init( ) {
    ADMUX = _BV( REFS0 ); // ref AVcc, select input ADC0
    ADCSRA =
        _BV( ADPS2 ) | // prescaler 64
        _BV( ADPS1 ) |
        _BV( ADEN ); // enable ADC
    DIDR0 |= _BV( ADC0D ); // disable digital input buffer on ADC0
}

unsigned int adc_read( ) {
    ADCSRA |= _BV( ADSC ); // start conversion
    // wait until conversion is finish
    loop_until_bit_is_clear( ADCSRA, ADSC );
    return ADC;
}

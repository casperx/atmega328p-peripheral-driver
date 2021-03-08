void uart_init( );

unsigned char uart_tx( char c );
unsigned char uart_rx( char c[ 1 ] );

unsigned char uart_avail( );
unsigned char uart_remain( );

void uart_putc( const char c );
void uart_puts( const char *s );
char uart_getc( );
